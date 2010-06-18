/*
 * usage.cpp
 *  Usage definitions for avs2wav.
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avs2wav.hpp"

#include <ostream>

void usage(std::ostream& out) {
    out
        << "Usage: " << name() << " [options] <inputfile> [| othercommands]\n"
        << "\n"
        << "Options:\n"
        << "    -h, --help      Shows these help messages.\n"
        << "    -v, --version   Shows version and license informations.\n"
        << "\n"
        << "    -b N            Sets size of buffer for output to N.\n"
        << "                    min: 2, default: 4096.\n"
        << "    --buffers N     Same as \"-b\"\n"
        << "\n"
        << "    -o <file>       Sets a name of output file to <file>.  This is\n"
        << "                    ignored when redirected to file or conneted to\n"
        << "                    other command with pipe.\n"
        << "    --output <file> Same as \"-o\"\n"
        << std::endl;
}

