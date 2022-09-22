#include "IfExprAST.h"
#include <Codegen.h>

IfExprAST::IfExprAST(std::unique_ptr<ExprAST> Cond, std::unique_ptr<ExprAST> Then, std::unique_ptr<ExprAST> Else)
	:Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

Value *IfExprAST::codegen() {
	Value *CondV = Cond->codegen();
	if (!CondV)
		return nullptr;

	//convert condition to a bool value by comparing non-equal to 0.0
	CondV = Codegen::Builder->CreateFCmpONE(CondV, ConstantFP::get(*Codegen::Thecontext, APFloat(0.0)), "ifcond");

	Function *TheFunction = Codegen::Builder->GetInsertBlock()->getParent();

	//Create blocks for the 'then' and 'else' cases. Insert the 'then' block at the
	//end of the function
	//ThenBB is insert at the end of the current function block TheFunction
	BasicBlock *ThenBB = BasicBlock::Create(*Codegen::Thecontext, "then", TheFunction);
	//ElseBB, MergeBB are not added to TheFunction yet
	BasicBlock *ElseBB = BasicBlock::Create(*Codegen::Thecontext, "else");
	BasicBlock *MergeBB = BasicBlock::Create(*Codegen::Thecontext, "ifcont");

	//create a bracch from the block that has CondV to ThenBB and ElseBB
	Codegen::Builder->CreateCondBr(CondV, ThenBB, ElseBB);

	//emit Then block
	//current we are in ThenBB since it is empty, it will insert at the beginning of the Then Block
	Codegen::Builder->SetInsertPoint(ThenBB);

	Value *ThenV = Then->codegen();
	if (!ThenV)
		return nullptr;

	//branch to MergeBB after completion of Thenblock
	Codegen::Builder->CreateBr(MergeBB);
	//codegen of 'Then' can change the current block, update ThenBB for the PHI node.
	ThenBB = Codegen::Builder->GetInsertBlock();

	//Emit else Block
	//ElseBB is added to added to TheFunction at the end, as it was not added during it's creation unlike ThenBB
	TheFunction->getBasicBlockList().push_back(ElseBB);
	Codegen::Builder->SetInsertPoint(ElseBB);

	Value *ElseV = Else->codegen();
	if (!ElseV)
		return nullptr;

	//branch to MergeBB after completion of Elseblock
	Codegen::Builder->CreateBr(MergeBB);
	//codegen of 'Else' can change the current block, update ElseBB for the PHI.
	ElseBB = Codegen::Builder->GetInsertBlock();

	//emit Merge block
	TheFunction->getBasicBlockList().push_back(MergeBB);
	//so that code is added to merge block
	Codegen::Builder->SetInsertPoint(MergeBB);
	PHINode *PN = Codegen::Builder->CreatePHI(Type::getDoubleTy(*Codegen::Thecontext), 2, "iftmp");

	PN->addIncoming(ThenV, ThenBB);
	PN->addIncoming(ElseV, ElseBB);
	return PN;
}

