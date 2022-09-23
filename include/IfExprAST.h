#ifndef IFEXPRAST_H
#define IFEXPRAST_H

#include "ExprAST.h"
#include <iostream>
class IfExprAST: public ExprAST {
	private:
		std::unique_ptr<ExprAST> Cond, Then, Else;
	public:
		IfExprAST(SourceLocation Loc, std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then, std::unique_ptr<ExprAST> Else);
		Value *codegen() override;
		raw_ostream &dump(raw_ostream &out, int ind) override;
};

#endif
