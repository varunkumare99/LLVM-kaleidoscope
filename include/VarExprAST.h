#ifndef VAREXPRAST_H
#define VAREXPRAST_H

#include <vector>
#include "ExprAST.h"

/// VarExprAST - Expression class for var/in
class VarExprAST: public ExprAST {
	private:
		std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames;
		std::unique_ptr<ExprAST> Body;

	public:
		VarExprAST(std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames, 
				std::unique_ptr<ExprAST> Body);

		Value *codegen() override;
};

#endif
