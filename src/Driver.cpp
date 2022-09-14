#include "Parser.h"
#include <Codegen.h>
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
	parser.InitializeModule();
	MainLoop(parser);

	//dump all the llvm IR on exit
	Codegen::TheModule->print(errs(), nullptr);
	return 0;
}
