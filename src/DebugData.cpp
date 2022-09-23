#include "DebugData.h"
#include "Codegen.h"
#include "ExprAST.h"

std::unique_ptr<DIBuilder> DBuilder;
struct DebugInfo KSDbgInfo;
struct SourceLocation CurLoc;
struct SourceLocation LexLoc = {1, 0};

DIType *DebugInfo::getDoubleTy() {
	if (DblTy)
		return DblTy;

	DblTy = DBuilder->createBasicType("double", 64, dwarf::DW_ATE_float);
	return DblTy;
}

void DebugInfo::emitLocation(ExprAST *AST) {
	if (!AST) 
		return Codegen::Builder->SetCurrentDebugLocation(DebugLoc());
	DIScope *Scope;
	if (LexicalBlocks.empty())
		Scope = TheCu;
	else
		Scope = LexicalBlocks.back();
	Codegen::Builder->SetCurrentDebugLocation(DILocation::get(
				Scope->getContext(), AST->getLine(), AST->getCol(), Scope));
}

raw_ostream &indent(raw_ostream &O, int size) {
	return O << std::string(size, ' ');
}

DISubroutineType *CreateFunctionType(unsigned NumArgs, DIFile *Unit) {
	SmallVector<Metadata *, 8> EltTys;
	DIType *DblTy = KSDbgInfo.getDoubleTy();

	//Add the result type
	EltTys.push_back(DblTy);

	for (unsigned i = 0, e = NumArgs; i != e; ++i)
		EltTys.push_back(DblTy);

	return DBuilder->createSubroutineType(DBuilder->getOrCreateTypeArray(EltTys));
}
