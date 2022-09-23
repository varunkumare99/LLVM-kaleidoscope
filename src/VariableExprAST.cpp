#include "VariableExprAST.h"
#include "Codegen.h"

VariableExprAST::VariableExprAST(SourceLocation Loc, const std::string& name) :ExprAST(Loc),  Name(name) {}

Value *VariableExprAST::codegen() {
	Value *V = Codegen::NamedValues[Name];
	if (!V)
		Codegen::LogErrorV("Unknown Variable Name");
	
	KSDbgInfo.emitLocation(this);
	//load the value
	return Codegen::Builder->CreateLoad(Type::getDoubleTy(*Codegen::Thecontext), V, Name.c_str());
}

const std::string &VariableExprAST::getName() {
	return Name;
}

raw_ostream &VariableExprAST::dump(raw_ostream &out, int ind) {
	return ExprAST::dump(out << Name, ind);
}
