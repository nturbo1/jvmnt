#include "base.h"
#include "classFile/classFile.h"
#include "classFile/classFileParser.h"

#include <iostream>
#include <fstream>

int main()
{
    try
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

        cf.format_check();
    }
    catch (...) {}

    return 0;
}
