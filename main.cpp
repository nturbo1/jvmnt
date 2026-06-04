#include "parser/parser.h"

#include "base.h"
#include "classFile/classFile.h"

#include <iostream>
#include <fstream>
#include <cstdio>

int main()
{
    try
    {
        std::string srcFile{ "Main.class" };
        std::ifstream classFileSrc{ srcFile, std::ios::binary };

        if (classFileSrc.fail())
            logFatal("Failed to open the class file.");

        logDebug("Successfully opened the class file.");

        ClassFileReader cfReader{ classFileSrc };
        ClassFileParser cfParser{ cfReader, srcFile };

        ClassFile cf{ cfParser.parse() };

        // TODO: SHOULD BE REMOVED LATER: For testing only!!!
        std::cout << cf << std::endl;

        cf.formatCheck();
    }
    catch (...) {}

    return 0;
}
