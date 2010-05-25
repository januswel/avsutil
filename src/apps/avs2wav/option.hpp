/*
 * option.hpp
 *  Declarations and definitions of option classes
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef OPTION_HPP
#define OPTION_HPP

#include "avs2wav.hpp"

#include "../../helper/getopt.hpp"
#include "../../helper/event.hpp"

enum opt_event_kind {
    OPT_BUFFERS,
    OPT_SAMPLES,
    OPT_OUTPUT
};

typedef pattern::event::basic_event<opt_event_kind, unsigned int>   event_opt_uint;
typedef pattern::event::basic_event<opt_event_kind, util::getopt::option::string_type>  event_opt_string;

// option definitions
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

class opt_buffers_type
    : public util::getopt::option,
      public pattern::event::event_source<event_opt_uint> {
    private:
        static const unsigned int buf_size_min = 2;

    protected:
        const char_type* shortname(void) const { return "b"; }
        const char_type* longname(void) const { return "buffers"; }
        unsigned int handle_params(const parameters_type& params) {
            parameters_type::const_iterator next = params.current() + 1;

            if (next == params.end()) {
                throw avs2wav_error(BAD_ARGUMENT,
                        "Specify size of buffers: "
                        + *(params.current()) + "\n");
            }

            unsigned int buf_size = tconv.strto<unsigned int>(*next);

            if (buf_size < buf_size_min) {
                throw avs2wav_error(BAD_ARGUMENT,
                        "Size of buffers for output must be "
                        + tconv.strfrom(buf_size_min) + " or bigger.\n"
                        + "Check the argument of \"" + *(params.current())
                        + "\" option.\n");
            }

            event_opt_uint event = {OPT_BUFFERS, buf_size};
            dispatch_event(event);
            return 2;
        }
};

class opt_samples_type
    : public util::getopt::option,
      public pattern::event::event_source<event_opt_uint> {
    private:
        static const unsigned int buf_samples_min = 1;

    protected:
        const char_type* shortname(void) const { return "s"; }
        const char_type* longname(void) const { return "samples"; }
        unsigned int handle_params(const parameters_type& params) {
            parameters_type::const_iterator next = params.current() + 1;

            if (next == params.end()) {
                throw avs2wav_error(BAD_ARGUMENT,
                        "Specify a number of samples processed at one time: "
                        + *(params.current()) + "\n");
            }

            unsigned int buf_samples = tconv.strto<unsigned int>(*next);

            if (buf_samples < buf_samples_min) {
                throw avs2wav_error(BAD_ARGUMENT,
                        "A number of samples processed at one time must be "
                        + tconv.strfrom(buf_samples_min) + " or bigger.\n"
                        + "Check the argument of \"" + *(params.current())
                        + "\" option.\n");
            }

            event_opt_uint event = {OPT_SAMPLES, buf_samples};
            dispatch_event(event);
            return 2;
        }
};

class opt_output_type
    : public util::getopt::option,
      public pattern::event::event_source<event_opt_string> {
    protected:
        const char_type* shortname(void) const { return "o"; }
        const char_type* longname(void) const { return "output"; }
        unsigned int handle_params(const parameters_type& params) {
            parameters_type::const_iterator next = params.current() + 1;

            if (next == params.end()) {
                throw avs2wav_error(BAD_ARGUMENT,
                        "Specify a name of output file: "
                        + *(params.current()) + "\n");
            }

            event_opt_string event = {OPT_OUTPUT, *next};
            dispatch_event(event);
            return 2;
        }
};

#endif // OPTION_HPP

