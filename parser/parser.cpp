#include "parser.h"

#include "class_file.h"

#include <string>
#include <iostream>
#include <fstream>

ClassFileParser::ClassFileParser(ClassFileReader& reader)
    : m_reader{ reader }
{}

ClassFile ClassFileParser::parse()
{
    u4 magic{ m_reader.read_u4() };

    return ClassFile{ magic };
}
