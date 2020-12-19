#include <BigInt.h>

namespace kazm {

    std::pair<char, char> BigInt::divideByTwo(const std::pair<char, char>& n) {

        if (n.first == '0' && n.second == '0') return std::pair<char, char>('0', '0');
        if (n.first == '0' && n.second == '1') return std::pair<char, char>('0', '1');
        if (n.first == '0' && n.second == '2') return std::pair<char, char>('1', '0');
        if (n.first == '0' && n.second == '3') return std::pair<char, char>('1', '1');
        if (n.first == '0' && n.second == '4') return std::pair<char, char>('2', '0');
        if (n.first == '0' && n.second == '5') return std::pair<char, char>('2', '1');
        if (n.first == '0' && n.second == '6') return std::pair<char, char>('3', '0');
        if (n.first == '0' && n.second == '7') return std::pair<char, char>('3', '1');
        if (n.first == '0' && n.second == '8') return std::pair<char, char>('4', '0');
        if (n.first == '0' && n.second == '9') return std::pair<char, char>('4', '1');
        if (n.first == '1' && n.second == '0') return std::pair<char, char>('5', '0');
        if (n.first == '1' && n.second == '1') return std::pair<char, char>('5', '1');
        if (n.first == '1' && n.second == '2') return std::pair<char, char>('6', '0');
        if (n.first == '1' && n.second == '3') return std::pair<char, char>('6', '1');
        if (n.first == '1' && n.second == '4') return std::pair<char, char>('7', '0');
        if (n.first == '1' && n.second == '5') return std::pair<char, char>('7', '1');
        if (n.first == '1' && n.second == '6') return std::pair<char, char>('8', '0');
        if (n.first == '1' && n.second == '7') return std::pair<char, char>('8', '1');
        if (n.first == '1' && n.second == '8') return std::pair<char, char>('9', '0');
        if (n.first == '1' && n.second == '9') return std::pair<char, char>('9', '1');
        
        return std::pair<char, char>('0', '0');
    }

    void BigInt::setBit(std::size_t b) {

        std::size_t idx = b/64;
        std::size_t pos = b%64;
        
        while(idx >= num.size()) num.push_back(0);
        num[idx] |= (1 << pos);

    }

    bool BigInt::getBit(std::size_t b) {

        std::size_t idx = b/64;
        std::size_t pos = b%64;

        while(idx >= num.size()) num.push_back(0);
        return ( (num[idx] & (1 << pos)) != 0);

    }

    BigInt::BigInt(const std::string& s): 
        str(s)
    {
        std::string dividend = str;
        std::size_t bit = 0;
        
        while (dividend != "0") {
            std::string division = "";
            char carry = '0';
            for (std::size_t i = 0; i < dividend.length(); i++) {
                auto result = divideByTwo(std::pair<char, char>(carry, dividend[i]));
                division += result.first;
                carry = result.second;
            }
            if (carry == '1') setBit(bit);
            bit++;
            dividend = division;
            if (dividend.size() > 1 && dividend[0] == '0') dividend = dividend.substr(1);
        }
    }

}
