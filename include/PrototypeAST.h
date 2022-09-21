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

    public:
        PrototypeAST(const std::string& Name, std::vector<std::string> Args);
        const std::string &getName() const;
		const std::vector<std::string>& getArgs() const;
		const void setName(const std::string &name);
		Function *codegen();
};

#endif
