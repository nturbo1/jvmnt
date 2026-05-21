#include "parser.h"

#include "class_file.h"

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

ClassFileParser::ClassFileParser(ClassFileReader& reader)
    : m_reader{ reader }
{}

ClassFile ClassFileParser::parse()
{
    u4 magic{ m_reader.read_u4() };
    u2 minor_version{ m_reader.read_u2() };
    u2 major_version{ m_reader.read_u2() };

    return ClassFile{ magic, minor_version, major_version };
}

std::ostream& operator<<(std::ostream& os, const ClassFile& cf)
{
    os << "{\n"
       << "\tmagic: " << std::hex << cf.magic << ",\n"
       << "\tminor_version: " << cf.minor_version << ",\n"
       << "\tmajor_version: " << cf.major_version << "\n"
       << "}";

    os << std::dec;

    return os;
}
