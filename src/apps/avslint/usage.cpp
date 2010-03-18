/*
 * usage.cpp
 *  Usage definitions for avslint.
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avslint.hpp"
#include <iostream>

void usage(std::ostream& out) {
    out << "Usage: avslint <inputfile>\n"
        << "       avslint options\n"
        << "Options:\n"
        << "    -h, --help      Shows this help\n"
        << "    -v, --version   Shows version and license informations.\n"
        << std::endl;
}

