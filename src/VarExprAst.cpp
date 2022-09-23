#include "VarExprAST.h"
#include <Codegen.h>

VarExprAST::VarExprAST(std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> VarNames,
		std::unique_ptr<ExprAST> Body)
	:VarNames(std::move(VarNames)), Body(std::move(Body)) {}


Value *VarExprAST::codegen() {
	std::vector<AllocaInst *> OldBindings;

	Function *TheFunction = Codegen::Builder->GetInsertBlock()->getParent();

	//Register all variables and emit their initialzers
	for (unsigned i = 0, e = VarNames.size(); i!=e; ++i) {
		const std::string &VarName = VarNames[i].first;
		ExprAST *Init = VarNames[i].second.get();

		/* Emit the initialzer before adding the variable to scope, this prevents
		 * the initialzer from referencing the variable itself, and permits stuff
		 * like this:
		 * var a = 1 in
		 * 		var a = a in ... # refers to outer 'a'
		 */

		Value *InitVal;
		if (Init) {
			InitVal = Init->codegen();
			if (!InitVal)
				return nullptr;
		}
		else {
			//default init is zero
			InitVal = ConstantFP::get(*Codegen::Thecontext, APFloat(0.0));
		}

		AllocaInst *Alloca = Codegen::CreateEntryBlockAlloca(TheFunction, VarName);
		Codegen::Builder->CreateStore(InitVal, Alloca);

		// Remember the old variable binding so that we can restore the binding when
		// we unrecurse
		OldBindings.push_back(Codegen::NamedValues[VarName]);

		//Remember this binding
		Codegen::NamedValues[VarName] = Alloca;
	}

	// codegen the body, now that all vars are in scope
	Value *BodyVal = Body->codegen();
	if (!BodyVal)
		return nullptr;

	//Pop all our variables from scope, set to old value
	for (unsigned i = 0, e = VarNames.size(); i != e; ++i) {
		Codegen::NamedValues[VarNames[i].first] = OldBindings[i];
	}

	//Return the body computation
	return BodyVal;
}
