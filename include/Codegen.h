#ifndef CODEGEN_H
#define CODEGEN_H

#include <memory>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

using namespace llvm;

namespace Codegen {
		extern std::unique_ptr<LLVMContext> Thecontext;
		extern std::unique_ptr<IRBuilder<>> Builder;
		extern std::unique_ptr<Module> TheModule;
		extern std::map<std::string, Value*> NamedValues;
		extern Value *LogErrorV(const char *Str);
}
#endif
