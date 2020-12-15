#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

namespace kazm {

    struct Exception : public std::exception {
    
        std::string filename;
        int line;        
        std::string message;

        Exception(const std::string&);
        Exception(const std::string&, const std::string&);
        Exception(const std::string&, int, const std::string&);
        
        const char* what() const noexcept;
    
    };

}

#endif
