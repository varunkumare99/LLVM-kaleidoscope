#ifndef VARIABLEEXPRAST_H
#define VARIABLEEXPRAST_H

#include "ExprAST.h"
#include <iostream>
class VariableExprAST: public ExprAST {
    private:
    std::string Name; //capture name in ast

    public:
    VariableExprAST(SourceLocation Loc, const std::string &name);
	Value *codegen() override;
	const std::string& getName();
	raw_ostream &dump(raw_ostream &out, int ind) override;
};

#endif
