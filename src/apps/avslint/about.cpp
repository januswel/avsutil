/*
 * about.cpp
 *  A definition of function to give informations "about" the program
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avslint.hpp"

#include <ostream>

#include "../../include/avsutil.hpp"

void about(std::ostream& out) {
    out
        << name() << " version " << version() << '\n'
        << "Library:\n"
        << "    avsutil version " << avsutil::version()
        << " compiled at " << avsutil::compile_date()
        << " " << avsutil::compile_time() << "\n"
        << "\n"
        << "Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>\n"
        << std::endl;
}

