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

	// Create a subprogram DIE for this function
	DIFile *Unit = DBuilder->createFile(KSDbgInfo.TheCu->getFilename(), KSDbgInfo.TheCu->getDirectory());
	DIScope *FContext = Unit;
	unsigned LineNo = protoref.getLine();
	unsigned ScopeLine = LineNo;
	DISubprogram *SP = DBuilder->createFunction(
			FContext, protoref.getName(), StringRef(), Unit, LineNo, 
			CreateFunctionType(TheFunction->arg_size(), Unit), ScopeLine,
				DINode::FlagPrototyped, DISubprogram::SPFlagDefinition);
	TheFunction->setSubprogram(SP);

	//push the current scope
	KSDbgInfo.LexicalBlocks.push_back(SP);

	/* unset the location for the prologue emission (leading instructions with no location
	 * in a function are considered part of the prologue and the debugger will run past
	 * them when breaking on a function
	 */
	KSDbgInfo.emitLocation(nullptr);
	// Record the function arguments in the NamedValues map.
	Codegen::NamedValues.clear();
	unsigned ArgIdx = 0;
	for (auto &Arg : TheFunction->args()) {
		//Create an alloca for this variable.
		AllocaInst *Alloca = Codegen::CreateEntryBlockAlloca(TheFunction, std::string(Arg.getName()));

		//Create a debug descriptor for the variable
		DILocalVariable *D = DBuilder->createParameterVariable(SP, Arg.getName(), ++ArgIdx, Unit, LineNo, KSDbgInfo.getDoubleTy()
				,true);

		DBuilder->insertDeclare(Alloca, D, DBuilder->createExpression(), DILocation::get(SP->getContext(), LineNo, 0, SP),
				Codegen::Builder->GetInsertBlock());
		//Store the initial value into the alloca
		Codegen::Builder->CreateStore(&Arg, Alloca);

		//Add arguments to variable symbol table
		Codegen::NamedValues[std::string(Arg.getName())] = Alloca;
	}

	KSDbgInfo.emitLocation(Body.get());

	if (Value *RetVal = Body->codegen()) {
		// Finish off the function.
		Codegen::Builder->CreateRet(RetVal);

		//Pop off the lexical block for the function
		KSDbgInfo.LexicalBlocks.pop_back();

		// Validate the generated code, checking for consistency.
		verifyFunction(*TheFunction);

		return TheFunction;
	}

	// Error reading body, remove function.
	TheFunction->eraseFromParent();

	if (protoref.isBinaryOp())
		Parser::BinopPrecendence.erase(protoref.getOperatorName());

	// Pop off the lexical block for the function since we added it
	// unconditionally.
	KSDbgInfo.LexicalBlocks.pop_back();

	return nullptr;
}

raw_ostream &FunctionAST::dump(raw_ostream &out, int ind) {
	indent(out, ind) << "FunctionAST\n";
	++ind;
	indent(out, ind) << "Body:";
	return Body ? Body->dump(out, ind) : out << "null\n";
}
