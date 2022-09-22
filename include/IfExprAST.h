#ifndef IFEXPRAST_H
#define IFEXPRAST_H

#include "ExprAST.h"
#include <iostream>
class IfExprAST: public ExprAST {
	private:
		std::unique_ptr<ExprAST> Cond, Then, Else;
	public:
		IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then, std::unique_ptr<ExprAST> Else);
		Value *codegen() override;
};

#endif
