#ifndef PARSER_H
#define PARSER_H

#include "class_file.h"

#include <string>

class ClassFileParser
{
public:
    void parseFile(const std::string& filename);
};

#endif // PARSER_H
