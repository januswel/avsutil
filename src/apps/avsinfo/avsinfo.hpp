/*
 * avsinfo.hpp
 *  Declarations of global objects and functions for meta informations
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVSINFO_HPP
#define AVSINFO_HPP

#include <string>

namespace avsinfo {
    namespace general {
        // global object
        // version number
        extern const std::string version;
        // enumerations for return statements
        enum {OK = 0, BAD_ARG, BAD_AVS, UNKNOWN};

        // typical one
        void usage(std::ostream& out);
        // the another typical
        void version_license(std::ostream& out);
    }
}

#endif // AVSINFO_HPP

