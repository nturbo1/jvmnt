#ifndef PARSER_CLASS_READER_H
#define PARSER_CLASS_READER_H

#include "base.h"

#include <istream>

class ClassFileReader
{
public:
    explicit ClassFileReader(std::istream& src);

    u1 read_u1();
    u2 read_u2();
    u4 read_u4();

private:
    std::istream& m_src;
};
#endif // PARSER_CLASS_READER_H
