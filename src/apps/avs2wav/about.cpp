/*
 * about.cpp
 *  A definition of function to give informations "about" the program
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avs2wav.hpp"

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
        << '\n'
        << "This program is free software: you can redistribute it and/or modify\n"
        << "it under the terms of the GNU General Public License as published by\n"
        << "the Free Software Foundation, either version 3 of the License, or\n"
        << "(at your option) any later version.\n"
        << '\n'
        << "This program is distributed in the hope that it will be useful,\n"
        << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        << "GNU General Public License for more details.\n"
        << '\n'
        << "You should have received a copy of the GNU General Public License\n"
        << "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
        << std::endl;
}

