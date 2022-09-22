#include "BinaryExprAST.h"
#include "Codegen.h"
BinaryExprAST::BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
    :Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

Value *BinaryExprAST::codegen() {
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
