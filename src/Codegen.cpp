#include "Codegen.h"
#include <algorithm>
#include <memory>

namespace Codegen {
	Value* LogErrorV(const char *Str) {
		fprintf(stderr, "Error: %s\n", Str);
		return nullptr;
	}

	std::unique_ptr<LLVMContext> Thecontext;
	std::unique_ptr<IRBuilder<>> Builder;
	std::unique_ptr<Module> TheModule;
	std::map<std::string, Value*> NamedValues;
}
