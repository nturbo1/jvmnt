#include "constPool.h"

#include <ostream>

ConstPoolEntry::ConstPoolEntry(ConstPoolEntryTag t)
    : tag{ t }
{}

ConstMethodrefInfo::ConstMethodrefInfo(ConstPoolEntryTag t, u2 classIdx, u2 nameAndTypeIdx)
    : ConstPoolEntry(t),
    classIndex{ classIdx },
    nameAndTypeIndex{ nameAndTypeIdx }
{}

ConstFieldrefInfo::ConstFieldrefInfo(ConstPoolEntryTag t, u2 classIdx, u2 nameAndTypeIdx)
    : ConstPoolEntry(t),
    classIndex{ classIdx },
    nameAndTypeIndex{ nameAndTypeIdx }
{}

ConstStringInfo::ConstStringInfo(ConstPoolEntryTag t, u2 stringIdx)
    : ConstPoolEntry(t),
    stringIndex{ stringIdx }
{}

ConstDoubleInfo::ConstDoubleInfo(ConstPoolEntryTag t, u4 hBytes, u4 lBytes)
    : ConstPoolEntry(t),
    highBytes{ hBytes },
    lowBytes{ lBytes }
{}

ConstInvokeDynamicInfo::ConstInvokeDynamicInfo(
    ConstPoolEntryTag t,
    u2 bootstrpMethodAttrIdx,
    u2 nameAndTypeIdx
)
    : ConstPoolEntry(t),
    bootstrapMethodAttrIndex{ bootstrpMethodAttrIdx },
    nameAndTypeIndex{ nameAndTypeIdx }
{}

ConstClassInfo::ConstClassInfo(ConstPoolEntryTag t, u2 nameIdx)
    : ConstPoolEntry(t),
    nameIndex{ nameIdx }
{}

ConstUtf8Info::ConstUtf8Info(ConstPoolEntryTag t, std::vector<u1> bs)
    : ConstPoolEntry(t),
    bytes{ bs }
{}

ConstNameAndTypeInfo::ConstNameAndTypeInfo(ConstPoolEntryTag t, u2 nameIdx, u2 descriptorIdx)
    : ConstPoolEntry(t),
    nameIndex{ nameIdx },
    descriptorIndex{ descriptorIdx }
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
