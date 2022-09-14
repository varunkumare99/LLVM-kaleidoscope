#ifndef VARIABLEEXPRAST_H
#define VARIABLEEXPRAST_H

#include "ExprAST.h"
#include <iostream>
class VariableExprAST: public ExprAST {
    private:
    std::string Name; //capture name in ast

    public:
    VariableExprAST(const std::string &name);
	Value *codegen() override;
};

#endif
