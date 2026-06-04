#include "attributes.h"
#include "log.h"

#include <cassert>
#include <unordered_map>
#include <string_view>

AttrInfo::AttrInfo(u2 attrNameIdx)
    : attrNameIndex{ attrNameIdx }
{}

CodeAttrInfo::CodeAttrInfo(u2 attrNameIdx)
    : AttrInfo(attrNameIdx)
{}

LineNumberTableAttrInfo::LineNumberTableAttrInfo(u2 attrNameIdx)
    : AttrInfo(attrNameIdx)
{}

SourceFileAttrInfo::SourceFileAttrInfo(u2 attrNameIdx)
    : AttrInfo(attrNameIdx)
{}

AttrType resolveAttrType(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool,
        u2 attrNameIndex)
{
    // The class file format uses `1-based` indexes.
    // However, the parsed `constPool` vector stores the parsed constant pool items
    // in usual `0-based` indexes internally, so
    //     class file `index` == parsed `constPool` vector `index - 1`.
    ConstPoolEntryTag entryTag{ (constPool[attrNameIndex - 1])->tag };

    if (entryTag != ConstPoolEntryTag::CONSTANT_Utf8)
        logFatal("The `constant_pool` entry at `attribute_name_index` MUST be a "
                "`CONSTANT_Utf8_info` structure representing the name of the attribute.");

    ConstUtf8Info* const_utf8{ dynamic_cast<ConstUtf8Info*>(constPool[attrNameIndex - 1].get()) };
    assert(const_utf8 &&
            "The constant pool entry was not validated to be of type `CONSTANT_Utf8_info` or "
            "the constant pool entry tag does not match the entry info bytes, which means "
            "the constant pool entry was NOT parsed correctly or the constant pool entry was "
            "of invalid format and it was not caught on time!"); 

    static const std::unordered_map<std::string_view, AttrType> attrTypeTable = {
        { "ConstantValue", AttrType::ConstantValue },
        { "Code", AttrType::Code },
        { "StackMapTable", AttrType::StackMapTable },
        { "Exceptions", AttrType::Exceptions },
        { "BootstrapMethods", AttrType::BootstrapMethods },
        { "InnerClasses", AttrType::InnerClasses },
        { "EnclosingMethod", AttrType::EnclosingMethod },
        { "Synthetic", AttrType::Synthetic },
        { "Signature", AttrType::Signature },
        { "RuntimeVisibleAnnotations", AttrType::RuntimeVisibleAnnotations },
        { "RuntimeInvisibleAnnotations", AttrType::RuntimeInvisibleAnnotations },
        { "RuntimeVisibleParameterAnnotations", AttrType::RuntimeVisibleParameterAnnotations },
        { "RuntimeInvisibleParameterAnnotations", AttrType::RuntimeInvisibleParameterAnnotations },
        { "RuntimeVisibleTypeAnnotations", AttrType::RuntimeVisibleTypeAnnotations },
        { "RuntimeInvisibleTypeAnnotations", AttrType::RuntimeInvisibleTypeAnnotations },
        { "AnnotationDefault", AttrType::AnnotationDefault },
        { "MethodParameters", AttrType::MethodParameters },
        { "SourceFile", AttrType::SourceFile },
        { "SourceDebugExtension", AttrType::SourceDebugExtension },
        { "LineNumberTable", AttrType::LineNumberTable },
        { "LocalVariableTable", AttrType::LocalVariableTable },
        { "LocalVariableTypeTable", AttrType::LocalVariableTypeTable },
        { "Deprecated", AttrType::Deprecated }
    };

    std::string_view attrName(
        reinterpret_cast<const char*>(const_utf8->bytes.data()),
        const_utf8->bytes.size()
    );
    auto it = attrTypeTable.find(attrName);

    return (it != attrTypeTable.end()) 
        ? it->second
        : AttrType::Unknown;
}

ExceptionTableEntry::ExceptionTableEntry(u2 start, u2 end, u2 handler, u2 catchT)
    : startPc{ start },
    endPc{ end },
    handlerPc{ handler },
    catchType{ catchT }
{}

LineNumberTableEntry::LineNumberTableEntry(u2 start, u2 lineNum)
    : startPc{ start },
    lineNumber{ lineNum }
{}

void checkAttr(const AttrInfo& attr, const std::vector<std::unique_ptr<ConstPoolEntry>>& constPool)
{
    AttrType attrType{ resolveAttrType(constPool, attr.attrNameIndex) };
    switch (attrType)
    {
    case AttrType::ConstantValue:
        logFixme("IMPLEMENT ConstantValue format checker printer!!!");
        break;

    case AttrType::Code:
    {
        logFixme("IMPLEMENT Code attribute format checker logic!!!");
        break;
    }

    case AttrType::StackMapTable:
        logFixme("IMPLEMENT StackMapTable attribute format checker logic!!!");
        break;

    case AttrType::Exceptions:
        logFixme("IMPLEMENT Exceptions attribute format checker logic!!!");
        break;

    case AttrType::BootstrapMethods:
        logFixme("IMPLEMENT BootstrapMethods attribute format checker logic!!!");
        break;

    case AttrType::InnerClasses:
        logFixme("IMPLEMENT InnerClasses attribute format checker logic!!!");
        break;

    case AttrType::EnclosingMethod:
        logFixme("IMPLEMENT EnclosingMethod attribute format checker logic!!!");
        break;

    case AttrType::Synthetic:
        logFixme("IMPLEMENT Synthetic attribute format checker logic!!!");
        break;

    case AttrType::Signature:
        logFixme("IMPLEMENT Signature attribute format checker logic!!!");
        break;

    case AttrType::RuntimeVisibleAnnotations:
        logFixme("IMPLEMENT RuntimeVisibleAnnotations attribute format checker logic!!!");
        break;

    case AttrType::RuntimeInvisibleAnnotations:
        logFixme("IMPLEMENT RuntimeInvisibleAnnotations attribute format checker logic!!!");
        break;

    case AttrType::RuntimeVisibleParameterAnnotations:
        logFixme("IMPLEMENT RuntimeVisibleParameterAnnotations attribute format checker logic!!!");
        break;

    case AttrType::RuntimeInvisibleParameterAnnotations:
        logFixme("IMPLEMENT RuntimeInvisibleParameterAnnotations attribute format checker logic!!!");
        break;

    case AttrType::RuntimeVisibleTypeAnnotations:
        logFixme("IMPLEMENT RuntimeVisibleTypeAnnotations attribute format checker logic!!!");
        break;

    case AttrType::RuntimeInvisibleTypeAnnotations:
        logFixme("IMPLEMENT RuntimeInvisibleTypeAnnotations attribute format checker logic!!!");
        break;

    case AttrType::AnnotationDefault:
        logFixme("IMPLEMENT AnnotationDefault attribute format checker logic!!!");
        break;

    case AttrType::MethodParameters:
        logFixme("IMPLEMENT MethodParameters attribute format checker logic!!!");
        break;

    case AttrType::SourceFile:
    {
        logFixme("IMPLEMENT SourceFile attribute format checker logic!!!");
        break;
    }

    case AttrType::SourceDebugExtension:
        logFixme("IMPLEMENT SourceDebugExtension attribute format checker logic!!!");
        break;

    case AttrType::LineNumberTable:
    {
        logFixme("IMPLEMENT LineNumberTable attribute format checker logic!!!");
        break;
    }

    case AttrType::LocalVariableTable:
        logFixme("IMPLEMENT LocalVariableTable attribute format checker logic!!!");
        break;

    case AttrType::LocalVariableTypeTable:
        logFixme("IMPLEMENT LocalVariableTypeTable attribute format checker logic!!!");
        break;

    case AttrType::Deprecated:
        logFixme("IMPLEMENT Deprecated attribute format checker logic!!!");
        break;

    default:
        logError("Unknown `AttrType` enum value: %d", attrType);
    }
}
