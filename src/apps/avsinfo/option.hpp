/*
 * option.hpp
 *  Declarations and definitions of option classes
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef OPTION_HPP
#define OPTION_HPP

#include "avsinfo.hpp"

#include "../../helper/getopt.hpp"
#include "../../helper/event.hpp"

enum opt_event_type {
    // events to inform a format
    OPT_HUMAN,
    OPT_MACHINE,

    // events to specify items to show
    // package
    OPT_ALL,
    OPT_VIDEO,
    OPT_AUDIO,
};

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

// options to determine notation
class opt_human_type
    : public util::getopt::option,
      public pattern::event::event_source<opt_event_type> {
    protected:
        const char_type* shortname(void) const { return "r"; }
        const char_type* longname(void) const { return "readable"; }
        unsigned int handle_params(const parameters_type&) {
            dispatch_event(OPT_HUMAN);
            return 1;
        }
};

class opt_machine_type
    : public util::getopt::option,
      public pattern::event::event_source<opt_event_type> {
    protected:
        const char_type* shortname(void) const { return "m"; }
        const char_type* longname(void) const { return "machine"; }
        unsigned int handle_params(const parameters_type&) {
            dispatch_event(OPT_MACHINE);
            return 1;
        }
};

// options to specify items to show
class opt_all_type
    : public util::getopt::option,
      public pattern::event::event_source<opt_event_type> {
    protected:
        const char_type* shortname(void) const { return "a"; }
        const char_type* longname(void) const { return "all"; }
        unsigned int handle_params(const parameters_type&) {
            dispatch_event(OPT_ALL);
            return 1;
        }
};

class opt_video_type
    : public util::getopt::option,
      public pattern::event::event_source<opt_event_type> {
    protected:
        const char_type* shortname(void) const { return "i"; }
        const char_type* longname(void) const { return "video"; }
        unsigned int handle_params(const parameters_type&) {
            dispatch_event(OPT_VIDEO);
            return 1;
        }
};

class opt_audio_type
    : public util::getopt::option,
      public pattern::event::event_source<opt_event_type> {
    protected:
        const char_type* shortname(void) const { return "u"; }
        const char_type* longname(void) const { return "audio"; }
        unsigned int handle_params(const parameters_type&) {
            dispatch_event(OPT_AUDIO);
            return 1;
        }
};

#endif // OPTION_HPP

