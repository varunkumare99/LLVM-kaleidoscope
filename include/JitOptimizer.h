#ifndef JITOPTIMIZER_H
#define JITOPTIMIZER_H

#include <iostream>
#include "KaleidoscopeJIT.h"

using namespace llvm::orc;
namespace JITopt {
	extern std::unique_ptr<KaleidoscopeJIT> TheJIT;
	void InitializeModule(void);
	const std::string ANONYMOUS_EXPR = "__anon_expr";
}

#endif
