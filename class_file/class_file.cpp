#include "class_file.h"

#include <string_view>
#include <cassert>

FieldInfo::FieldInfo(
        u2 access_fgs,
        u2 name_idx,
        u2 descriptor_idx,
        std::vector<std::unique_ptr<AttrInfo>> attrs)
    : access_flags{ access_fgs },
    name_index{ name_idx },
    descriptor_index{ descriptor_idx },
    attributes{ std::move(attrs) }
{}

MethodInfo::MethodInfo(
        u2 access_fgs,
        u2 name_idx,
        u2 descriptor_idx,
        std::vector<std::unique_ptr<AttrInfo>> attrs)
    : access_flags{ access_fgs },
    name_index{ name_idx },
    descriptor_index{ descriptor_idx },
    attributes{ std::move(attrs) }
{}

void ClassFile::format_check()
{
    // The `magic` number is validated during the class file parsing.
    // If not, validate it here.

    // TODO: Validate the version support!      About `m_minor_version` and `m_major_version`

    check_this_class();

    // TODO: VALIDATE that:
    //       - for a class, the value of the `super_class` item either MUST be zero or
    //         MUST be a valid index into the `constant_pool` table.
    //
    //       - if the value of the `super_class` item is nonzero, the `constant_pool`
    //         entry at that index MUST be a `CONSTANT_Class_info` structure representing
    //         the direct superclass of the class defined by this class file. Neither the
    //         direct superclass nor any of its superclasses may have the `ACC_FINAL` flag
    //         set in the `access_flags` item of its `ClassFile` structure.
    //
    //       - if the value of the `super_class` item is zero, then this class file MUST represent
    //         the class `Object`, the only class or interface without a direct superclass.
    //
    //       - for an interface, the value of the `super_class` item MUST always be a valid
    //         index into the `constant_pool` table. The `constant_pool` entry at that index
    //         MUST be a `CONSTANT_Class_info` structure representing the class `Object`.

    // TODO: VALIDATE that:
    //       - each value in the `interfaces` array must be a valid index into the
    //         `constant_pool` table.
    //       - the `constant_pool` entry at each value of `interfaces[i]`, where
    //         `0 ≤ i < interfaces_count`, must be a `CONSTANT_Class_info` structure
    //         representing an interface that is a direct superinterface of this
    //         class or interface type, in the left-to-right order given in the
    //         source for the type.
}

void ClassFile::check_const_pool_index(std::size_t index)
{
    if (index < 1 || m_const_pool.size() < index)
        log_fatal("In class file %s: Invalid constant pool index: %zu",
                m_filename.c_str(),
                index);
}

void ClassFile::check_const_pool_index_type_const_class(std::size_t index)
{
    check_const_pool_index(index);

    if (!dynamic_cast<ConstClassInfo*>(m_const_pool.at(index - 1).get()))
        log_fatal("In class file %s: Expected the constant pool entry at index %d"
                "to be of type CONSTANT_Class",
                m_filename.c_str(),
                index);
}

void ClassFile::check_const_pool_index_type_const_utf8(std::size_t index)
{
    check_const_pool_index(index);

    if (!dynamic_cast<ConstUtf8Info*>(m_const_pool.at(index - 1).get()))
        log_fatal("In class file %s: Expected the constant pool entry at index %d"
                "to be of type CONSTANT_Utf8",
                m_filename.c_str(),
                index);
}

void ClassFile::check_this_class()
{
    check_const_pool_index_type_const_class(m_this_class);
    std::string_view class_name(m_filename.begin(), m_filename.end() - 6);
    ConstClassInfo* const_class{ dynamic_cast<ConstClassInfo*>(m_const_pool.at(m_this_class - 1).get()) };
    assert(const_class != nullptr &&
            "The constant pool entry was not type checked for `ConstClassInfo` struct.");

    u2 class_name_index{ const_class->name_index };
    check_const_pool_index_type_const_utf8(class_name_index);
    ConstUtf8Info* const_utf8{ dynamic_cast<ConstUtf8Info*>(m_const_pool.at(class_name_index - 1).get()) };
    assert(const_utf8 != nullptr &&
            "The constant pool entry was not type checked for `ConstUtf8Info` struct.");

    std::string_view const_class_name(
            reinterpret_cast<const char*>(const_utf8->bytes.data()),
            const_utf8->bytes.size()
    );

    if (const_class_name != class_name)
        log_fatal("Class filename %s and class name %s don't match.", m_filename, const_class_name);
}
