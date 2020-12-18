#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <string>
#include <memory>

#include <Data.h>

namespace kazm {

    struct Argument : Data {

        private:
            std::string _name;
            std::shared_ptr<Data> _arg;

        public:
            Argument(const std::string&);
            Argument(const std::string&, const std::shared_ptr<Data>&);

            virtual std::string name() override;
            virtual DataType type() throw (Exception) override;
            virtual std::size_t size() throw (Exception) override;
            virtual std::size_t offset() throw (Exception) override;
            virtual bool isReg() throw (Exception) override;

            std::shared_ptr<Data> arg();
            void set(const std::shared_ptr<Data>&);
            void reset();            

    };

}

#endif
