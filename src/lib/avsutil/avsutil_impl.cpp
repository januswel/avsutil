/*
 * avsutil_impl.cpp
 *  Implementations for "avsutil.hpp"
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "../../include/avsutil.hpp"

#include "manager_impl.hpp"

namespace avsutil {
    const char* version(void) {
        return "1.00";
    }

    unsigned int version_num(void) {
        return 100;
    }

    const char* compile_date(void) {
        return __DATE__;
    }

    const char* compile_time(void) {
        return __TIME__;
    }

    // implementations for functions
    manager_type& manager(void) {
        static impl::cmanager_type manager;
        return manager;
    }
}

