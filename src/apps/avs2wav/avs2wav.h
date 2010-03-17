/*
 * avs2wav.h
 *  Declarations of global objects and functions for meta informations
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include <string>
#include <ostream>

// global object
extern const std::string version;

// typical one
void usage(std::ostream& out);
// the another typical
void version_license(std::ostream& out);

