#include "VariableExprAST.h"
#include "Codegen.h"

VariableExprAST::VariableExprAST(const std::string& name) : Name(name) {}

Value *VariableExprAST::codegen() {
	Value *V = Codegen::NamedValues[Name];
	if (!V)
		Codegen::LogErrorV("Unknown Variable Name");
	return V;
}
