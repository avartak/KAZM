#include <iostream>
#include <Globals.h>
#include <Exception.h>

int main(int argc, char* argv[]) {

    try {
        if (argc != 2) throw kazm::Exception("Expect one command line argument -- name of the source file");
        kazm::TheParser.parse(argv[1]);
        std::cout << kazm::TheParser.str();
    }
    catch (const kazm::Exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

