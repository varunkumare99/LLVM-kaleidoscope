#include "FunctionAST.h"
#include "Codegen.h"
#include "llvm/IR/Verifier.h"

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body)
    :Proto(std::move(Proto)), Body(std::move(Body)) {}

Function *FunctionAST::codegen() {
	//First, check for an existing function with same name from previous extern declarations
	Function *TheFunction = Codegen::TheModule->getFunction(Proto->getName());

	//if not previous defined, first generate LLVM IR for prototype then proceed towards the body of the function
	if (!TheFunction)
		TheFunction = Proto->codegen();

	if (!TheFunction)
		return nullptr;

	//if the function body is already generated, then exit since body should of function is begined redefined 
	if (!TheFunction->empty())
		return (Function*)Codegen::LogErrorV("function cannot be redefined");

	//Create a basic block who name is entry
	BasicBlock *BB = BasicBlock::Create(*Codegen::Thecontext, "entry", TheFunction);
	//Our whole function body represents one block as it does not have any control flow as of now, this tells LLVM to insert
	//instructions after this block
	Codegen::Builder->SetInsertPoint(BB);

	//we add the function arguments of the NamedValues map so that they are available in current scope
	Codegen::NamedValues.clear();
	for (auto &Arg : TheFunction->args())
		Codegen::NamedValues[std::string(Arg.getName())] = &Arg;

	//generate body of the function
	if (Value *RetVal = Body->codegen()) {
		//add LLVM ret statment at the end of function
		Codegen::Builder->CreateRet(RetVal);

		//used to validate the Thefunction generated, does a lot of checks
		verifyFunction(*TheFunction);
		return TheFunction;
	}

	//error in generating body of the function, thus remove the function
	//it removes it from the symbol table, thus if a user incorrectly types a function, he can further redefine it with its
	//correct definition
	TheFunction->eraseFromParent();
	return nullptr;
}
