/*
 * avsutil_impl.cpp
 *  Implementations for "avsutil.hpp"
 *
 *  written by janus_wel<janus.wel.3@gmail.com>
 *  This source code is in public domain, and has NO WARRANTY.
 * */

#include "../../include/avsutil.hpp"

#include "manager_impl.hpp"

namespace avsutil {
    // implementations for functions
    manager_type& manager(void) {
        static impl::cmanager_type manager;
        return manager;
    }
}

