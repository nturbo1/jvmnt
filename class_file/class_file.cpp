#include "class_file.h"

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
}

void ClassFile::check_const_pool_index(std::size_t index)
{
    if (index < 1 || m_const_pool.size() < index)
        log_fatal("Invalid constant pool index: %zu", index);
}

void ClassFile::check_const_pool_index_type_const_class(std::size_t index)
{
}

void ClassFile::check_this_class()
{
}
