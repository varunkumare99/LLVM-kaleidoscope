#include "KaleidoscopeJIT.h"
#include <memory>
#include "Codegen.h"
#include "JitOptimizer.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

namespace JITopt {

	std::unique_ptr<KaleidoscopeJIT> TheJIT;
	void InitializeModuleAndPassManager(void) {
		//Open a new context and module
		Codegen::Thecontext = std::make_unique<LLVMContext>();
		Codegen::TheModule = std::make_unique<Module>("my cool jit", *(Codegen::Thecontext));

		//setup data layout for jit
		Codegen::TheModule->setDataLayout(JITopt::TheJIT->getDataLayout());

		//Creat a new builder for the module
		Codegen::Builder = std::make_unique<IRBuilder<>>(*Codegen::Thecontext);

		//Pass manager to attach to the module
		Codegen::TheFPM = std::make_unique<legacy::FunctionPassManager>(Codegen::TheModule.get());

		// peephole and bit twiddling optimizations
		Codegen::TheFPM->add(createInstructionCombiningPass());

		//Reassociate expressions.
		Codegen::TheFPM->add(createReassociatePass());

		//Elimate Common sub expressions
		Codegen::TheFPM->add(createGVNPass());

		// Delete unreachable nodes in the control graph
		Codegen::TheFPM->add(createCFGSimplificationPass());

		Codegen::TheFPM->doInitialization();
	}
}
