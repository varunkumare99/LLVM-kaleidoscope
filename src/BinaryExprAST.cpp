#include "BinaryExprAST.h"
#include "VariableExprAST.h"
#include "Codegen.h"
BinaryExprAST::BinaryExprAST(SourceLocation Loc, char Op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
    :ExprAST(Loc), Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

Value *BinaryExprAST::codegen() {
	KSDbgInfo.emitLocation(this);
	//special case '=' because we don't want to emit the LHS as an expresson
	if (Op == '=') {
		//Assignment requires the LHS to be an identifier.
		VariableExprAST *LHSE = dynamic_cast<VariableExprAST*>(LHS.get());
		if (!LHSE)
			return Codegen::LogErrorV("destination of '=' must be a variable");
		
		//codegen the RHS
		Value *Val = RHS->codegen();
		if (!Val)
			return nullptr;

		//Lookup the name.
		Value *Variable = Codegen::NamedValues[LHSE->getName()];
		if (!Variable)
			return Codegen::LogErrorV("Unknown variable name");

		//store the RHS values in variable(alloca)
		Codegen::Builder->CreateStore(Val, Variable);
		return Val;
	}

	Value *L = LHS->codegen();
	Value *R = RHS->codegen();
	if (!L || !R)
		return nullptr;

	switch (Op) {
		case '+':
			return Codegen::Builder->CreateFAdd(L, R, "addtmp");
		case '-':
			return Codegen::Builder->CreateFSub(L, R, "subtmp");
		case '*':
			return Codegen::Builder->CreateFMul(L, R, "multmp");
		case '/':
			return Codegen::Builder->CreateFDiv(L, R, "divtmp");
		case '<':
			L = Codegen::Builder->CreateFCmpULT(L, R, "cmptmp");
			//convert 0 or 1 to double 0.0 or 1.0 since our language understands only doubles
			return Codegen::Builder->CreateUIToFP(L, Type::getDoubleTy(*Codegen::Thecontext), "booltmp");
		default:
			break;
	}

	// if it wasn't a built-in operator, it must be a user definied. Emit a call to it
	Function *F = Codegen::getFunction(std::make_pair(std::string("binary") + Op, 2));
	assert(F && "binary operator not found!");

	Value *Ops[2] = { L, R };
	return Codegen::Builder->CreateCall(F, Ops, "binop");
}

raw_ostream &BinaryExprAST::dump(raw_ostream &out, int ind) {
	ExprAST::dump(out << "binary" << Op, ind);
	LHS->dump(indent(out, ind) << "LHS:", ind + 1);
	RHS->dump(indent(out, ind) << "RHS:", ind + 1);
	return out;
}
