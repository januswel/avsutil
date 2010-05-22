/*
 * avslint.hpp
 *  Declarations and definitions for a sub class of the class util::main::main
 *  and meta informations
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVSLINT_HPP
#define AVSLINT_HPP

#include <stdexcept>
#include <list>

#include "../../helper/event.hpp"
#include "../../helper/getopt.hpp"
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

// customized exception class
class avslint_error : public std::domain_error {
    private:
        return_type mv_return_value;

    public:
        avslint_error(const return_type return_value, const std::string& msg)
            : std::domain_error(msg), mv_return_value(return_value) {}
        return_type return_value(void) const { return mv_return_value; }
};

class Main :
public util::getopt::getopt,
public pattern::event::event_listener<priority_type> {
    private:
        string_type inputfile;
        std::list<string_type> unknown_opt;
        // a kind of priority action
        // default: UNSPECIFIED
        priority_type priority;

    protected:
        unsigned int handle_unknown_opt(const parameters_type& params) {
            // cash unknown options
            unknown_opt.push_back(*(params.current()));
            return 1;
        }
        unsigned int handle_behind_parameters(const parameters_type& params) {
            // only one input is allowed
            throw avslint_error(BAD_ARGUMENT,
                      "Don't specify anything behind the nonopt parameter: "
                    + *(params.current()) + "\n");
        }
        unsigned int handle_nonopt(const parameters_type& params) {
            inputfile = *(params.current());
            return 1;
        }

    public:
        // event handler
        void handle_event(const priority_type& p) {
            if (priority == UNSPECIFIED) priority = p;
        }

    public:
        // utility
        static util::string::typeconverter& tconv(void) {
            static util::string::typeconverter tconv;
            return tconv;
        }

        // version definition
        static const char* name(void) { return "avslint"; };
        static const char* version(void);
        // typical one
        static void usage(std::ostream&);
        // the another typical
        static void version_license(std::ostream&);

    private:
        // options
        class opt_version_type :
        public option_type,
        public pattern::event::event_source<priority_type> {
            protected:
                const char_type* shortname(void) const { return "v"; }
                const char_type* longname(void) const { return "version"; }
                unsigned int handle_params(const parameters_type&) {
                    dispatch_event(VERSION);
                    return 1;
                }
        } opt_version;

        class opt_help_type :
        public option_type,
        public pattern::event::event_source<priority_type> {
            protected:
                const char_type* shortname(void) const { return "h"; }
                const char_type* longname(void) const { return "help"; }
                unsigned int handle_params(const parameters_type&) {
                    dispatch_event(HELP);
                    return 1;
                }
        } opt_help;

    public:
        // constructor
        Main(void) : priority(UNSPECIFIED) {
            register_option(opt_version);
            register_option(opt_help);

            opt_version.add_event_listener(this);
            opt_help.add_event_listener(this);
        }

    private:
        void preparation(void) {
            // Treating unknown options
            if (!unknown_opt.empty()) {
                throw avslint_error(BAD_ARGUMENT,
                        "Unknown options: "
                        + tconv().join(
                            unknown_opt.begin(),
                            unknown_opt.end(), ", ") + "\n");
            }
        }

    public:
        int start(void);
};

#endif // AVSLINT_HPP

