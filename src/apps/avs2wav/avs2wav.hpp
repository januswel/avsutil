/*
 * avs2wav.hpp
 *  Declarations of global objects and functions for meta informations
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVS2WAV_HPP
#define AVS2WAV_HPP

#include <string>
#include <ostream>

// global object
// version number
extern const std::string version;
// return enumeration
enum {OK = 0, BAD_ARG, BAD_AVS, UNKNOWN};

// typical one
void usage(std::ostream& out);
// the another typical
void version_license(std::ostream& out);

#endif // AVS2WAV_HPP

