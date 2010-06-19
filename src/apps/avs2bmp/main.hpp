/*
 * main.hpp
 *  Declarations and definitions for basic flow of the program
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef MAIN_HPP
#define MAIN_HPP

#include "avs2bmp.hpp"
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
      public pattern::event::event_listener<range_type>,
      public pattern::event::event_listener<timerange_type>,
      public pattern::event::event_listener<util::getopt::option::string_type> {
    public:
        typedef std::list<unsigned int>     target_frames_type;
        typedef std::list<timerange_type>   timerange_array_type;

    private:
        // objects to handle options
        opt_version_type    opt_version;
        opt_help_type       opt_help;
        opt_frame_type      opt_frame;
        opt_range_type      opt_range;
        opt_time_type       opt_time;
        opt_trange_type     opt_trange;
        opt_base_type       opt_base;
        opt_digit_type      opt_digit;

        // a kind of priority action
        // default: UNSPECIFIED
        priority_type priority;

        // member variables
        string_type inputfile;
        std::list<string_type> unknown_opt;
        std::list<unsigned int> target_frames;
        timerange_array_type timerange_array;
        string_type base;
        unsigned int digit;

        // constants
        static const unsigned int digit_default = 6;

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
            throw avs2bmp_error(BAD_ARGUMENT,
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
        void handle_event(const range_type& r) {
            for (unsigned int i = r.first; i <= r.second; ++i) {
                target_frames.push_back(i);
            }
        }
        void handle_event(const util::getopt::option::string_type& s) {
            base = s;
        }
        void handle_event(const event_opt_uint& e) {
            switch (e.kind) {
                case OPT_FRAME: target_frames.push_back(e.data); break;
                case OPT_DIGIT: digit = e.data; break;
            }
        }
        void handle_event(const timerange_type& t) {
            timerange_array.push_back(t);
        }

    public:
        // constructor
        Main(void) : priority(UNSPECIFIED), digit(digit_default) {
            // register options
            register_option(opt_version);
            register_option(opt_help);
            register_option(opt_frame);
            register_option(opt_range);
            register_option(opt_time);
            register_option(opt_trange);
            register_option(opt_base);
            register_option(opt_digit);

            // register event listeners
            opt_version.add_event_listener(this);
            opt_help.add_event_listener(this);
            opt_frame.add_event_listener(this);
            opt_range.add_event_listener(this);
            opt_time.add_event_listener(this);
            opt_trange.add_event_listener(this);
            opt_base.add_event_listener(this);
            opt_digit.add_event_listener(this);
        }

        // option analysis and error handling
        void preparation(void) {
            if (!unknown_opt.empty()) {
                throw avs2bmp_error(BAD_ARGUMENT,
                        "unknown options: "
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

