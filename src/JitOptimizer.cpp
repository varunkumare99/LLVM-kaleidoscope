#include "KaleidoscopeJIT.h"
#include "Codegen.h"
#include "JitOptimizer.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"

namespace JITopt {

	std::unique_ptr<KaleidoscopeJIT> TheJIT;
	void InitializeModule(void) {
		//Open a new context and module
		Codegen::Thecontext = std::make_unique<LLVMContext>();
		Codegen::TheModule = std::make_unique<Module>("my cool jit", *(Codegen::Thecontext));

		//setup data layout for jit
		Codegen::TheModule->setDataLayout(JITopt::TheJIT->getDataLayout());

		//Creat a new builder for the module
		Codegen::Builder = std::make_unique<IRBuilder<>>(*Codegen::Thecontext);
	}
}
