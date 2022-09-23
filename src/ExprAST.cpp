#include "ExprAST.h"

ExprAST::ExprAST(SourceLocation Loc): Loc(Loc) {}

int ExprAST::getLine() const{
	return Loc.Line;
}

int ExprAST::getCol() const {
	return Loc.Col;
}

raw_ostream &ExprAST::dump(raw_ostream &out, int ind) {
	return out << ':' << getLine()  << ':' << getCol() << '\n';
}
