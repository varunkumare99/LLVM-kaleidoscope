#ifndef FUNCTIONAST_H
#define FUNCTIONAST_H

#include "PrototypeAST.h"
#include "ExprAST.h"

class FunctionAST {
    private:
        std::unique_ptr<PrototypeAST> Proto;
        std::unique_ptr<ExprAST> Body;

    public:
        FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body);
		Function *codegen();
		raw_ostream &dump(raw_ostream &out, int ind);
};

#endif
