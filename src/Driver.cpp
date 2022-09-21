#include "Parser.h"
#include "JitOptimizer.h"
#include "Codegen.h"
#include <memory>
#include "KaleidoscopeJIT.h"
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
	LLVMInitializeNativeTarget();
	LLVMInitializeNativeAsmPrinter();
	LLVMInitializeNativeAsmParser();
	//get first token
	fprintf(stderr, "ready>>> ");
	Lexer lex;
	Parser parser(lex);
	parser.getNextToken();	
	//Make the module which holds the code
	JITopt::TheJIT = Codegen::ExitOnErr(KaleidoscopeJIT::Create());
	JITopt::InitializeModuleAndPassManager();
	MainLoop(parser);

	//dump all the llvm IR on exit
	Codegen::TheModule->print(errs(), nullptr);
	return 0;
}
