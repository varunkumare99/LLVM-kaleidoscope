#ifndef LEXER_H
#define LEXER_H

#include <iostream>
class Lexer {
    public:
		Lexer(std::string str = "", double val = 0.0);
        enum Token {
            tok_eof = -1,

            //commands
            tok_def = -2,
            tok_extern = -3,

            //primary
            tok_identifier = -4,
            tok_number = -5
        };

        int gettok();
        double getNumVal();
        std::string getIdentifierStr();

    private:
        std::string IdentifierStr; //filled if tok_identifier
        double NumVal; // filled if tok_number
};

#endif
