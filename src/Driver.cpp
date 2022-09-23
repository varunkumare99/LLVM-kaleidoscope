#include "Parser.h"
#include "JitOptimizer.h"
#include "Codegen.h"
#include "DebugData.h"
#include "KaleidoscopeJIT.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/// putchard - putchar that takes a double and returns 0.
extern "C" DLLEXPORT double putchard(double X) {
	fputc((char)X, stderr);
	return 0;
}

/// printd - printf that takes a double prints it as "%f\n", returning 0.
extern "C" DLLEXPORT double printd(double X) {
	fprintf(stderr, "%f\n", X);
	return 0;
}
//top := definition | external | expression | ';'
void MainLoop(Parser& parser) {
	while (true) {
		/* fprintf(stderr, "ready>>> "); */
		switch (parser.getCurTok()) {
			case Lexer::tok_eof:
				return;
			case ';':
				parser.getNextToken();
				break;
			case Lexer::tok_def:
				parser.HandleDefinition();
				break;
			case Lexer::tok_extern:
				parser.HandleExtern();
				break;
			default:
				parser.HandleTopLevelExpression();
				break;
		}
	}
}

int main() {
	InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetAsmParser();

	Lexer lex;
	Parser parser(lex);
	//prime the next token
	parser.getNextToken();	

	JITopt::TheJIT = Codegen::ExitOnErr(KaleidoscopeJIT::Create());

	JITopt::InitializeModule();

	//Add the current debug info version into the module.
	Codegen::TheModule->addModuleFlag(Module::Warning, "Debug Info Version", DEBUG_METADATA_VERSION);

	//Darwin only supports dwarf2
	if (Triple(sys::getProcessTriple()).isOSDarwin()) {
		Codegen::TheModule->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 2);
	}

	//Construct the DIBuilder, we do this here because we need the module
	DBuilder = std::make_unique<DIBuilder>(*Codegen::TheModule);

	// Create the compile unit for the module.
	// Currently down as "fib.ks" as a filename since we're redirecting stdin
	// but we'd like actual source locations
	KSDbgInfo.TheCu = DBuilder->createCompileUnit(
			dwarf::DW_LANG_C, DBuilder->createFile("fib.ks", "."),
			"Kaleidoscope Compiler", 0, "", 0);

	//Run the main interpreter loop now
	MainLoop(parser);

	//Finalize the debug info
	DBuilder->finalize();

	//Print out all the generated code
	Codegen::TheModule->print(errs(), nullptr);
	return 0;
}
