#include "CallExprAST.h"
#include <memory>
#include "Codegen.h"
#include "llvm/IR/Module.h"
#include "JitOptimizer.h"

CallExprAST::CallExprAST(SourceLocation Loc, const std::string& Callee, std::vector<std::unique_ptr<ExprAST>> Args)
	:ExprAST(Loc), Callee(Callee), Args(std::move(Args)) {}

	Value *CallExprAST::codegen() {
		KSDbgInfo.emitLocation(this);
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

raw_ostream &CallExprAST::dump(raw_ostream &out, int ind) {
	ExprAST::dump(out << "call " << Callee, ind);
	for (const auto &Arg: Args)
		Arg->dump(indent(out, ind + 1), ind + 1);
	return out;
}
