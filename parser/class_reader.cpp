#include "class_reader.h"

#include <istream>

ClassFileReader::ClassFileReader(std::istream& src)
    : m_src{ src }
{}

u1 ClassFileReader::read_u1()
{
    u1 b;
    m_src >> b;
    return b;
}

u2 ClassFileReader::read_u2()
{
    u1 b1{ read_u1() };
    u1 b2{ read_u1() };

    return (b1 << 8) | b2;
}

u4 ClassFileReader::read_u4()
{
    u1 b1{ read_u1() };
    u1 b2{ read_u1() };
    u1 b3{ read_u1() };
    u1 b4{ read_u1() };

    return
        (b1 << 24) |
        (b2 << 16) |
        (b3 << 8)  |
        b4;
}
