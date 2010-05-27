/*
 * main.hpp
 *  Declarations and definitions for basic flow of the program
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef MAIN_HPP
#define MAIN_HPP

#include "avsinfo.hpp"
#include "option.hpp"
#include "items.hpp"

#include <iostream>
#include <list>

#include "../../helper/getopt.hpp"
#include "../../helper/event.hpp"
#include "../../helper/typeconv.hpp"

class Main
    : public util::getopt::getopt,
      public pattern::event::event_listener<priority_type>,
      public pattern::event::event_listener<opt_event_type> {
    private:
        // objects to handle options
        opt_version_type    opt_version;
        opt_help_type       opt_help;
        opt_human_type      opt_human;
        opt_machine_type    opt_machine;
        opt_all_type        opt_all;
        opt_video_type      opt_video;
        opt_audio_type      opt_audio;

        // a kind of priority action
        // default: UNSPECIFIED
        priority_type priority;

        // member variables
        string_type inputfile;
        std::list<string_type> unknown_opt;
        bool is_human_friendly;
        avsinfo::items::VideoItems video_items;
        avsinfo::items::AudioItems audio_items;

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
            throw avsinfo_error(BAD_ARGUMENT,
                      "don't specify anything behind the nonopt parameter: "
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
        void handle_event(const opt_event_type& e) {
            using namespace avsinfo::items;
            switch (e) {
                case OPT_HUMAN:     is_human_friendly = true;   break;
                case OPT_MACHINE:   is_human_friendly = false;  break;
                case OPT_ALL:       add_all_video_items(video_items);
                                    add_all_audio_items(audio_items);
                                    break;
                case OPT_VIDEO:     add_all_video_items(video_items);
                                    break;
                case OPT_AUDIO:     add_all_audio_items(audio_items);
                                    break;
            }
        }

    public:
        // constructor
        Main(void)
        : priority(UNSPECIFIED), is_human_friendly(true) {
            // register options
            register_option(opt_version);
            register_option(opt_help);
            register_option(opt_human);
            register_option(opt_machine);
            register_option(opt_all);
            register_option(opt_video);
            register_option(opt_audio);

            // register event listeners
            opt_version.add_event_listener(this);
            opt_help.add_event_listener(this);
            opt_human.add_event_listener(this);
            opt_machine.add_event_listener(this);
            opt_all.add_event_listener(this);
            opt_video.add_event_listener(this);
            opt_audio.add_event_listener(this);
        }

        // option analysis and error handling
        void preparation(void) {
            if (!unknown_opt.empty()) {
                throw avsinfo_error(BAD_ARGUMENT,
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

