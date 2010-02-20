/*
 * Some functions for handling endian.
 * refer
 *  http://www.kijineko.co.jp/tech/cpptempls/endian
 * */

#ifndef ENDIAN_H
#define ENDIAN_H

#include <algorithm>
#include "cast.hpp"

namespace endian {
    inline bool is_le() {
        int t = 1;
        return *(pointer_cast<char*>(&t)) == 1;
    }
    inline bool is_be() { return !is_le(); }

    template<typename T> T reverse(T value) {
        char volatile* first = pointer_cast<char volatile*>(&value);
        char volatile* last = first + sizeof(T);
        std::reverse(first, last);
        return value;
    }

    template<typename T> T const* get_name();
    template<> char const* get_name<char>() {
        return is_le() ? "Little" : "Big";
    }
    template<> wchar_t const* get_name<wchar_t>() {
        return is_le() ? L"Little" : L"Big";
    }
}

#endif // ENDIAN_H
