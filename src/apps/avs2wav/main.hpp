/*
 * main.hpp
 *  Declarations and definitions for basic flow of the program
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef MAIN_HPP
#define MAIN_HPP

#include "avs2wav.hpp"
#include "option.hpp"

#include <iostream>
#include <list>

#include "../../helper/getopt.hpp"
#include "../../helper/event.hpp"
#include "../../helper/typeconv.hpp"

class Main
    : public util::getopt::getopt,
      public pattern::event::event_listener<priority_type>,
      public pattern::event::event_listener<event_opt_uint>,
      public pattern::event::event_listener<event_opt_string> {
    private:
        // objects to handle options
        opt_version_type    opt_version;
        opt_help_type       opt_help;
        opt_buffers_type    opt_buffers;
        opt_output_type     opt_output;

        // a kind of priority action
        // default: UNSPECIFIED
        priority_type priority;

        // member variables
        string_type inputfile;
        string_type outputfile;
        unsigned int buf_size;
        std::list<string_type> unknown_opt;

        // constants
        static const unsigned int buf_size_def = 65536;

    protected:
        // implementations for virtual member functions of the super class
        // util::getopt::getopt
        unsigned int handle_unknown_opt(const parameters_type& params) {
            // cash unknown optoins
            unknown_opt.push_back(*(params.current()));
            return 1;
        }
        unsigned int handle_behind_parameters(const parameters_type& params) {
            // only one input is allowed
            throw avs2wav_error(BAD_ARGUMENT,
                      "Don't specify anything behind the nonopt parameter: "
                    + *(params.current()) + "\n");
        }
        unsigned int handle_nonopt(const parameters_type& params) {
            inputfile = *(params.current());
            return 1;
        }

    public:
        // event handlers
        void handle_event(const priority_type& p) {
            if (priority == UNSPECIFIED) priority = p;
        }
        void handle_event(const event_opt_uint& u) {
            switch (u.kind) {
                case OPT_BUFFERS:   buf_size = u.data;
                                    break;
                default:            throw std::logic_error("unknown error");
            }
        }
        void handle_event(const event_opt_string& s) {
            switch (s.kind) {
                case OPT_OUTPUT:    outputfile = s.data;
                                    break;
                default:            throw std::logic_error("unknown error");
            }
        }

    public:
        // constructor
        Main(void)
            : priority(UNSPECIFIED),
              buf_size(buf_size_def) {
            // register options
            register_option(opt_version);
            register_option(opt_help);
            register_option(opt_buffers);
            register_option(opt_output);

            // register event listeners
            opt_version.add_event_listener(this);
            opt_help.add_event_listener(this);
            opt_buffers.add_event_listener(this);
            opt_output.add_event_listener(this);
        }

        // option analysis and error handling
        void preparation(void) {
            if (!unknown_opt.empty()) {
                throw avs2wav_error(BAD_ARGUMENT,
                        "Unknown options: "
                        + tconv.join(
                            unknown_opt.begin(),
                            unknown_opt.end(), ", ") + "\n");
            }
        }

        // do it
        int start(void) {
            switch (priority) {
                case VERSION:       about(std::cout);
                                    return OK;
                case HELP:          usage(std::cout);
                                    return OK;
                case UNSPECIFIED:   return main();
                default:            throw std::logic_error("unknown error");
            }
        }

        int main(void);
};

#endif // MAIN_HPP

