#include "PrototypeAST.h"
#include "Codegen.h"
#include <vector>
#include "DebugData.h"

PrototypeAST::PrototypeAST(SourceLocation Loc, const std::string& Name, std::vector<std::string> Args, bool IsOperator, unsigned Precedence)
    :Name(Name), Args(std::move(Args)), IsOperator(IsOperator), Precedence(Precedence), Line(Loc.Line) {}

const std::string& PrototypeAST::getName() const {
    return Name;
}

const void PrototypeAST::setName(const std::string &name) {
	Name = name;
}

const std::vector<std::string>& PrototypeAST::getArgs() const {
	return Args;
}

bool PrototypeAST::isUnaryOp() const {
	return IsOperator && Args.size() == 1;
}

bool PrototypeAST::isBinaryOp() const {
	return IsOperator && Args.size() == 2;
}

unsigned PrototypeAST::getBinaryPrecedence() const {
	return Precedence;
}

char PrototypeAST::getOperatorName() const {
	assert(isUnaryOp() || isBinaryOp());
	return Name[Name.size() - 1];
}

Function *PrototypeAST::codegen() {
	//Make a function which returns a double takes args as n doubles. Since in llvm all are doubles. f(double n times) return double
	std::vector<Type *> Doubles(Args.size(), Type::getDoubleTy(*Codegen::Thecontext));
	FunctionType *FT = FunctionType::get(Type::getDoubleTy(*Codegen::Thecontext), Doubles, false);

	//External function thus it can called for outside.
	//We are adding funtion name, Name to TheModule(since it holds all the functio names)
	Function *F = Function::Create(FT, Function::ExternalLinkage, Name, Codegen::TheModule.get());

	//we setting the names of args in F, from the ones by to ProtoTypeAST
	unsigned Idx = 0;
	for (auto &Arg : F->args())
		Arg.setName(Args[Idx++]);
	//this sets our from function prototype in LLVM
	return F;
}

int PrototypeAST::getLine() const {
	return Line;
}
