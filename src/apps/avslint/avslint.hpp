/*
 * avslint.hpp
 *  Declarations of global objects and functions for meta informations
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVSLINT_HPP
#define AVSLINT_HPP

#include <string>

// global object
// version number
extern const std::string version;
// return enumeration
enum {OK = 0, BAD_ARG, BAD_AVS, UNKNOWN};

// typical one
void usage(std::ostream& out);
// the another typical
void version_license(std::ostream& out);

#endif // AVSLINT_HPP

