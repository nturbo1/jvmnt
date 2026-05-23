#include "const_pool.h"

#include <ostream>

ConstPoolEntry::ConstPoolEntry(ConstPoolEntryTag t)
    : tag{ t }
{}

ConstDoubleInfo::ConstDoubleInfo(ConstPoolEntryTag t, u4 h_bytes, u4 l_bytes)
    : ConstPoolEntry(t),
    high_bytes{ h_bytes },
    low_bytes{ l_bytes }
{}

ConstInvokeDynamicInfo::ConstInvokeDynamicInfo(
    ConstPoolEntryTag t,
    u2 bootstrp_method_attr_idx,
    u2 name_and_type_idx
)
    : ConstPoolEntry(t),
    bootstrap_method_attr_index{ bootstrp_method_attr_idx },
    name_and_type_index{ name_and_type_idx }
{}

ConstClassInfo::ConstClassInfo(ConstPoolEntryTag t, u2 name_idx)
    : ConstPoolEntry(t),
    name_index{ name_idx }
{}

ConstUtf8Info::ConstUtf8Info(ConstPoolEntryTag t, std::vector<u1> bs)
    : ConstPoolEntry(t),
    bytes{ bs }
{}

std::ostream& operator<<(std::ostream& os, const ConstPoolEntryTag& t)
{
    switch(t) {
    case ConstPoolEntryTag::CONSTANT_Class:
        os << "CONSTANT_Class";
        break;
    case ConstPoolEntryTag::CONSTANT_Fieldref:
        os << "CONSTANT_Fieldref";
        break;
    case ConstPoolEntryTag::CONSTANT_Methodref:
        os << "CONSTANT_Methodref";
        break;
    case ConstPoolEntryTag::CONSTANT_InterfaceMethodref:
        os << "CONSTANT_InterfaceMethodref";
        break;
    case ConstPoolEntryTag::CONSTANT_String:
        os << "CONSTANT_String";
        break;
    case ConstPoolEntryTag::CONSTANT_Integer:
        os << "CONSTANT_Integer";
        break;
    case ConstPoolEntryTag::CONSTANT_Float:
        os << "CONSTANT_Float";
        break;
    case ConstPoolEntryTag::CONSTANT_Long:
        os << "CONSTANT_Long";
        break;
    case ConstPoolEntryTag::CONSTANT_Double:
        os << "CONSTANT_Double";
        break;
    case ConstPoolEntryTag::CONSTANT_NameAndType:
        os << "CONSTANT_NameAndType";
        break;
    case ConstPoolEntryTag::CONSTANT_Utf8:
        os << "CONSTANT_Utf8";
        break;
    case ConstPoolEntryTag::CONSTANT_MethodHandle:
        os << "CONSTANT_MethodHandle";
        break;
    case ConstPoolEntryTag::CONSTANT_MethodType:
        os << "CONSTANT_MethodType";
        break;
    case ConstPoolEntryTag::CONSTANT_InvokeDynamic:
        os << "CONSTANT_InvokeDynamic";
        break;
    default:
        os << "INVALID";
    }

    return os;
}
