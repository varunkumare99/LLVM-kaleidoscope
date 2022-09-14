#include "CallExprAST.h"
#include <memory>
#include "Codegen.h"

CallExprAST::CallExprAST(const std::string& Callee, std::vector<std::unique_ptr<ExprAST>> Args)
    :Callee(Callee), Args(std::move(Args)) {}

Value *CallExprAST::codegen() {
	// Look up the name in the global module table
	Function *CalleeF = Codegen::TheModule->getFunction(Callee);
	if (!CalleeF)
		return Codegen::LogErrorV("Unknown function referenced");

	// If number of arguments do not match
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
