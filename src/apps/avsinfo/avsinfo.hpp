/*
 * avsinfo.hpp
 *  Declarations of global objects and functions for meta informations
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVSINFO_HPP
#define AVSINFO_HPP

#include <stdexcept>

#include "../../helper/typeconv.hpp"

// enumerations for return expression
enum return_type {
    OK = 0,
    BAD_ARGUMENT,
    BAD_AVS,
    UNKNOWN
};

enum priority_type {
    VERSION,
    HELP,
    UNSPECIFIED
};

// global objects
extern util::string::typeconverter tconv;

// functions to give meta informations
const char* name(void);
const char* version(void);
void usage(std::ostream& out);
void about(std::ostream& out);

// customized exception class
class avsinfo_error : public std::domain_error {
    private:
        return_type mv_return_value;

    public:
        avsinfo_error(const return_type return_value, const std::string& msg)
            : std::domain_error(msg), mv_return_value(return_value) {}
        return_type return_value(void) const { return mv_return_value; }
};

#endif // AVSINFO_HPP

