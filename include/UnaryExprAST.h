#ifndef UNARYEXPRAST_H
#define UNARYEXPRAST_H

#include "ExprAST.h"
/// UnaryExprAST - Expression class for a unary operator
class UnaryExprAST: public ExprAST {
	private:
	char Opcode;
	std::unique_ptr<ExprAST> Operand;

	public:
	UnaryExprAST(char Opcode, std::unique_ptr<ExprAST> Operand);
	Value *codegen() override;
};

#endif
