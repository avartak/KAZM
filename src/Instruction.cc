#include <sstream>

#include <Instruction.h>
#include <Expression.h>
#include <Bit.h>

namespace kazm {

    Instruction::Instruction(InstructionType t):
        type(t)
    {
    }

    BarrierInst::BarrierInst(const std::vector<std::shared_ptr<Data> >& qv):
        Instruction(instruction_barrier)
    {
        for (const std::shared_ptr<Data>& q : qv) bits.push_back(q);
    }

    std::string BarrierInst::str() {
        std::stringstream ss;
        ss << "barrier on ";
        for (std::size_t i = 0; i < bits.size(); i++) {
            auto b = bits[i];
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

    MeasureInst::MeasureInst(const std::shared_ptr<Data>& q_, const std::shared_ptr<Data>& c_):
        Instruction(instruction_measure),
        q(q_),
        c(c_)
    {
    }

    std::string MeasureInst::str() {
        std::stringstream ss;

        ss << "measure " << q->name();
        if (q->isBit()) ss << "[" << dynamic_cast<Bit*>(q.get())->index() << "]";

        ss << " in " << c->name();
        if (c->isBit()) ss << "[" << dynamic_cast<Bit*>(c.get())->index() << "]";

        return ss.str();
    }

    void MeasureInst::execute() {
    }

    ResetInst::ResetInst(const std::shared_ptr<Data>& q_):
        Instruction(instruction_reset),
        q(q_)
    {
    }

    std::string ResetInst::str() {
        std::stringstream ss;

        ss << "reset ";
        if (q->isReg()) ss << "register " << q->name();
        if (q->isBit()) ss << "qubit " << q->name() << "[" << dynamic_cast<Bit*>(q.get())->index() << "]";

        return ss.str();
    }

    void ResetInst::execute() {
    }

    CallInst::CallInst(const std::shared_ptr<Gate>& g, const std::vector<std::shared_ptr<Data> >& b):
        Instruction(instruction_call),
        gate(g)
    {
        for (const std::shared_ptr<Data>& i : b) bits.push_back(i);
    }

    CallInst::CallInst(const std::shared_ptr<Gate>& g, const std::vector<std::shared_ptr<Expression> >& pv, const std::vector<std::shared_ptr<Data> >& bv):
        Instruction(instruction_call),
        gate(g)
    {
        for (const std::shared_ptr<Data>& b : bv) bits.push_back(b);
        for (const std::shared_ptr<Expression>& p : pv) params.push_back(p);
    }

    std::string CallInst::str() {
        std::stringstream ss;

        ss << "call gate " << gate->name << " on ";
        for (std::size_t i = 0; i < bits.size(); i++) {
            auto qb = bits[i];
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
            auto p = params[i];
            ss << p->str();
            if (i != params.size()-1) ss << ",";
            ss << " ";
        }

        return ss.str();
    }

    void CallInst::execute() {
        gate->execute(params, bits);
    }

    IfInst::IfInst(const std::shared_ptr<Data>& cr, const std::string& n, const std::shared_ptr<Instruction>& i):
        Instruction(instruction_if),
        creg(cr),
        num(n),
        inst(i)
    {
    }

    std::string IfInst::str() {
        std::stringstream ss;
        ss << inst->str() << " if " << creg->name() << " = " << num.str;
        return ss.str();
    }

    void IfInst::execute() {
    }

}
