#ifndef PROGRAM_H
#define PROGRAM_H

#include <memory>
#include <vector>

namespace kazm {

    struct Data;
    struct Expression;
    struct ExpOps;
    struct Instruction;

    struct Program {

        std::vector<std::shared_ptr<Data> > bstack;
        std::vector<std::shared_ptr<Expression> > pstack;

        std::vector<std::shared_ptr<ExpOps> > ops;
        std::vector<std::shared_ptr<Instruction> > instructions;

        virtual ~Program() = default;

        virtual void setupPStack();
		virtual std::string str();

        void run();

    };

}

#endif
