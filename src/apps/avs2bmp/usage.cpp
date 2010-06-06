/*
 * usage.cpp
 *  A definition of function to give usage of the program
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avs2bmp.hpp"

#include <ostream>

void usage(std::ostream& out) {
    out
        << "Usage: " << name() << " -f|--frame N [options] <inputfile>\n"
        << "\n"
        << "Options:\n"
        << "    -h, --help      Shows these help messages.\n"
        << "    -v, --version   Shows version and license informations.\n"
        << "\n"
        << "    -f N            Writes Nth frame.\n"
        << "    --frame N       Same as \"-f N\".\n"
        << "    -r M N          Writes Mth to Nth frames.\n"
        << "    --range M N     Same as \"-r M N\".\n"
        << "    -t T            Writes a nearest frame from T. T may be\n"
        << "                    floating-point number\n"
        << "    --time T        Same as \"-t T\".\n"
        << "    -a T1 T2        Writes frames that exist from T1 to T2.\n"
        << "                    T1 and T2 may be floating-point numbers.\n"
        << "    --trange T1 T2  Same as \"-a T1 T2\".\n"
        << "\n"
        << "    -b <base>       Sets a base name of output files to <base>.\n"
        << "                    Default is <inputfile>.\n"
        << "    --base <base>   Same as \"-b <base>\".\n"
        << "    -d N            Sets a digits of frame number that is\n"
        << "                    concatenated to a base name specified by the\n"
        << "                    option \"-b <base>\" or \"--base <base>\".\n"
        << "    --digit N       Same as \"-d N\".\n"
        << std::endl;
}

