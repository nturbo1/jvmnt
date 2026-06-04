#include "attributes.h"
#include "log.h"

#include <cassert>
#include <unordered_map>
#include <string_view>
#include <cstddef>

AttrInfo::AttrInfo(u2 attr_name_idx)
    : attr_name_index{ attr_name_idx }
{}

CodeAttrInfo::CodeAttrInfo(u2 attr_name_idx)
    : AttrInfo(attr_name_idx)
{}

LineNumberTableAttrInfo::LineNumberTableAttrInfo(u2 attr_name_idx)
    : AttrInfo(attr_name_idx)
{}

SourceFileAttrInfo::SourceFileAttrInfo(u2 attr_name_idx)
    : AttrInfo(attr_name_idx)
{}

AttrType resolve_attr_type(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool,
        u2 attr_name_index)
{
    // The class file format uses `1-based` indexes.
    // However, the parsed `const_pool` vector stores the parsed constant pool items
    // in usual `0-based` indexes internally, so
    //     class file `index` == parsed `const_pool` vector `index - 1`.
    ConstPoolEntryTag entry_tag{ (const_pool[attr_name_index - 1])->tag };

    if (entry_tag != ConstPoolEntryTag::CONSTANT_Utf8)
        log_fatal("The `constant_pool` entry at `attribute_name_index` MUST be a "
                "`CONSTANT_Utf8_info` structure representing the name of the attribute.");

    ConstUtf8Info* const_utf8{ dynamic_cast<ConstUtf8Info*>(const_pool[attr_name_index - 1].get()) };
    assert(const_utf8 &&
            "The constant pool entry was not validated to be of type `CONSTANT_Utf8_info` or "
            "the constant pool entry tag does not match the entry info bytes, which means "
            "the constant pool entry was NOT parsed correctly or the constant pool entry was "
            "of invalid format and it was not caught on time!"); 

    static const std::unordered_map<std::string_view, AttrType> attr_type_table = {
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

    std::string_view attr_name(
        reinterpret_cast<const char*>(const_utf8->bytes.data()),
        const_utf8->bytes.size()
    );
    auto it = attr_type_table.find(attr_name);

    return (it != attr_type_table.end())
        ? it->second
        : AttrType::Unknown;
}

ExceptionTableEntry::ExceptionTableEntry(u2 start, u2 end, u2 handler, u2 catch_t)
    : start_pc{ start },
    end_pc{ end },
    handler_pc{ handler },
    catch_type{ catch_t }
{}

LineNumberTableEntry::LineNumberTableEntry(u2 start, u2 line_num)
    : start_pc{ start },
    line_number{ line_num }
{}

static void check_exception_table_entry(const ExceptionTableEntry& ete)
{
    log_fixme("IMPLEMENT ExceptionTableEntry format checker!");
}

static void check_code_attr(const CodeAttrInfo& code_attr)
{
    std::size_t code_size{ code_attr.code.size() };
    if (code_size == 0)
        log_fatal("");
}

void check_attr(const AttrInfo& attr, const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool)
{
    AttrType attr_type{ resolve_attr_type(const_pool, attr.attr_name_index) };
    switch (attr_type)
    {
    case AttrType::ConstantValue:
        log_fixme("IMPLEMENT ConstantValue format checker printer!!!");
        break;

    case AttrType::Code:
    {
        log_fixme("IMPLEMENT Code attribute format checker logic!!!");
        break;
    }

    case AttrType::StackMapTable:
        log_fixme("IMPLEMENT StackMapTable attribute format checker logic!!!");
        break;

    case AttrType::Exceptions:
        log_fixme("IMPLEMENT Exceptions attribute format checker logic!!!");
        break;

    case AttrType::BootstrapMethods:
        log_fixme("IMPLEMENT BootstrapMethods attribute format checker logic!!!");
        break;

    case AttrType::InnerClasses:
        log_fixme("IMPLEMENT InnerClasses attribute format checker logic!!!");
        break;

    case AttrType::EnclosingMethod:
        log_fixme("IMPLEMENT EnclosingMethod attribute format checker logic!!!");
        break;

    case AttrType::Synthetic:
        log_fixme("IMPLEMENT Synthetic attribute format checker logic!!!");
        break;

    case AttrType::Signature:
        log_fixme("IMPLEMENT Signature attribute format checker logic!!!");
        break;

    case AttrType::RuntimeVisibleAnnotations:
        log_fixme("IMPLEMENT RuntimeVisibleAnnotations attribute format checker logic!!!");
        break;

    case AttrType::RuntimeInvisibleAnnotations:
        log_fixme("IMPLEMENT RuntimeInvisibleAnnotations attribute format checker logic!!!");
        break;

    case AttrType::RuntimeVisibleParameterAnnotations:
        log_fixme("IMPLEMENT RuntimeVisibleParameterAnnotations attribute format checker logic!!!");
        break;

    case AttrType::RuntimeInvisibleParameterAnnotations:
        log_fixme("IMPLEMENT RuntimeInvisibleParameterAnnotations attribute format checker logic!!!");
        break;

    case AttrType::RuntimeVisibleTypeAnnotations:
        log_fixme("IMPLEMENT RuntimeVisibleTypeAnnotations attribute format checker logic!!!");
        break;

    case AttrType::RuntimeInvisibleTypeAnnotations:
        log_fixme("IMPLEMENT RuntimeInvisibleTypeAnnotations attribute format checker logic!!!");
        break;

    case AttrType::AnnotationDefault:
        log_fixme("IMPLEMENT AnnotationDefault attribute format checker logic!!!");
        break;

    case AttrType::MethodParameters:
        log_fixme("IMPLEMENT MethodParameters attribute format checker logic!!!");
        break;

    case AttrType::SourceFile:
    {
        log_fixme("IMPLEMENT SourceFile attribute format checker logic!!!");
        break;
    }

    case AttrType::SourceDebugExtension:
        log_fixme("IMPLEMENT SourceDebugExtension attribute format checker logic!!!");
        break;

    case AttrType::LineNumberTable:
    {
        log_fixme("IMPLEMENT LineNumberTable attribute format checker logic!!!");
        break;
    }

    case AttrType::LocalVariableTable:
        log_fixme("IMPLEMENT LocalVariableTable attribute format checker logic!!!");
        break;

    case AttrType::LocalVariableTypeTable:
        log_fixme("IMPLEMENT LocalVariableTypeTable attribute format checker logic!!!");
        break;

    case AttrType::Deprecated:
        log_fixme("IMPLEMENT Deprecated attribute format checker logic!!!");
        break;

    default:
        log_error("Unknown `AttrType` enum value: %d", attr_type);
    }
}
