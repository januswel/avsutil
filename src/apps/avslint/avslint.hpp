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

#include "../../helper/main.hpp"
#include "../../helper/typeconv.hpp"

class Main : public util::main::main {
    private:
        // return enumeration
        enum {OK = 0, BAD_ARG, BAD_AVS, UNKNOWN};

    private:
        // utility
        static util::string::typeconverter& tconv(void) {
            static util::string::typeconverter tconv;
            return tconv;
        }

    private:
        // options
        class opt_version_type : public option_type {
            private:
                bool state;

            public:
                opt_version_type(void) : state(false) {}

            public:
                bool operator()(void) { return state; }

            protected:
                const char_type* shortname(void) const { return "v"; }
                const char_type* longname(void) const { return "version"; }
                unsigned int handle_params(const parameters_type&) {
                    state = true;
                    return 1;
                }
        } opt_version;

        class opt_help_type : public option_type {
            private:
                bool state;

            public:
                opt_help_type(void) : state(false) {}

            public:
                bool operator()(void) { return state; }

            protected:
                const char_type* shortname(void) const { return "h"; }
                const char_type* longname(void) const { return "help"; }
                unsigned int handle_params(const parameters_type&) {
                    state = true;
                    return 1;
                }
        } opt_help;

    public:
        // constructor
        Main(void) {
            register_option(opt_version);
            register_option(opt_help);
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

