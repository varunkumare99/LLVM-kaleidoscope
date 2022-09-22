#include "ForExprAST.h"
#include "Codegen.h"

ForExprAST::ForExprAST(const std::string &VarName, std::unique_ptr<ExprAST> Start,std::unique_ptr<ExprAST> End,std::unique_ptr<ExprAST> Step,std::unique_ptr<ExprAST> Body)
	:VarName(VarName), Start(std::move(Start)), End(std::move(End)), Step(std::move(Step)), Body(std::move(Body)) {}

Value *ForExprAST::codegen() {
	//Emit the Start code first, without 'variable' in scope.
	Value *StartVal = Start->codegen();
	if (!StartVal)
		return nullptr;

	//Make the new basic block for the loop header, inserting after current block
	Function *TheFunction = Codegen::Builder->GetInsertBlock()->getParent();
	//need to add to PHI node
	BasicBlock *PreHeaderBB = Codegen::Builder->GetInsertBlock();
	BasicBlock *LoopBB = BasicBlock::Create(*Codegen::Thecontext, "loop", TheFunction);

	//Insert an explicit fall through from the current block to the LoopBB
	Codegen::Builder->CreateBr(LoopBB);

	//Start insertion in LoopBB
	Codegen::Builder->SetInsertPoint(LoopBB);

	//Start the PHI node with an entry for Start.
	PHINode *Variable = Codegen::Builder->CreatePHI(Type::getDoubleTy(*Codegen::Thecontext), 2, VarName.c_str());

	Variable->addIncoming(StartVal, PreHeaderBB);

	//Within the loop, the variable is defined equal to the PHI node. If it
	//shadows an existing variable, we have to restore it, so save it now.
	//save the variable incase a variable is already present in scope
	Value *OldVal= Codegen::NamedValues[VarName];
	Codegen::NamedValues[VarName] = Variable;

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

	//value of the loop on the next iteration
	Value *NextVar = Codegen::Builder->CreateFAdd(Variable, StepVal, "nextvar");

	//Compute the end condition
	Value *EndCond = End->codegen();
	if (!EndCond)
		return nullptr;

	//Convert condition to a bool by comparing non-equal to 0.0
	EndCond = Codegen::Builder->CreateFCmpONE(EndCond, ConstantFP::get(*Codegen::Thecontext, APFloat(0.0)), "loopcond");

	//Create the after loop block and insert it
	//remeber end of phi node block
	BasicBlock *LoopEndBB = Codegen::Builder->GetInsertBlock();
	BasicBlock *AfterBB = BasicBlock::Create(*Codegen::Thecontext, "afterloop", TheFunction);

	//Insert the conditional branch into the end of LoopEndBB
	//true then loopBB else AfterBB
	Codegen::Builder->CreateCondBr(EndCond, LoopBB, AfterBB);

	//Now any new code will be inserted AfterBB
	Codegen::Builder->SetInsertPoint(AfterBB);

	//Add a new entry to the PHI node for the backedge
	Variable->addIncoming(NextVar, LoopEndBB);

	//restore the old shadowed value
	if (OldVal)
		Codegen::NamedValues[VarName] = OldVal;
	else
		Codegen::NamedValues.erase(VarName);

	// for expr always returns 0.0
	return Constant::getNullValue(Type::getDoubleTy(*Codegen::Thecontext));
}
