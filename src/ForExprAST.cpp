#include "ForExprAST.h"
#include "Codegen.h"

ForExprAST::ForExprAST(const std::string &VarName, std::unique_ptr<ExprAST> Start,std::unique_ptr<ExprAST> End,std::unique_ptr<ExprAST> Step,std::unique_ptr<ExprAST> Body)
	:VarName(VarName), Start(std::move(Start)), End(std::move(End)), Step(std::move(Step)), Body(std::move(Body)) {}

Value *ForExprAST::codegen() {
	Function *TheFunction = Codegen::Builder->GetInsertBlock()->getParent();

	//Create an alloca for the variable in the entry block
	AllocaInst *Alloca = Codegen::CreateEntryBlockAlloca(TheFunction, VarName);

	//Emit the Start code first, without 'variable' in scope.
	Value *StartVal = Start->codegen();
	if (!StartVal)
		return nullptr;

	// Store the value into the alloca
	Codegen::Builder->CreateStore(StartVal, Alloca);

	//make the new basic block for the loop header, inserting after current block
	BasicBlock *LoopBB = BasicBlock::Create(*Codegen::Thecontext, "loop", TheFunction);

	//Insert an explicit fall through from the current block to the LoopBB
	Codegen::Builder->CreateBr(LoopBB);

	//Start insertion in LoopBB
	Codegen::Builder->SetInsertPoint(LoopBB);

	//within the loop, the variable is defined equal to the PHI node. If it
	//shadows an existing variable, we have to restore it, so save it now.
	AllocaInst *OldVal = Codegen::NamedValues[VarName];
	Codegen::NamedValues[VarName] = Alloca;

	//emit the body of the loop. This, like any other expr, can change the
	//current BB. Note that we ignore the value computed by the body,
	//But don't allow an error
	if (!Body->codegen())
		return nullptr;

	//emit the step value
	Value *StepVal = nullptr;
	if (Step) {
		StepVal = Step->codegen();
		if (!StepVal)
			return nullptr;
	}
	else {
		//if not specified use, 1.0
		StepVal = ConstantFP::get(*Codegen::Thecontext, APFloat(1.0));
	}

	//Compute the end condition
	Value *EndCond = End->codegen();
	if (!EndCond)
		return nullptr;

	// Reload, increment, and restore the alloca. This handles the case where
	// the body of the loop mutates the variable
	Value *CurVar = Codegen::Builder->CreateLoad(Alloca->getAllocatedType(), Alloca, VarName.c_str());
	Value *NextVar = Codegen::Builder->CreateFAdd(CurVar, StepVal, "nextvar");
	Codegen::Builder->CreateStore(NextVar, Alloca);

	//Convert condition to a bool by comparing non-equal to 0.0
	EndCond = Codegen::Builder->CreateFCmpONE(EndCond, ConstantFP::get(*Codegen::Thecontext, APFloat(0.0)), "loopcond");

	//Create the after loop block and insert it
	BasicBlock *AfterBB = BasicBlock::Create(*Codegen::Thecontext, "afterloop", TheFunction);

	//Insert the conditional branch into the end of LoopEndBB
	//true then loopBB else AfterBB
	Codegen::Builder->CreateCondBr(EndCond, LoopBB, AfterBB);

	//Now any new code will be inserted AfterBB
	Codegen::Builder->SetInsertPoint(AfterBB);

	//restore the old shadowed value
	if (OldVal)
		Codegen::NamedValues[VarName] = OldVal;
	else
		Codegen::NamedValues.erase(VarName);

	// for expr always returns 0.0
	return Constant::getNullValue(Type::getDoubleTy(*Codegen::Thecontext));
}
