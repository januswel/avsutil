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

#include "../../helper/event.hpp"
#include "../../helper/main.hpp"
#include "../../helper/typeconv.hpp"

enum priority_type {
    VERSION,
    HELP,
    UNSPECIFIED
};

class Main :
public util::main::main,
public pattern::event::event_listener<priority_type> {
    public:
        // return enumeration
        enum return_type {
            OK = 0,
            BAD_ARGUMENT,
            BAD_AVS,
            UNKNOWN
        };

    private:
        // utility
        static util::string::typeconverter& tconv(void) {
            static util::string::typeconverter tconv;
            return tconv;
        }

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

    private:
        // a kind of priority action
        // default: UNSPECIFIED
        priority_type priority;

    public:
        // event handler
        void handle_event(const priority_type& p) {
            if (priority == UNSPECIFIED) priority = p;
        }

    public:
        // constructor
        Main(void) : priority(UNSPECIFIED) {
            register_option(opt_version);
            register_option(opt_help);

            opt_version.add_event_listener(this);
            opt_help.add_event_listener(this);
        }

    public:
        // an implementation for the virtual function of the super class
        int start(void);

    private:
        // version definition
        const char* version(void);
        // typical one
        void usage(std::ostream& out);
        // the another typical
        void version_license(std::ostream& out);
};

#endif // AVSLINT_HPP

