#include "CallExprAST.h"
#include <memory>
#include "Codegen.h"
#include "llvm/IR/Module.h"
#include "JitOptimizer.h"

CallExprAST::CallExprAST(const std::string& Callee, std::vector<std::unique_ptr<ExprAST>> Args)
	:Callee(Callee), Args(std::move(Args)) {}

	Value *CallExprAST::codegen() {
		// Look up the name in the global module table.
		Function *CalleeF;
		if (Callee != JITopt::ANONYMOUS_EXPR) {
			if(Codegen::functionOverloadNameMap.find(make_pair(Callee, Args.size())) != Codegen::functionOverloadNameMap.end()) {
				std::string origName = Codegen::functionOverloadNameMap[make_pair(Callee, Args.size())];
				CalleeF = Codegen::getFunction(make_pair(origName, Args.size()));
			}
			else {
				CalleeF = Codegen::getFunction(make_pair(Callee, Args.size()));
			}
		}
		if (!CalleeF)
			return Codegen::LogErrorV("Unknown function referenced");

		// If argument mismatch error.
		if (CalleeF->arg_size() != Args.size())
			return Codegen::LogErrorV("Incorrect # arguments passed");

		std::vector<Value *> ArgsV;
		for (unsigned i = 0, e = Args.size(); i != e; ++i) {
			ArgsV.push_back(Args[i]->codegen());
			if (!ArgsV.back())
				return nullptr;
		}

		return Codegen::Builder->CreateCall(CalleeF, ArgsV, "calltmp");
	}
