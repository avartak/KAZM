#ifndef REGISTER_H
#define REGISTER_H

#include <string>

#include <Data.h>

namespace kazm {

    struct Register : public Data {

        private:
            std::string _name;
            DataType _type;
            std::size_t _size;
            std::size_t _offset;

        public:
            Register(DataType, const std::string&, std::size_t, std::size_t);
            virtual ~Register() = default;

            virtual std::string name() override;
            virtual DataType type() override;
            virtual std::size_t size() override;
            virtual std::size_t offset() override;
            virtual bool isReg() override;

            virtual std::string str();
    };

}

#endif
