#include "Codegen.h"
#include <algorithm>
#include <memory>

namespace Codegen {
	Value* LogErrorV(const char *Str) {
		fprintf(stderr, "Error: %s\n", Str);
		return nullptr;
	}

	Function *getFunction(const std::pair<std::string, unsigned int>& functionName_args) {
		bool found = false;
		auto functionItr = Codegen::TheModule->getFunctionList().begin();
		for (; functionItr != Codegen::TheModule->getFunctionList().end(); ++functionItr) {
			auto functionName = functionItr->getName().str();
			auto position = functionName.find('.');
			if (position != std::string::npos) {
				functionName = functionName.substr(0, position);
			}
			if ((functionName == functionName_args.first) && (functionItr->arg_size() == functionName_args.second)) {
				found = true;
				break;
			}
		}

		if (found)
			return &(*functionItr);

		auto func = find_if(FunctionProtos.begin(), FunctionProtos.end(), 
				[functionName_args](auto& itr) {
				return itr.first.first == functionName_args.first && itr.first.second == functionName_args.second;});
		if (func != FunctionProtos.end()) {
			return func->second->codegen();
		}
		// return null if prototype does not exist
		return nullptr;
	}

	std::unique_ptr<LLVMContext> Thecontext;
	std::unique_ptr<IRBuilder<>> Builder;
	std::unique_ptr<Module> TheModule;
	std::map<std::string, Value*> NamedValues;
	std::unique_ptr<legacy::FunctionPassManager> TheFPM;
	std::map<std::pair<std::string, unsigned int>, std::unique_ptr<PrototypeAST>> FunctionProtos;
	std::map<std::pair<std::string,unsigned int>, std::string> functionOverloadNameMap;
	ExitOnError ExitOnErr;
	unsigned int FUNCTION_COUNTER;
}
