
#pragma once

#include <vector>
#include <string>
#include "type_data.h"

namespace Core
{
    template<typename DSTTYPE, typename SRCTYPE>
    DSTTYPE transform(SRCTYPE src);



    std::vector<char> read_binary_file(const std::string &filename);

    template <class T>
    ClassHashCode get_class_hash_code() {
        return typeid(std::remove_reference_t<T>).hash_code();
    }
}
