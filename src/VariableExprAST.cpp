#include "VariableExprAST.h"
#include "Codegen.h"

VariableExprAST::VariableExprAST(const std::string& name) : Name(name) {}

Value *VariableExprAST::codegen() {
	Value *V = Codegen::NamedValues[Name];
	if (!V)
		Codegen::LogErrorV("Unknown Variable Name");
	
	//load the value
	return Codegen::Builder->CreateLoad(Type::getDoubleTy(*Codegen::Thecontext), V, Name.c_str());
}

const std::string &VariableExprAST::getName() {
	return Name;
}
