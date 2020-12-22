#include <sstream>

#include <Program.h>
#include <Instruction.h>

namespace kazm {

    std::string Program::str() {

        std::stringstream ss;

        ss << "Program {\n";
        for (std::size_t i = 0; i < instructions.size(); i++) {
            ss << "    " << instructions[i]->str() << std::endl;
        }
        ss << "}\n";

        return ss.str();
    }

    void Program::run() {

        for (std::size_t i = 0; i < instructions.size(); i++) {
            instructions[i]->execute();
        }

    }

}
