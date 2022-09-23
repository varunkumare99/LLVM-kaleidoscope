#ifndef DEBUGDATA_H
#define DEBUGDATA_H

#include "llvm/IR/DIBuilder.h"
#include <memory>

class ExprAST;
using namespace llvm;

extern std::unique_ptr<DIBuilder> DBuilder;

struct DebugInfo {
	DICompileUnit *TheCu;
	DIType *DblTy;
	std::vector<DIScope *> LexicalBlocks;

	void emitLocation(ExprAST *AST);
	DIType *getDoubleTy();
};

extern struct DebugInfo KSDbgInfo;

struct SourceLocation {
	int Line;
	int Col;
};

extern SourceLocation CurLoc;
extern SourceLocation LexLoc;

raw_ostream &indent(raw_ostream &O, int size);
DISubroutineType *CreateFunctionType(unsigned NumArgs, DIFile *Unit);
#endif
