#include "Parser.h"
#include "JitOptimizer.h"
#include "Codegen.h"
#include <memory>
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
		fprintf(stderr, "ready>>> ");
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
	//get first token
	fprintf(stderr, "ready>>> ");
	Lexer lex;
	Parser parser(lex);
	parser.getNextToken();	
	//Make the module which holds the code
	/* JITopt::TheJIT = Codegen::ExitOnErr(KaleidoscopeJIT::Create()); */
	JITopt::InitializeModuleAndPassManager();
	MainLoop(parser);

	// Initialize the target registry etc.
	InitializeAllTargetInfos();
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmParsers();
	InitializeAllAsmPrinters();

	auto TargetTriple = sys::getDefaultTargetTriple();
	Codegen::TheModule->setTargetTriple(TargetTriple);

	std::string Error;
	auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

	// Print an error and exit if we couldn't find the requested target.
	// This generally occurs if we've forgotten to initialise the
	// TargetRegistry or we have a bogus target triple.
	if (!Target) {
		errs() << Error;
		return 1;
	}

	auto CPU = "generic";
	auto Features = "";

	TargetOptions opt;
	auto RM = Optional<Reloc::Model>();
	auto TheTargetMachine =
		Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

	Codegen::TheModule->setDataLayout(TheTargetMachine->createDataLayout());

	auto Filename = "output.o";
	std::error_code EC;
	raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

	if (EC) {
		errs() << "Could not open file: " << EC.message();
		return 1;
	}

	legacy::PassManager pass;
	auto FileType = CGFT_ObjectFile;

	if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
		errs() << "TheTargetMachine can't emit a file of this type";
		return 1;
	}

	pass.run(*Codegen::TheModule);
	dest.flush();

	outs() << "Wrote " << Filename << "\n";

	//dump all the llvm IR on exit
	Codegen::TheModule->print(errs(), nullptr);
	return 0;
}
