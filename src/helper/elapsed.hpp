/*
 * elapsed.hpp
 * a timer that has second precision
 *
 * written by janus_wel<janus.wel.3@gmail.com>
 * This source code is in the public domain, and has NO WARRANTY.
 * */

#ifndef ELAPSED_HPP
#define ELAPSED_HPP

#include <ctime>

namespace util {
    namespace time {
        class elapsed {
            private:
                std::time_t basis;

            public:
                elapsed(void) {
                    set();
                }

                void set(void) {
                    std::time(&basis);
                }

                const double get(void) const {
                    std::time_t current;
                    std::time(&current);
                    return std::difftime(current, basis);
                }
        };
    }
}

#endif // ELAPSED_HPP

