#include "UnaryExprAST.h"
#include <Codegen.h>

UnaryExprAST::UnaryExprAST(char Opcode, std::unique_ptr<ExprAST> Operand)
	: Opcode(Opcode), Operand(std::move(Operand)) {}

Value *UnaryExprAST::codegen() {
	Value *OperandV = Operand->codegen();
	if (!OperandV)
		return nullptr;

	Function *F = Codegen::getFunction(std::make_pair(std::string("unary") + Opcode, 1));
	if (!F)
		return Codegen::LogErrorV("Unknown unary operator");

	KSDbgInfo.emitLocation(this);
	return Codegen::Builder->CreateCall(F, OperandV, "unop");
}

raw_ostream &UnaryExprAST::dump(raw_ostream &out, int ind) {
	ExprAST::dump(out << "unary" << Opcode, ind);
	Operand->dump(out, ind + 1);
	return out;
}
