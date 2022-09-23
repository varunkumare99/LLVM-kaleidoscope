#include "NumberExprAST.h"
#include "Codegen.h"

NumberExprAST::NumberExprAST(double Val) : Val(Val) {}

Value *NumberExprAST::codegen() {
	KSDbgInfo.emitLocation(this);
	return ConstantFP::get(*Codegen::Thecontext, APFloat(Val));
}
raw_ostream &NumberExprAST::dump(raw_ostream &out, int ind) {
	return ExprAST::dump(out << Val, ind);
}
