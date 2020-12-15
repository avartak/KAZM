#include <sstream>
#include <Exception.h>

namespace kazm {

    Exception::Exception(const std::string& m):
        filename(""),
        line(0)
    {
        std::stringstream out;
        out << "[KAZM error]" << ": " << m;
        message = out.str();
    }

    Exception::Exception(const std::string& f, const std::string& m):
        filename(f),
        line(0),
        message(m)
    {
        std::stringstream out;
        out << "[KAZM error]";
        if (filename == "") out << ": " << message;
        else out << " in " << filename << ": " << message;
        message = out.str();
    }

    Exception::Exception(const std::string& f, int l, const std::string& m):
        filename(f),
        line(l),
        message(m)
    {
        std::stringstream out;
        out << "[KAZM error]";
        if (filename == "") out << ": " << message;
        else {
            if (line != 0) out << " on line " << line << " in " << filename << ": " << message;
            else out << " in " << filename << ": " << message;
        }
        message = out.str();
    }

    const char* Exception::what() const noexcept {

        return message.c_str();
    }
}
