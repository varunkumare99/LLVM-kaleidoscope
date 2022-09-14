#ifndef CALLEXPRAST_H
#define CALLEXPRAST_H

#include "ExprAST.h"
#include <iostream>
#include <memory>
#include <vector>

class CallExprAST : public ExprAST {
    private:
        std::string Callee;
        std::vector<std::unique_ptr<ExprAST>> Args; //capture the callee function name and arguments

    public:
        CallExprAST(const std::string& Callee, std::vector<std::unique_ptr<ExprAST>> Args);
		Value* codegen() override;
};

#endif
