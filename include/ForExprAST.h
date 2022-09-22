#ifndef FOREXPRAST_H
#define FOREXPRAST_H

#include "ExprAST.h"
/// ForExprAST - Expression class for for/in
class ForExprAST: public ExprAST {
	private:
		std::string VarName;
		std::unique_ptr<ExprAST> Start, End, Step, Body;

	public:
		ForExprAST(const std::string &VarName, std::unique_ptr<ExprAST> Start,std::unique_ptr<ExprAST> End,std::unique_ptr<ExprAST> Step,std::unique_ptr<ExprAST> Body); 

		Value *codegen() override;
};

#endif
