#include <sstream>

#include <Instruction.h>
#include <Expression.h>
#include <Bit.h>

namespace kazm {

    Instruction::Instruction(InstructionType t, const Program& c):
        type(t),
        caller(&c)
    {
    }

    BarrierInst::BarrierInst(const Program& c, const std::vector<std::size_t>& q):
        Instruction(instruction_barrier, c)
    {
        for (const std::size_t& i : q) bits.push_back(i);
    }

    std::string BarrierInst::str() {
        std::stringstream ss;
        ss << "barrier on ";
        for (std::size_t i = 0; i < bits.size(); i++) {
            auto b = caller->bstack[bits[i]];
            ss << b->name();
            try {
                if (b->isBit()) ss << "[" << dynamic_cast<Bit*>(b.get())->index() << "]";
            }
            catch (const kazm::Exception& e) {
            }
            if (i != bits.size()-1) ss << ",";
            ss << " ";
        }
        ss << std::endl;
        return ss.str();
    }

    void BarrierInst::execute() {
    }

    MeasureInst::MeasureInst(const Program& c, std::size_t q_, std::size_t c_):
        Instruction(instruction_measure, c),
        q(q_),
        c(c_)
    {
    }

    std::string MeasureInst::str() {
        std::stringstream ss;

        auto qb = caller->bstack[q];
        auto cb = caller->bstack[c];

        ss << "measure " << qb->name();
        if (qb->isBit()) ss << "[" << dynamic_cast<Bit*>(qb.get())->index() << "]";

        ss << " in " << cb->name();
        if (cb->isBit()) ss << "[" << dynamic_cast<Bit*>(cb.get())->index() << "]";

        return ss.str();
    }

    void MeasureInst::execute() {
    }

    ResetInst::ResetInst(const Program& c, std::size_t q_):
        Instruction(instruction_reset, c),
        q(q_)
    {
    }

    std::string ResetInst::str() {
        std::stringstream ss;

        auto qb = caller->bstack[q];

        ss << "reset ";
        if (qb->isReg()) ss << "register " << qb->name();
        if (qb->isBit()) ss << "qubit " << qb->name() << "[" << dynamic_cast<Bit*>(qb.get())->index() << "]";

        return ss.str();
    }

    void ResetInst::execute() {
    }

    CallInst::CallInst(const Program& c, const std::shared_ptr<Gate>& g, const std::vector<std::size_t>& b):
        Instruction(instruction_call, c),
        gate(g)
    {
        for (const std::size_t& i : b) bits.push_back(i);
    }

    CallInst::CallInst(const Program& c, const std::shared_ptr<Gate>& g, const std::vector<std::size_t>& p, const std::vector<std::size_t>& b):
        Instruction(instruction_call, c),
        gate(g)
    {
        for (const std::size_t& i : b) bits.push_back(i);
        for (const std::size_t& i : p) params.push_back(i);
    }

    std::string CallInst::str() {
        std::stringstream ss;

        ss << "call gate " << gate->name << " on ";
        for (std::size_t i = 0; i < bits.size(); i++) {
            auto qb = caller->bstack[bits[i]];
            ss << qb->name();
            try {
                if (qb->isBit()) ss << "[" << dynamic_cast<Bit*>(qb.get())->index() << "]";
            }
            catch (const kazm::Exception& e) {
            }
            if (i != bits.size()-1) ss << ",";
            ss << " ";
        }

        if (params.size() == 0) return ss.str();

        ss << "using parameters ";
        for (std::size_t i = 0; i < params.size(); i++) {
            auto p = caller->pstack[params[i]];
            ss << p->str();
            if (i != params.size()-1) ss << ",";
            ss << " ";
        }

        return ss.str();
    }

    void CallInst::execute() {
        gate->execute(*caller, params, bits);
    }

    IfInst::IfInst(const Program& c, std::size_t cr, const std::string& n, const std::shared_ptr<Instruction>& i):
        Instruction(instruction_if, c),
        creg(cr),
        num(n),
        inst(i)
    {
    }

    std::string IfInst::str() {
        std::stringstream ss;
        ss << inst->str() << " if " << caller->bstack[creg]->name() << " = " << num.str;
        return ss.str();
    }

    void IfInst::execute() {
    }

}
