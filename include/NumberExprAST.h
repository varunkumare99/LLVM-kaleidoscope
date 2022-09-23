#ifndef NUMBEREXPRAST_H
#define NUMBEREXPRAST_H

#include "ExprAST.h"
class NumberExprAST: public ExprAST{
	private:
		double Val; //capture numeric value in AST

	public:
		NumberExprAST(double Val);
		raw_ostream &dump(raw_ostream &out, int ind) override;
		Value *codegen() override;
};

#endif
