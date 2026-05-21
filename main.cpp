#include "parser/parser.h"

#include "parser/class_file.h"

#include <iostream>
#include <fstream>
#include <cstdio>

int main()
{
    std::ifstream class_file_src{"Main.class", std::ios::binary };
    if (class_file_src.fail())
    {
        // TODO: Replace with a log function/method
        std::cout << "Failed to open the class file." << std::endl;
        return 1;
    }
    // TODO: Replace with a log function/method
    std::cout << "Successfully opened the class file.\n";

    ClassFileReader cf_reader{ class_file_src };
    ClassFileParser cf_parser{ cf_reader };

    ClassFile cf{ cf_parser.parse() };

    // For testing only
    std::cout << cf << std::endl;

    return 0;
}
