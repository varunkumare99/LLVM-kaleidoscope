#ifndef PROTOTYPEAST_H
#define PROTOTYPEAST_H

#include <iostream>
#include <memory>
#include <vector>
#include "llvm/IR/Function.h"
using namespace llvm;

class PrototypeAST {
    private:
        std::string Name;
        std::vector<std::string> Args; //capture the function name and arguments
		bool IsOperator;
		unsigned Precedence; //Precedence if Operator is binary

    public:
        PrototypeAST(const std::string& Name, std::vector<std::string> Args, bool IsOperator = false, unsigned Prec = 0);
        const std::string &getName() const;
		const std::vector<std::string>& getArgs() const;
		const void setName(const std::string &name);
		Function *codegen();

		bool isUnaryOp() const;
		bool isBinaryOp() const;
		unsigned getBinaryPrecedence() const;
		char getOperatorName() const;
};

#endif
