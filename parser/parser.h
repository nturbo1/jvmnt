#ifndef PARSER_H
#define PARSER_H

#include "class_file.h"
#include "class_reader.h"

#include <string>

class ClassFileParser
{
public:
    explicit ClassFileParser(ClassFileReader& reader);

    ClassFile parse();

private:
    ClassFileReader& m_reader;
};

std::ostream& operator<<(std::ostream& os, const ClassFile& cf);

#endif // PARSER_H
