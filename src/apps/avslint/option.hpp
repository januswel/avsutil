/*
 * option.hpp
 *  Declarations and definitions of option classes
 *
 *  written by janus_wel<janus.wel.3@gmail.com>
 *  This source code is in public domain, and has NO WARRANTY.
 * */

#ifndef OPTION_HPP
#define OPTION_HPP

#include "avslint.hpp"

#include "../../helper/getopt.hpp"
#include "../../helper/event.hpp"

// options
class opt_version_type
    : public util::getopt::option,
      public pattern::event::event_source<priority_type> {
    protected:
        const char_type* shortname(void) const { return "v"; }
        const char_type* longname(void) const { return "version"; }
        unsigned int handle_params(const parameters_type&) {
            dispatch_event(VERSION);
            return 1;
        }
};

class opt_help_type
    : public util::getopt::option,
      public pattern::event::event_source<priority_type> {
    protected:
        const char_type* shortname(void) const { return "h"; }
        const char_type* longname(void) const { return "help"; }
        unsigned int handle_params(const parameters_type&) {
            dispatch_event(HELP);
            return 1;
        }
};

#endif // OPTION_HPP

