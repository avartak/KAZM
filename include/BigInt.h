#include <string>
#include <vector>
#include <utility>

namespace kazm {

    struct BigInt {

        std::string str;
        std::vector<uint64_t> num;
        
        BigInt(const std::string&);
        
        std::pair<char, char> divideByTwo(const std::pair<char, char>&);
        void setBit(std::size_t);
        bool getBit(std::size_t);

    };

}
