#include <iostream>
#include "KaleidoscopeJIT.h"

using namespace llvm::orc;
namespace JITopt {
	extern std::unique_ptr<KaleidoscopeJIT> TheJIT;
	void InitializeModuleAndPassManager(void);
	const std::string ANONYMOUS_EXPR = "__anon_expr";
}
