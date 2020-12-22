#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <vector>
#include <memory>
#include <string>

#include <Data.h>
#include <Expression.h>
#include <Gate.h>
#include <BigInt.h>

namespace kazm {

    enum InstructionType {

        instruction_nop,
        instruction_barrier,
        instruction_measure,
        instruction_reset,
        instruction_call,
        instruction_if        

    };

    struct Instruction {

        InstructionType type;

        Instruction(InstructionType);

        virtual ~Instruction() = default;

        virtual std::string str() = 0;
        virtual void execute() = 0;
    };

    struct BarrierInst : public Instruction {

        std::vector<std::shared_ptr<Data> > bits;

        BarrierInst(const std::vector<std::shared_ptr<Data> >&);

        std::string str() override;
        void execute() override;
    };

    struct MeasureInst : public Instruction {

        std::shared_ptr<Data> q;
        std::shared_ptr<Data> c;

        MeasureInst(const std::shared_ptr<Data>&, const std::shared_ptr<Data>&);

        std::string str() override;
        void execute() override;
    };

    struct ResetInst : public Instruction {

        std::shared_ptr<Data> q;

        ResetInst(const std::shared_ptr<Data>&);

        std::string str() override;
        void execute() override;
    };

    struct CallInst : public Instruction {

        std::shared_ptr<Gate> gate;
        std::vector<std::shared_ptr<Expression> > params;
        std::vector<std::shared_ptr<Data> > bits;

        CallInst(const std::shared_ptr<Gate>&, const std::vector<std::shared_ptr<Data> >&);
        CallInst(const std::shared_ptr<Gate>&, const std::vector<std::shared_ptr<Expression> >&, const std::vector<std::shared_ptr<Data> >&);

        std::string str() override;
        void execute() override;
    };

    struct IfInst : public Instruction {

        std::shared_ptr<Data> creg;
        BigInt num;
        std::shared_ptr<Instruction> inst;

        IfInst(const std::shared_ptr<Data>&, const std::string&, const std::shared_ptr<Instruction>&);        

        std::string str() override;
        void execute() override;

    };
}

#endif
