#include "parser/parser.h"

#include <iostream>
#include <fstream>

int main()
{
    std::ifstream class_file{"Main.class", std::ios::binary };
    if (class_file.fail())
    {
        std::cout << "Failed to open the class file." << std::endl;
        return 1;
    }

    std::cout << "Successfully opened the class file.\n";

    int ch{ class_file.get() };
    while(!class_file.eof())
    {
        std::cout << ch;
        ch = class_file.get();
    }
    std::cout << "\n\n==========================================================\n"
        << "End of file\n";

    ClassFileParser class_file_parser = ClassFileParser{};
    class_file_parser.parseFile("Main.class");

    return 0;
}
