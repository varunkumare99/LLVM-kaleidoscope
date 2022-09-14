#include "NumberExprAST.h"
#include "Codegen.h"

NumberExprAST::NumberExprAST(double Val) : Val(Val) {}

Value *NumberExprAST::codegen() {
	return ConstantFP::get(*Codegen::Thecontext, APFloat(Val));
}
