#include "attributes.h"

#include <cassert>
#include <unordered_map>
#include <string_view>

AttrInfo::AttrInfo(u2 attr_name_idx)
    : attr_name_index{ attr_name_idx }
{}

CodeAttrInfo::CodeAttrInfo(
            u2 attr_name_idx,
            u2 max_stk,
            u2 max_lcls,
            std::vector<u1> c,
            std::vector<ExceptionTableEntry> exc_table,
            std::vector<std::unique_ptr<AttrInfo>> attrs)
    : AttrInfo(attr_name_idx),
    max_stack{ max_stk },
    max_locals{ max_lcls },
    code{ c },
    exception_table{ exc_table },
    attributes{ std::move(attrs) }
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

    // How can I treat `(const_pool[attr_name_index - 1])` as of type `std::unique_ptr<ConstUtf8Info>`???
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
        { "BootstrapMethods", AttrType::BootstrapMethods }
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
