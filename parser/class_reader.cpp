#include "class_reader.h"

#include "base.h"

#include <istream>

ClassFileReader::ClassFileReader(std::istream& src)
    : m_src{ src }
{}

u1 ClassFileReader::read_u1()
{
    u1 b{};
    if (!m_src.read(reinterpret_cast<char*>(&b), 1))
        log_fatal("Failed to read a byte from a class file.");

    return b;
}

u2 ClassFileReader::read_u2()
{
    u1 b1{ read_u1() };
    u1 b2{ read_u1() };

    return (static_cast<u2>(b1) << 8) |
            static_cast<u2>(b2);
}

u4 ClassFileReader::read_u4()
{
    u1 b1{ read_u1() };
    u1 b2{ read_u1() };
    u1 b3{ read_u1() };
    u1 b4{ read_u1() };

    return
        (static_cast<u4>(b1) << 24) |
        (static_cast<u4>(b2) << 16) |
        (static_cast<u4>(b3) << 8)  |
        static_cast<u4>(b4);
}
