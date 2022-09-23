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
        CallExprAST(SourceLocation Loc, const std::string& Callee, std::vector<std::unique_ptr<ExprAST>> Args);
		Value* codegen() override;
		raw_ostream &dump(raw_ostream &out, int ind) override;
};

#endif
