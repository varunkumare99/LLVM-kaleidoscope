#ifndef CODEGEN_H
#define CODEGEN_H

#include <memory>
#include "PrototypeAST.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"

using namespace llvm;

namespace Codegen {
		extern std::unique_ptr<LLVMContext> Thecontext;
		extern std::unique_ptr<IRBuilder<>> Builder;
		extern std::unique_ptr<Module> TheModule;
		extern std::map<std::string, Value*> NamedValues;
		extern std::unique_ptr<legacy::FunctionPassManager> TheFPM;
		extern std::map<std::pair<std::string, unsigned int>, std::unique_ptr<PrototypeAST>> FunctionProtos;
		extern std::map<std::pair<std::string, unsigned int>, std::string> functionOverloadNameMap;
		extern ExitOnError ExitOnErr;
		extern unsigned int FUNCTION_COUNTER;

		extern Value *LogErrorV(const char *Str);
		extern Function *getFunction(std::string Name);
	    extern Function *getFunction(const std::pair<std::string, unsigned int>& functionName_args);
}
#endif
