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

// argument macros
#define OPT_OBJ_DECL(_name) opt_##_name##_type  opt_##_name

#define OPT_VIDEO_ACTION(_name) \
case OPT_##_name:   video_items.add_item(video::##_name); break

#define OPT_AUDIO_ACTION(_name) \
case OPT_##_name:   audio_items.add_item(audio::##_name); break

#define REGISTER_OPT(_name)             \
register_option(opt_##_name);           \
opt_##_name##.add_event_listener(this)

class Main
    : public util::getopt::getopt,
      public pattern::event::event_listener<priority_type>,
      public pattern::event::event_listener<opt_event_type> {
    private:
        // objects to handle options
        OPT_OBJ_DECL(version);
        OPT_OBJ_DECL(help);
        OPT_OBJ_DECL(readable);
        OPT_OBJ_DECL(machine);
        OPT_OBJ_DECL(all);
        OPT_OBJ_DECL(video);
        OPT_OBJ_DECL(audio);
        OPT_OBJ_DECL(width);
        OPT_OBJ_DECL(height);
        OPT_OBJ_DECL(ratio);
        OPT_OBJ_DECL(fps);
        OPT_OBJ_DECL(fpsfraction);
        OPT_OBJ_DECL(videotime);
        OPT_OBJ_DECL(frames);
        OPT_OBJ_DECL(colorspace);
        OPT_OBJ_DECL(bpp);
        OPT_OBJ_DECL(interlacetype);
        OPT_OBJ_DECL(fieldorder);
        OPT_OBJ_DECL(channels);
        OPT_OBJ_DECL(bitdepth);
        OPT_OBJ_DECL(sampletype);
        OPT_OBJ_DECL(audiotime);
        OPT_OBJ_DECL(samplingrate);
        OPT_OBJ_DECL(samples);
        OPT_OBJ_DECL(blocksize);

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
                case OPT_READABLE:  is_human_friendly = true;   break;
                case OPT_MACHINE:   is_human_friendly = false;  break;
                case OPT_ALL:       add_all_video_items(video_items);
                                    add_all_audio_items(audio_items);
                                    break;
                case OPT_VIDEO:     add_all_video_items(video_items);
                                    break;
                case OPT_AUDIO:     add_all_audio_items(audio_items);
                                    break;

                // for video
                OPT_VIDEO_ACTION(WIDTH);
                OPT_VIDEO_ACTION(HEIGHT);
                OPT_VIDEO_ACTION(RATIO);
                OPT_VIDEO_ACTION(FPS);
                OPT_VIDEO_ACTION(FPS_FRACTION);
                OPT_VIDEO_ACTION(VIDEO_TIME);
                OPT_VIDEO_ACTION(FRAMES);
                OPT_VIDEO_ACTION(COLOR_SPACE);
                OPT_VIDEO_ACTION(BPP);
                OPT_VIDEO_ACTION(INTERLACE_TYPE);
                OPT_VIDEO_ACTION(FIELD_ORDER);

                // for audio
                OPT_AUDIO_ACTION(CHANNELS);
                OPT_AUDIO_ACTION(BIT_DEPTH);
                OPT_AUDIO_ACTION(SAMPLE_TYPE);
                OPT_AUDIO_ACTION(AUDIO_TIME);
                OPT_AUDIO_ACTION(SAMPLING_RATE);
                OPT_AUDIO_ACTION(SAMPLES);
                OPT_AUDIO_ACTION(BLOCK_SIZE);
            }
        }

    public:
        // constructor
        Main(void) : priority(UNSPECIFIED), is_human_friendly(true) {
            REGISTER_OPT(version);
            REGISTER_OPT(help);
            REGISTER_OPT(readable);
            REGISTER_OPT(machine);
            REGISTER_OPT(all);
            REGISTER_OPT(video);
            REGISTER_OPT(audio);
            REGISTER_OPT(width);
            REGISTER_OPT(height);
            REGISTER_OPT(ratio);
            REGISTER_OPT(fps);
            REGISTER_OPT(fpsfraction);
            REGISTER_OPT(videotime);
            REGISTER_OPT(frames);
            REGISTER_OPT(colorspace);
            REGISTER_OPT(bpp);
            REGISTER_OPT(interlacetype);
            REGISTER_OPT(fieldorder);
            REGISTER_OPT(channels);
            REGISTER_OPT(bitdepth);
            REGISTER_OPT(sampletype);
            REGISTER_OPT(audiotime);
            REGISTER_OPT(samplingrate);
            REGISTER_OPT(samples);
            REGISTER_OPT(blocksize);
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

