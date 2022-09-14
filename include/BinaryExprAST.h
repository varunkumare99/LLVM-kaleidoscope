#ifndef BINARYEXPRAST_H
#define BINARYEXPRAST_H

#include "ExprAST.h"
#include <iostream>

#include <memory>
class BinaryExprAST : public ExprAST {
    private:
        char Op;
        std::unique_ptr<ExprAST> LHS, RHS; //captures the operation and LHS and RHS of a binary expr
    public:
        BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS);
		Value* codegen() override;
};

#endif
