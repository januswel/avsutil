/*
 * Some functions for handling endian.
 *
 * written by janus_wel<janus.wel.3@gmail.com>
 * This source code is NOT MY COPYRIGHTED WORK, and has NO WARRANTY.
 *
 * Refer - a source of an idea:
 *  http://www.kijineko.co.jp/tech/cpptempls/endian
 * */

#ifndef ENDIAN_H
#define ENDIAN_H

#include <algorithm>
#include "cast.hpp"

namespace util {
    namespace endian {
        inline bool is_le() {
            int t = 1;
            return *(util::cast::pointer_cast<char*>(&t)) == 1;
        }
        inline bool is_be() { return !is_le(); }

        template<typename T> T reverse(T value) {
            char volatile* first = util::cast::pointer_cast<char volatile*>(&value);
            char volatile* last = first + sizeof(T);
            std::reverse(first, last);
            return value;
        }
    }
}

#endif // ENDIAN_H

