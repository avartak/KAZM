#ifndef BIT_H
#define BIT_H

#include <memory>
#include <string>

#include <Data.h>
#include <Register.h>

namespace kazm {

    struct Bit : public Data {

        private:
            std::shared_ptr<Register> _reg;
            std::size_t _index;

        public:
            Bit(const std::shared_ptr<Register>&, std::size_t);

            virtual std::string name() override;
            virtual DataType type() throw (Exception) override;
            virtual std::size_t size() throw (Exception) override;
            virtual std::size_t offset() throw (Exception) override;
            virtual bool isReg() throw (Exception) override;

            std::size_t index();
            std::shared_ptr<Register> reg();

    };

}

#endif
