#include "class_format.h"
#include "base.h"

void check_class_file(const ClassFile& cf)
{
}

void check_const_pool_index(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool,
        std::size_t index)
{
    if (index < 1 || const_pool.size() < index)
        log_fatal("Invalid constant pool index: %uz", index);
}

void check_const_pool_index_type_const_class(
        const std::vector<std::unique_ptr<ConstPoolEntry>>& const_pool,
        std::size_t index)
{
}

void check_this_class(const ClassFile& cf)
{
}
