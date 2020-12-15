#include <sstream>

#include <Program.h>
#include <Data.h>
#include <Expression.h>
#include <Constant.h>
#include <ExpOps.h>
#include <Instruction.h>

namespace kazm {

    std::string Program::str() {

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

        for (std::size_t i = 0; i < instructions.size(); i++) {
            instructions[i]->execute();
        }

        pstack.clear();
        bstack.clear();

    }

}
