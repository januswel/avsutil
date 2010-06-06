/*
 * option.hpp
 *  Declarations and definitions of option classes
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef OPTION_HPP
#define OPTION_HPP

#include "avs2bmp.hpp"

#include <cassert>
#include <string>
#include <utility>

#include "../../helper/getopt.hpp"
#include "../../helper/event.hpp"
#include "../../helper/dlogger.hpp"

enum opt_event_kind {
    OPT_FRAME,
    OPT_DIGIT
};

typedef pattern::event::basic_event<opt_event_kind, unsigned int> event_opt_uint;
typedef std::pair<unsigned int, unsigned int> range_type;
typedef std::pair<double, double> timerange_type;

// option definitions
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

class opt_frame_type
    : public util::getopt::option,
      public pattern::event::event_source<event_opt_uint> {
    protected:
        const char_type* shortname(void) const { return "f"; }
        const char_type* longname(void) const { return "frame"; }
        unsigned int handle_params(const parameters_type& params) {
            parameters_type::const_iterator next = params.current() + 1;
            const string_type& current = *(params.current());

            if (next == params.end()) {
                throw avs2bmp_error(BAD_ARGUMENT,
                        "Specify target frame: " + current + "\n");
            }

            const unsigned int frame = tconv.strto<unsigned int>(*next);
            if (frame == 0) {
                throw avs2bmp_error(BAD_ARGUMENT,
                        "Target frame[s] should be a number beginning with"
                        " ONE: " + current + " " + *next + "\n");
            }

            event_opt_uint event = {OPT_FRAME, frame};
            dispatch_event(event);

            return 2;
        }
};

class opt_range_type
    : public util::getopt::option,
      public pattern::event::event_source<range_type> {
    protected:
        const char_type* shortname(void) const { return "r"; }
        const char_type* longname(void) const { return "range"; }
        unsigned int handle_params(const parameters_type& params) {
            parameters_type::const_iterator next01 = params.current() + 1;
            parameters_type::const_iterator next02 = next01 + 1;
            const string_type& current = *(params.current());

            if (next01 == params.end() || next02 == params.end()) {
                throw avs2bmp_error(BAD_ARGUMENT,
                        "Specify frames: " + current + "\n");
            }

            const unsigned int first = tconv.strto<unsigned int>(*next01);
            const unsigned int last = tconv.strto<unsigned int>(*next02);
            if (first == 0 || last == 0) {
                throw avs2bmp_error(BAD_ARGUMENT,
                        "Target frames should be a number beginning with"
                        " ONE: " + current + " " + *next01 + " " + *next02 +
                        "\n");
            }

            (first < last)
                ? dispatch_event(range_type(first, last))
                : dispatch_event(range_type(last, first));

            return 3;
        }
};

class opt_time_type
    : public util::getopt::option,
      public pattern::event::event_source<timerange_type> {
    protected:
        const char_type* shortname(void) const { return "t"; }
        const char_type* longname(void) const { return "time"; }
        unsigned int handle_params(const parameters_type& params) {
            parameters_type::const_iterator next = params.current() + 1;
            const string_type& current = *(params.current());

            if (next == params.end()) {
                throw avs2bmp_error(BAD_ARGUMENT,
                        "Specify target frame: " + current + "\n");
            }

            const double time = tconv.strto<double>(*next);
            dispatch_event(timerange_type(time, time));

            return 2;
        }
};

class opt_trange_type
    : public util::getopt::option,
      public pattern::event::event_source<timerange_type> {
    protected:
        const char_type* shortname(void) const { return "a"; }
        const char_type* longname(void) const { return "trange"; }
        unsigned int handle_params(const parameters_type& params) {
            parameters_type::const_iterator next01 = params.current() + 1;
            parameters_type::const_iterator next02 = next01 + 1;
            const string_type& current = *(params.current());

            if (next01 == params.end() || next02 == params.end()) {
                throw avs2bmp_error(BAD_ARGUMENT,
                        "Specify frames: " + current + "\n");
            }

            const double first = tconv.strto<double>(*next01);
            const double last = tconv.strto<double>(*next02);

            (first < last)
                ? dispatch_event(timerange_type(first, last))
                : dispatch_event(timerange_type(last, first));

            return 3;
        }
};

class opt_base_type
    : public util::getopt::option,
      public pattern::event::event_source<util::getopt::option::string_type> {
    protected:
        const char_type* shortname(void) const { return "b"; }
        const char_type* longname(void) const { return "base"; }
        unsigned int handle_params(const parameters_type& params) {
            parameters_type::const_iterator next = params.current() + 1;
            const string_type& current = *(params.current());

            if (next == params.end()) {
                throw avs2bmp_error(BAD_ARGUMENT,
                        "Specify <base>: " + current + "\n");
            }

            dispatch_event(*next);

            return 2;
        }
};

class opt_digit_type
    : public util::getopt::option,
      public pattern::event::event_source<event_opt_uint> {
    protected:
        const char_type* shortname(void) const { return "d"; }
        const char_type* longname(void) const { return "digit"; }
        unsigned int handle_params(const parameters_type& params) {
            parameters_type::const_iterator next = params.current() + 1;
            const string_type& current = *(params.current());

            if (next == params.end()) {
                throw avs2bmp_error(BAD_ARGUMENT,
                        "Specify N: " + current + "\n");
            }

            event_opt_uint event = {OPT_DIGIT, tconv.strto<unsigned int>(*next)};
            dispatch_event(event);

            return 2;
        }
};

#endif // OPTION_HPP

