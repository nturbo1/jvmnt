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

void checkAttr(const AttrInfo& attr)
{
    logFixme("IMPLEMENT an `AttrInfo` format checker!");
}
