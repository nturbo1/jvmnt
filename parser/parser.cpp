#include "parser.h"

#include "class_file.h"
#include "base.h"

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <cassert>
#include <stdexcept>
#include <cstddef>

ClassFileParser::ClassFileParser(ClassFileReader& reader)
    : m_reader{ reader }
{}

ClassFile ClassFileParser::parse()
{
    u4 magic{ m_reader.read_u4() };
    // TODO: Validate the magic number!
    u2 minor_version{ m_reader.read_u2() };
    u2 major_version{ m_reader.read_u2() };

    std::vector<std::unique_ptr<ConstPoolEntry>> const_pool{ parse_const_pool() };

    return ClassFile{
            magic,
            minor_version,
            major_version,
            std::move(const_pool)
    };
}

ConstPoolEntryTag matchConstPoolEntryTag(u1 tag)
{
    ConstPoolEntryTag t{ static_cast<ConstPoolEntryTag>(tag) };
    switch(t) {
    case ConstPoolEntryTag::CONSTANT_Class:
        return ConstPoolEntryTag::CONSTANT_Class;
    case ConstPoolEntryTag::CONSTANT_Fieldref:
        return ConstPoolEntryTag::CONSTANT_Fieldref;
    case ConstPoolEntryTag::CONSTANT_Methodref:
        return ConstPoolEntryTag::CONSTANT_Methodref;
    case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
        return ConstPoolEntryTag::CONSTANT_InterfaceMethodref;
    case ConstPoolEntryTag::CONSTANT_String:
        return ConstPoolEntryTag::CONSTANT_String;
    case ConstPoolEntryTag::CONSTANT_Integer:
        return ConstPoolEntryTag::CONSTANT_Integer;
    case ConstPoolEntryTag::CONSTANT_Float:
        return ConstPoolEntryTag::CONSTANT_Float;
    case ConstPoolEntryTag::CONSTANT_Long:
        return ConstPoolEntryTag::CONSTANT_Long;
    case ConstPoolEntryTag::CONSTANT_Double:
        return ConstPoolEntryTag::CONSTANT_Double;
    case ConstPoolEntryTag::CONSTANT_NameAndType:
        return ConstPoolEntryTag::CONSTANT_NameAndType;
    case ConstPoolEntryTag::CONSTANT_Utf8:
        return ConstPoolEntryTag::CONSTANT_Utf8;
    case ConstPoolEntryTag::CONSTANT_MethodHandle:
        return ConstPoolEntryTag::CONSTANT_MethodHandle;
    case ConstPoolEntryTag::CONSTANT_MethodType:
        return ConstPoolEntryTag::CONSTANT_MethodType;
    case ConstPoolEntryTag::CONSTANT_InvokeDynamic:
        return ConstPoolEntryTag::CONSTANT_InvokeDynamic;
    default:
        return ConstPoolEntryTag::INVALID;
    }
}

std::vector<std::unique_ptr<ConstPoolEntry>> ClassFileParser::parse_const_pool()
{
    u2 const_pool_count{ m_reader.read_u2() };
    // TODO: REMOVE WHEN YOU'RE DONE!
    std::cout << "const_pool_count = " << const_pool_count << "\n";

    m_reader.read_u1(); // Skipping a byte because it is 0 for some reason even though
                        // it should not be 0.

    // cp_info constant_pool[constant_pool_count-1]
    std::vector<std::unique_ptr<ConstPoolEntry>> const_pool;
    for (int i = 1; i < const_pool_count; i++)
    {
        u1 t{ m_reader.read_u1() };
        ConstPoolEntryTag tag{ matchConstPoolEntryTag(t) };
        if (tag == ConstPoolEntryTag::INVALID)
            log_fatal("Constant pool entry at index %d has an invalid tag.", i);

        switch(tag)
        {
        case ConstPoolEntryTag::CONSTANT_Class:
            log_fixme("Implement constant pool entry 'CONSTANT_Class' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_Fieldref:
            log_fixme("Implement constant pool entry 'CONSTANT_Fieldref' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_Methodref:
            log_fixme("Implement constant pool entry 'CONSTANT_Methodref' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
            log_fixme("Implement constant pool entry 'CONSTANT_InterfaceMethodref' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_String:
            log_fixme("Implement constant pool entry 'CONSTANT_String' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_Integer:
            log_fixme("Implement constant pool entry 'CONSTANT_Integer' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_Float:
            log_fixme("Implement constant pool entry 'CONSTANT_Float' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_Long:
            log_fixme("Implement constant pool entry 'CONSTANT_Long' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_Double:
            const_pool.push_back(parse_const_double(tag));
            break;
        case ConstPoolEntryTag::CONSTANT_NameAndType:
            log_fixme("Implement constant pool entry 'CONSTANT_NameAndType' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_Utf8:
            log_fixme("Implement constant pool entry 'CONSTANT_Utf8' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_MethodHandle:
            log_fixme("Implement constant pool entry 'CONSTANT_MethodHandle' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_MethodType:
            log_fixme("Implement constant pool entry 'CONSTANT_MethodType' parser!");
            break;
        case ConstPoolEntryTag::CONSTANT_InvokeDynamic:
            log_fixme("Implement constant pool entry 'CONSTANT_InvokeDynamic' parser!");
            break;
        default:
            assert(false && "The constant pool entry tag was not validated beforehand!");
        }

        // TODO: Should be removed!!!
        break;
    }

    return const_pool;
}

std::unique_ptr<ConstDoubleInfo> ClassFileParser::parse_const_double(ConstPoolEntryTag tag)
{
    assert(tag == ConstPoolEntryTag::CONSTANT_Double &&
            "Non 'CONSTANT_Double' constant pool entry tag value was passed to the constant double info parser");
    u4 high_bytes{ m_reader.read_u4() };
    u4 low_bytes{ m_reader.read_u4() };

    return std::make_unique<ConstDoubleInfo>(tag, high_bytes, low_bytes);
}

static void print(std::ostream& os, const ConstPoolEntry& e, const std::string& indent)
{
    os << indent << "{" << "\n"
       << indent << "\ttag: " << e.tag << ",\n";

    switch(e.tag)
    {
    case ConstPoolEntryTag::CONSTANT_Class:
        break;
    case ConstPoolEntryTag::CONSTANT_Fieldref:
        break;
    case ConstPoolEntryTag::CONSTANT_Methodref:
        break;
    case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
        break;
    case ConstPoolEntryTag::CONSTANT_String:
        break;
    case ConstPoolEntryTag::CONSTANT_Integer:
        break;
    case ConstPoolEntryTag::CONSTANT_Float:
        break;
    case ConstPoolEntryTag::CONSTANT_Long:
        break;
    case ConstPoolEntryTag::CONSTANT_Double:
    {
        const ConstDoubleInfo& cdi = static_cast<const ConstDoubleInfo&>(e);
        os << indent << "\thigh_bytes: " << std::hex << cdi.high_bytes << ",\n"
           << indent << "\tlow_bytes: " << std::hex << cdi.high_bytes << "\n"
           << std::dec;
        break;
    }
    case ConstPoolEntryTag::CONSTANT_NameAndType:
        break;
    case ConstPoolEntryTag::CONSTANT_Utf8:
        break;
    case ConstPoolEntryTag::CONSTANT_MethodHandle:
        break;
    case ConstPoolEntryTag::CONSTANT_MethodType:
        break;
    case ConstPoolEntryTag::CONSTANT_InvokeDynamic:
        break;
    }

    os << indent << "}\n";
}

std::ostream& operator<<(std::ostream& os, const ClassFile& cf)
{
    os << "{\n"
       << "\tmagic: " << std::hex << cf.magic << ",\n"
       << std::dec
       << "\tminor_version: " << cf.minor_version << ",\n"
       << "\tmajor_version: " << cf.major_version << "\n";

    if (cf.const_pool.size() > 0)
    {
        os << "\tconst_pool: [\n";
        for (std::size_t i = 0; i < cf.const_pool.size(); i++)
        {
            print(os, *cf.const_pool[i], "\t\t");
        }
        os << "\t]\n";
    }

    os << "}";

    os << std::dec;

    return os;
}
