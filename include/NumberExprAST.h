#ifndef NUMBEREXPRAST_H
#define NUMBEREXPRAST_H

#include "ExprAST.h"
class NumberExprAST: public ExprAST{
	private:
		double Val; //capture numeric value in AST

	public:
		NumberExprAST(double Val);
		Value *codegen() override;
};

#endif
