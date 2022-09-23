#include "FunctionAST.h"
#include "Parser.h"
#include "Codegen.h"
#include "llvm/IR/Verifier.h"
#include "JitOptimizer.h"

FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body)
	:Proto(std::move(Proto)), Body(std::move(Body)) {
	}

Function *FunctionAST::codegen() {
	// Transfer ownership of the prototype to the FunctionProtos map, but keep a
	// reference to it for use below.
	auto &protoref = *Proto;

	if (Proto->getName() != JITopt::ANONYMOUS_EXPR) {
		auto func = find_if(Codegen::FunctionProtos.begin(), Codegen::FunctionProtos.end(), 
				[protoref](auto& itr) {
				return itr.first.first == protoref.getName() && itr.first.second == protoref.getArgs().size();});

		auto funcSameName = find_if(Codegen::FunctionProtos.begin(), Codegen::FunctionProtos.end(), 
				[protoref](auto& itr) {
				return itr.first.first == protoref.getName();});

		std::string origName = Proto->getName();
		if ((func !=funcSameName)&& funcSameName != Codegen::FunctionProtos.end()){
			Proto->setName(Proto->getName() + "." + std::to_string(Codegen::FUNCTION_COUNTER++));	
		}
		Codegen::functionOverloadNameMap[make_pair(origName, Proto->getArgs().size())] = Proto->getName();
	}
	Codegen::FunctionProtos[make_pair(Proto->getName(), Proto->getArgs().size())] = std::move(Proto);
	Function *TheFunction = Codegen::getFunction(make_pair(protoref.getName(), protoref.getArgs().size()));

	if (TheFunction) {
		int i = 0;
		for (auto itr = TheFunction->arg_begin(); itr != TheFunction->arg_end() ; ++itr, ++i) {
			itr->setName(protoref.getArgs()[i]);
		}
	}

	if (!TheFunction)
		return nullptr;

	//If this is an operator, install it.
	if (protoref.isBinaryOp())
		Parser::BinopPrecendence[protoref.getOperatorName()] = protoref.getBinaryPrecedence();

	// Create a new basic block to start insertion into.
	BasicBlock *BB = BasicBlock::Create(*Codegen::Thecontext, "entry", TheFunction);
	Codegen::Builder->SetInsertPoint(BB);

	// Record the function arguments in the NamedValues map.
	Codegen::NamedValues.clear();
	for (auto &Arg : TheFunction->args()) {
		//Create an alloca for this variable.
		AllocaInst *Alloca = Codegen::CreateEntryBlockAlloca(TheFunction, std::string(Arg.getName()));

		//Store the initial value into the alloca
		Codegen::Builder->CreateStore(&Arg, Alloca);

		//Add arguments to variable symbol table
		Codegen::NamedValues[std::string(Arg.getName())] = Alloca;
	}

	if (Value *RetVal = Body->codegen()) {
		// Finish off the function.
		Codegen::Builder->CreateRet(RetVal);

		// Validate the generated code, checking for consistency.
		verifyFunction(*TheFunction);

		// Run the optimizer on the function.
		Codegen::TheFPM->run(*TheFunction);

		return TheFunction;
	}

	// Error reading body, remove function.
	TheFunction->eraseFromParent();
	return nullptr;
}
