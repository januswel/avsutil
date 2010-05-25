/*
 * usage.cpp
 *  Usage definitions for avslint.
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avslint.hpp"

#include <ostream>

void usage(std::ostream& out) {
    out
        << "Usage: " << name() << " <inputfile>\n"
        << "       " << name() << " options\n"
        << "\n"
        << "Options:\n"
        << "    -h, --help      Shows this help messages.\n"
        << "    -v, --version   Shows version and license informations.\n"
        << std::endl;
}

