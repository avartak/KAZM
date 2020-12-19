#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <vector>
#include <memory>
#include <string>

#include <Program.h>
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
        const Program* caller;

        Instruction(InstructionType, const Program&);

        virtual ~Instruction() = default;

        virtual std::string str() = 0;
        virtual void execute() = 0;
    };

    struct BarrierInst : public Instruction {

        std::vector<std::size_t> bits;

        BarrierInst(const Program&, const std::vector<std::size_t>&);

        std::string str() override;
        void execute() override;
    };

    struct MeasureInst : public Instruction {

        std::size_t q;
        std::size_t c;

        MeasureInst(const Program&, std::size_t, std::size_t);

        std::string str() override;
        void execute() override;
    };

    struct ResetInst : public Instruction {

        std::size_t q;

        ResetInst(const Program&, std::size_t);

        std::string str() override;
        void execute() override;
    };

    struct CallInst : public Instruction {

        std::shared_ptr<Gate> gate;
        std::vector<std::size_t> params;
        std::vector<std::size_t> bits;

        CallInst(const Program&, const std::shared_ptr<Gate>&, const std::vector<std::size_t>&);
        CallInst(const Program&, const std::shared_ptr<Gate>&, const std::vector<std::size_t>&, const std::vector<std::size_t>&);

        std::string str() override;
        void execute() override;
    };

    struct IfInst : public Instruction {

        std::size_t creg;
        BigInt num;
        std::shared_ptr<Instruction> inst;

        IfInst(const Program&, std::size_t, const std::string&, const std::shared_ptr<Instruction>&);        

        std::string str() override;
        void execute() override;

    };
}

#endif
