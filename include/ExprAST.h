//ExprAST - Base class for all expression nodes
#ifndef EXPRAST_H
#define EXPRAST_H

#include "llvm/IR/Value.h"
#include "DebugData.h"
using namespace llvm;

class ExprAST{
	private:
		SourceLocation Loc;
    public:
		ExprAST(SourceLocation Loc = CurLoc);
		int getLine() const;
		int getCol() const;
		virtual raw_ostream &dump(raw_ostream &out, int ind);
        virtual ~ExprAST() {}
		virtual Value *codegen() = 0;
};

#endif
