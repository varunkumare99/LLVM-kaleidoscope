#include "Lexer.h"

Lexer::Lexer(std::string str, double val): IdentifierStr(str), NumVal(val) {}

int Lexer::gettok() {
    static int LastChar = ' ';

    //skip any white space
    while (isspace(LastChar))
        LastChar = getchar();

    if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
        IdentifierStr = LastChar;
        while (isalnum((LastChar = getchar())))
            IdentifierStr += LastChar;

        if (IdentifierStr == "def")
            return Lexer::tok_def;
        if (IdentifierStr == "extern")
            return Lexer::tok_extern;
        if (IdentifierStr == "if")
            return Lexer::tok_if;
        if (IdentifierStr == "then")
            return Lexer::tok_then;
        if (IdentifierStr == "else")
            return Lexer::tok_else;
        if (IdentifierStr == "for")
            return Lexer::tok_for;
        if (IdentifierStr == "in")
            return Lexer::tok_in;
        if (IdentifierStr == "binary")
            return Lexer::tok_binary;
        if (IdentifierStr == "unary")
            return Lexer::tok_unary;
        else
            return Lexer::tok_identifier;
    }

    if (isdigit(LastChar) || LastChar == '.') {
        std::string Numstr;

        do {
            Numstr += LastChar;
            LastChar = getchar();
        }while (isdigit(LastChar) || LastChar == '.');
        NumVal = strtod(Numstr.c_str(), 0);
		return Lexer::tok_number;
    }

    if (LastChar == '#') {
        // comment till end of line
        do {
            LastChar = getchar();
        }while (LastChar != EOF || LastChar != '\n' || LastChar != '\r');

        if (LastChar != EOF)
            return gettok();
    }

    // do not eat EOF
    if (LastChar == EOF)
        return Lexer::tok_eof;

    //return char as its ascii value
    int thisChar = LastChar;
    LastChar = getchar();
    return thisChar;
}

double Lexer::getNumVal() {
    return NumVal;
}

std::string Lexer::getIdentifierStr() {
    return IdentifierStr;
}
