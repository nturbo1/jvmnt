#include "classFile.h"

#include <string_view>
#include <cassert>
#include <cstddef>

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
    check_super_class();
    check_interfaces();
    check_fields();
    check_methods();
    for (std::size_t i{0}; i < m_attributes.size(); ++i)
    {
        const AttrInfo& attr{ *(m_attributes[i].get()) };
        check_const_pool_index_type_const_utf8(attr.attr_name_index);
        check_attr( attr, m_const_pool );
    }

    // TODO: VALIDATE that:
    //       - each value in the `interfaces` array must be a valid index into the
    //         `constant_pool` table.
    //       - the `constant_pool` entry at each value of `interfaces[i]`, where
    //         `0 ≤ i < interfaces_count`, must be a `CONSTANT_Class_info` structure
    //         representing an interface that is a direct superinterface of this
    //         class or interface type, in the left-to-right order given in the
    //         source for the type.
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
        log_fatal("Class filename %s and class name %s don't match.", m_filename.c_str(), const_class_name);
}

void ClassFile::check_super_class()
{
    bool is_interface{
        static_cast<bool>( m_access_flags & static_cast<const u2>(AccMasks::ACC_INTERFACE) )
    };

    if (m_super_class == 0)
    {
        if (is_interface)
            log_fatal("In class file %s: For an interface, the value of the `super_class` item"
                    " MUST always be a valid index into the `constant_pool` table.",
                    m_filename.c_str());

        // Assumes the `this_class` field of the `ClassFile` structure is already checked.
        ConstClassInfo* this_class_info {
            dynamic_cast<ConstClassInfo*>(m_const_pool.at(m_this_class - 1).get())
        };
        assert(this_class_info != nullptr &&
                "The `this_class` field of `ClassFile` structure must have been format "
                "checked already!");

        ConstUtf8Info* const_utf8_info {
            dynamic_cast<ConstUtf8Info*>(m_const_pool.at(this_class_info->name_index - 1).get())
        };
        assert(const_utf8_info != nullptr &&
                "The `this_class` field of `ClassFile` structure must have been format "
                "checked already!");

        std::string_view this_class_name(
                reinterpret_cast<const char*>(const_utf8_info->bytes.data()),
                const_utf8_info->bytes.size()
        );

        if (this_class_name != std::string_view("Object"))
            log_fatal(
                    "In class file %s: Only Object class can have NO direct superclass.",
                    m_filename.c_str()
            );
    }
    else
    {
        check_const_pool_index_type_const_class(m_super_class);
        ConstClassInfo* super_class_info {
            static_cast<ConstClassInfo*>(m_const_pool.at(m_super_class - 1).get())
        };
        check_const_pool_index_type_const_utf8(super_class_info->name_index);
        ConstUtf8Info* super_class_const_utf8_info {
            static_cast<ConstUtf8Info*>(m_const_pool.at(super_class_info->name_index - 1).get())
        };
        if (is_interface)
        {
            std::string_view super_class_name(
                    reinterpret_cast<const char*>(super_class_const_utf8_info->bytes.data()),
                    super_class_const_utf8_info->bytes.size()
            );

            if (super_class_name != std::string_view("Object"))
                log_fatal(
                    "In class file %s: For an interface, the `constant_pool` entry at index "
                    "`super_class` MUST be a `CONSTANT_Class_info` structure representing the "
                    "class `Object`",
                    m_filename.c_str()
                );
        }

        // TODO: Check that:
        //       neither the direct superclass nor any of its superclasses may have
        //       the `ACC_FINAL` flag set in the `access_flags` item of its `ClassFile`
        //       structure.
    }
}

void ClassFile::check_interfaces()
{
    for (u2 interface_idx : m_interfaces)
    {
        check_const_pool_index_type_const_class(interface_idx);
    }
}

void ClassFile::check_fields()
{
    for (const FieldInfo& field : m_fields)
    {
        check_const_pool_index_type_const_utf8(field.name_index);
        check_const_pool_index_type_const_utf8(field.descriptor_index);
        for (std::size_t i{0}; i < field.attributes.size(); ++i)
        {
            const AttrInfo& attr{ *(field.attributes[i].get()) };
            check_const_pool_index_type_const_utf8(attr.attr_name_index);
            check_attr( attr, m_const_pool );
        }
    }
}

void ClassFile::check_methods()
{
    for (const MethodInfo& method : m_methods)
    {
        check_const_pool_index_type_const_utf8(method.name_index);
        check_const_pool_index_type_const_utf8(method.descriptor_index);
        for (std::size_t i{0}; i < method.attributes.size(); ++i)
        {
            const AttrInfo& attr{ *(method.attributes[i].get()) };
            check_const_pool_index_type_const_utf8(attr.attr_name_index);
            check_attr( attr, m_const_pool );
        }
    }
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
