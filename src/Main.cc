#include <memory>
#include <iostream>

#include <Parser.h>
#include <Exception.h>

int main(int argc, char* argv[]) {

    auto parser = std::make_shared<kazm::Parser>();

    try {
        if (argc != 2) throw kazm::Exception("Expect one command line argument -- name of the source file");
        parser->parse(argv[1]);
        std::cout << parser->str();
    }
    catch (const kazm::Exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

