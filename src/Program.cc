#include <sstream>

#include <Program.h>
#include <Data.h>
#include <Expression.h>
#include <Constant.h>
#include <ExpOps.h>
#include <Instruction.h>

namespace kazm {

    void Program::setupPStack() {

        for (std::size_t i = 0; i < ops.size(); i++) {
            if (ops[i]->isUnary()) {
                auto uop = dynamic_cast<UnaryOp*>(ops[i].get());
                pstack.push_back(std::make_shared<UnaryExpression>(uop->op, pstack[uop->exp]));
            }            
            else if (ops[i]->isBinary()) {
                auto bop = dynamic_cast<BinaryOp*>(ops[i].get());
                pstack.push_back(std::make_shared<BinaryExpression>(bop->op, pstack[bop->lhs], pstack[bop->rhs]));
            }
            else {
                auto cop = dynamic_cast<ConstOp*>(ops[i].get());
                pstack.push_back(std::make_shared<Constant>(cop->val));
            }        
        }        

    }

    std::string Program::str() {

        setupPStack();

        std::stringstream ss;

        ss << "Program {\n";
        for (std::size_t i = 0; i < instructions.size(); i++) {
            ss << "    " << instructions[i]->str() << std::endl;
        }
        ss << "}\n";

        pstack.clear();
        bstack.clear();

        return ss.str();
    }

    void Program::run() {

        setupPStack();

        for (std::size_t i = 0; i < instructions.size(); i++) {
            instructions[i]->execute();
        }

        pstack.clear();
        bstack.clear();

    }

}
