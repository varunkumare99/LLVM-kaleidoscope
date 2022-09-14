//ExprAST - Base class for all expression nodes
#ifndef EXPRAST_H
#define EXPRAST_H
#include "llvm/IR/Value.h"
using namespace llvm;
class ExprAST{
    public:
        virtual ~ExprAST() {}
		virtual Value *codegen() = 0;
};

#endif
