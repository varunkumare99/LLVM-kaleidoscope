#include "CallExprAST.h"
#include <memory>
#include "Codegen.h"
#include "llvm/IR/Module.h"

CallExprAST::CallExprAST(const std::string& Callee, std::vector<std::unique_ptr<ExprAST>> Args)
    :Callee(Callee), Args(std::move(Args)) {}

Value *CallExprAST::codegen() {
	bool found = false;

	auto itr = Codegen::TheModule->getFunctionList().begin();
	for (; itr != Codegen::TheModule->getFunctionList().end(); ++itr) {
		auto fName = itr->getName().str();
		auto position = fName.find('.');
		if (position != std::string::npos) {
			fName = fName.substr(0, position);

		}
		if ((fName == Callee) && (itr->arg_size() == Args.size())) {
			found = true;
			break;
		}
	}

	if (!found)
		return Codegen::LogErrorV("Unknown function referenced");

	std::vector<Value *> ArgsV;
	for (unsigned i = 0, e = Args.size(); i != e; ++i) {
		ArgsV.push_back(Args[i]->codegen());
		if (!ArgsV.back())
			return nullptr;
	}

	return Codegen::Builder->CreateCall(&(*itr), ArgsV, "calltmp");
}
