#include "Parser.h"
#include "BinaryExprAST.h"
#include "NumberExprAST.h"
#include "VariableExprAST.h"
#include "CallExprAST.h"
#include <memory>
#include "Codegen.h"

std::map<char, int> Parser::BinopPrecendence = { {'<', 10}, {'+', 20}, {'-', 20}, {'*', 40}};

Parser::Parser(Lexer lex, int tok) : CurTok(tok), lexer(lex) {}

int Parser::getNextToken() {
	return Parser::CurTok = lexer.gettok();
}

std::unique_ptr<ExprAST> Parser::LogError(const char *Str) {
	fprintf(stderr, "LogError: %s\n", Str);
	return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::LogErrorP(const char *Str) {
	LogError(Str);
	return nullptr;
}

std::unique_ptr<ExprAST> Parser::ParseNumberExpr() {
	auto Result = std::make_unique<NumberExprAST>(lexer.getNumVal());
	getNextToken(); //consume token
	return std::move(Result);
}

//parenExpr ::= '(' expression ')'
std::unique_ptr<ExprAST> Parser::ParseParenExpr() {
	//AST does not have '(' ')' needed only by parser for grouping of exprs
	getNextToken(); // consume '('
	auto V = ParseExpression();
	if (!V)
		return nullptr;

	if (CurTok != ')')
		return LogError("exprected )");
	getNextToken(); //consume ')'
	return V;
}

//identifierExpr
// ::= identifier
// ::= identifier '(' expression* ')' eg: add(x, y, z)

std::unique_ptr<ExprAST> Parser::ParseIdentifierExpr() {
	std::string IdName = lexer.getIdentifierStr();

	getNextToken(); //eat identifier

	if (CurTok != '(')
		return std::make_unique<VariableExprAST>(IdName);

	//Call
	getNextToken(); //eat '('
	std::vector<std::unique_ptr<ExprAST>> Args;
	if (CurTok != ')') {
		while (1) {
			if (auto Arg = ParseExpression())
				Args.push_back(std::move(Arg));
			else
				return nullptr;

			if (CurTok == ')')
				break;

			if (CurTok != ',')
				return LogError("Expected ')' or ',' in argument list");
			getNextToken();
		}
	}

	//eat token ')'
	getNextToken();

	return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

//primary
//      ::identifierExpr
//      ::numberExpr
//      ::parenExpr

std::unique_ptr<ExprAST> Parser::ParsePrimary() {
	switch (CurTok) {
		case Lexer::tok_identifier:
			return ParseIdentifierExpr();
		case Lexer::tok_number:
			return ParseNumberExpr();
		case '(':
			return ParseParenExpr();
		default:
			return LogError("unknown token when expecting an expression");
	}
}

int Parser::GetTokPrecedence() {
	if (!isascii(CurTok))
		return -1;

	int TokPrec = BinopPrecendence[CurTok];
	if (TokPrec <= 0) 
		return -1;
	return TokPrec;
}

// expression
// 			 := primary Binoprhs
std::unique_ptr<ExprAST> Parser::ParseExpression() {
	auto LHS = ParsePrimary();
	if (!LHS)
		return nullptr;
	return ParseBinOpRHS(0, std::move(LHS));
}

//binOprhs
//		  := (Op primary)*, eg: ('+' primary)
std::unique_ptr<ExprAST> Parser::ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS) {

	while(1) {
		int TokPrec = GetTokPrecedence();

		if (TokPrec < ExprPrec)
			return LHS;

		int BinOp = CurTok;
		getNextToken(); //eat binOp

		auto RHS = ParsePrimary();
		if (!RHS)
			return nullptr;

		int NextPrec = GetTokPrecedence();
		if (TokPrec  < NextPrec) {
			RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
			if (!RHS)
				return nullptr;
		}

		LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
	}
}

//prototype
//			:= id (id*)
std::unique_ptr<PrototypeAST> Parser::ParsePrototype() {
	if (CurTok != Lexer::tok_identifier) {
		return LogErrorP("Expected function name in prototype");
	}

	std::string Fname = lexer.getIdentifierStr();
	getNextToken();

	if (CurTok != '(') {
		return LogErrorP("Expected '(' in function prototype");
	}

	//Read list of arguments. They are separated by spaces
	std::vector<std::string> ArgNames;
	while (getNextToken() == Lexer::tok_identifier)
		ArgNames.push_back(lexer.getIdentifierStr());
	if (CurTok != ')')
		return LogErrorP("Expected ')' in function prototype");

	getNextToken(); //consume ')'
	return std::make_unique<PrototypeAST>(Fname, std::move(ArgNames));
}

///definition := 'def' prototype expression
std::unique_ptr<FunctionAST> Parser::ParseDefinition() {
	getNextToken(); //eat 'def'
	auto Proto = ParsePrototype();
	if (!Proto)
		return nullptr;
	if (auto E = ParseExpression())
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
	return nullptr;
}

//external ::= 'extern' prototype
std::unique_ptr<PrototypeAST> Parser::ParseExtern() {
	getNextToken(); //eat extern
	return ParsePrototype();
}

//toplevelexpr := expression
std::unique_ptr<FunctionAST> Parser::ParseTopLevelExpr() {
	if (auto E = ParseExpression()) {
		//Make a anonymous Proto
		auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
		return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
	}
	return nullptr;
}

void Parser::HandleDefinition() {
	if (auto FnAST = ParseDefinition()) {
		if (auto *FnIR = FnAST->codegen()) {
			fprintf(stderr, "Read function definition");
			FnIR->print(errs());
			fprintf(stderr, "\n");
		}
	}
	else {
		//Skip the token for error recovery
		getNextToken();
	}
}

void Parser::HandleExtern() {
	if (auto ProtoAST = ParseExtern()) {
		if (auto *FnIR = ProtoAST->codegen()) {
			fprintf(stderr, "Read extern: ");
			FnIR->print(errs());
			fprintf(stderr, "\n");
		}
	}
	else {
		//skip the token for error recovery
		getNextToken();
	}
}

void Parser::HandleTopLevelExpression() {
	if (auto FnAST = ParseTopLevelExpr()) {
		if (auto *FnIR = FnAST->codegen()) {
			fprintf(stderr, "Read top-level expression:");
			FnIR->print(errs());
			fprintf(stderr, "\n");
		}
	}
	else {
		//skip the token for error recovery
		getNextToken();
	}
}

void Parser::InitializeModule() {
	//Open a new context and module
	Codegen::Thecontext = std::make_unique<LLVMContext>();
	Codegen::TheModule = std::make_unique<Module>("my cool jit Arrakis", *(Codegen::Thecontext));

	//Create a new builder for the module
	Codegen::Builder = std::make_unique<IRBuilder<>>(*(Codegen::Thecontext));
}

int Parser::getCurTok() {
	return CurTok;
}
