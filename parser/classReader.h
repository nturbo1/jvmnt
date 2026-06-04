#ifndef PARSER_CLASS_READER_H
#define PARSER_CLASS_READER_H

#include "base.h"

#include <istream>

class ClassFileReader
{
public:
    explicit ClassFileReader(std::istream& src);

    u1 readU1();
    u2 readU2();
    u4 readU4();

private:
    std::istream& m_src;
};
#endif // PARSER_CLASS_READER_H
