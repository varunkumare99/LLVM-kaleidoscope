#ifndef PARSER_H
#define PARSER_H

#include "PrototypeAST.h"
#include "FunctionAST.h"
#include "Lexer.h"
#include <memory>
#include <map>

class Parser {
    public:
	Parser(Lexer lex, int tok = 0);
    std::unique_ptr<ExprAST> LogError(const char *str);
    std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);
    int getNextToken();

    std::unique_ptr<ExprAST> ParseNumberExpr();
    std::unique_ptr<ExprAST> ParseParenExpr();
    std::unique_ptr<ExprAST> ParseIdentifierExpr();
    std::unique_ptr<ExprAST> ParsePrimary();
	std::unique_ptr<PrototypeAST> ParsePrototype();
	std::unique_ptr<FunctionAST> ParseDefinition();
	std::unique_ptr<FunctionAST> ParseTopLevelExpr();
	std::unique_ptr<PrototypeAST> ParseExtern();
	int GetTokPrecedence();


	std::unique_ptr<ExprAST> ParseExpression();
	std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);
	
	void HandleDefinition();
	void HandleExtern();
	void HandleTopLevelExpression();
	int getCurTok();

    private:
    int CurTok;
    Lexer lexer;
    static std::map<char, int> BinopPrecendence;
};

#endif
