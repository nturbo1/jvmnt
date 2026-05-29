#include "parser/parser.h"

#include "base.h"
#include "parser/class_file.h"

#include <iostream>
#include <fstream>
#include <cstdio>

int main()
{
    std::string src_file{ "Main.class" };
    std::ifstream class_file_src{ src_file, std::ios::binary };

    if (class_file_src.fail())
        log_fatal("Failed to open the class file.");

    log_debug("Successfully opened the class file.");

    ClassFileReader cf_reader{ class_file_src };
    ClassFileParser cf_parser{ cf_reader, src_file };

    ClassFile cf{ cf_parser.parse() };

    // TODO: SHOULD BE REMOVED LATER: For testing only!!!
    std::cout << cf << std::endl;

    return 0;
}
