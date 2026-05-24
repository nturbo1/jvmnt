#include "parser/parser.h"

#include "base.h"
#include "parser/class_file.h"

#include <iostream>
#include <fstream>
#include <cstdio>

int main()
{
    std::ifstream class_file_src{"Main.class", std::ios::binary };

    if (class_file_src.fail())
        log_fatal("Failed to open the class file.");

    log_debug("Successfully opened the class file.");

    ClassFileReader cf_reader{ class_file_src };
    ClassFileParser cf_parser{ cf_reader };

    ClassFile cf{ cf_parser.parse() };

    // TODO: SHOULD BE REMOVED LATER: For testing only!!!
    std::cout << cf << std::endl;

    return 0;
}
