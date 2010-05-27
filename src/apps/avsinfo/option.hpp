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
    OPT_READABLE,
    OPT_MACHINE,

    // events to specify items to show
    // packages
    OPT_ALL,
    OPT_VIDEO,
    OPT_AUDIO,

    // individuals
    // for video
    OPT_WIDTH,
    OPT_HEIGHT,
    OPT_RATIO,
    OPT_FPS,
    OPT_FPS_FRACTION,
    OPT_VIDEO_TIME,
    OPT_FRAMES,
    OPT_COLOR_SPACE,
    OPT_BPP,
    OPT_INTERLACE_TYPE,
    OPT_FIELD_ORDER,
    // for audio
    OPT_CHANNELS,
    OPT_BIT_DEPTH,
    OPT_SAMPLE_TYPE,
    OPT_AUDIO_TIME,
    OPT_SAMPLING_RATE,
    OPT_SAMPLES,
    OPT_BLOCK_SIZE
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

#define OPT_GENERAL_DECL(_name, _short, _event)                     \
class opt_##_name##_type                                            \
    : public util::getopt::option,                                  \
      public pattern::event::event_source<opt_event_type> {         \
    protected:                                                      \
        const char_type* shortname(void) const { return #_short; }  \
        const char_type* longname(void) const { return #_name; }    \
        unsigned int handle_params(const parameters_type&) {        \
            dispatch_event(_event);                                 \
            return 1;                                               \
        }                                                           \
}

// options to determine format
OPT_GENERAL_DECL(readable,  r,  OPT_READABLE);
OPT_GENERAL_DECL(machine,   m,  OPT_MACHINE);

// options to specify items to show
// packages
OPT_GENERAL_DECL(all,       a,  OPT_ALL);
OPT_GENERAL_DECL(video,     i,  OPT_VIDEO);
OPT_GENERAL_DECL(audio,     u,  OPT_AUDIO);

// individuals
#define OPT_INDIVIDUAL_DECL(_name, _event)                          \
class opt_##_name##_type                                            \
    : public util::getopt::option,                                  \
      public pattern::event::event_source<opt_event_type> {         \
    protected:                                                      \
        const char_type* longname(void) const { return #_name; }    \
        unsigned int handle_params(const parameters_type&) {        \
            dispatch_event(_event);                                 \
            return 1;                                               \
        }                                                           \
}

// for video
OPT_INDIVIDUAL_DECL(width,          OPT_WIDTH);
OPT_INDIVIDUAL_DECL(height,         OPT_HEIGHT);
OPT_INDIVIDUAL_DECL(ratio,          OPT_RATIO);
OPT_INDIVIDUAL_DECL(fps,            OPT_FPS);
OPT_INDIVIDUAL_DECL(fpsfraction,    OPT_FPS_FRACTION);
OPT_INDIVIDUAL_DECL(videotime,      OPT_VIDEO_TIME);
OPT_INDIVIDUAL_DECL(frames,         OPT_FRAMES);
OPT_INDIVIDUAL_DECL(colorspace,     OPT_COLOR_SPACE);
OPT_INDIVIDUAL_DECL(bpp,            OPT_BPP);
OPT_INDIVIDUAL_DECL(interlacetype,  OPT_INTERLACE_TYPE);
OPT_INDIVIDUAL_DECL(fieldorder,     OPT_FIELD_ORDER);

// for audio
OPT_INDIVIDUAL_DECL(channels,       OPT_CHANNELS);
OPT_INDIVIDUAL_DECL(bitdepth,       OPT_BIT_DEPTH);
OPT_INDIVIDUAL_DECL(sampletype,     OPT_SAMPLE_TYPE);
OPT_INDIVIDUAL_DECL(audiotime,      OPT_AUDIO_TIME);
OPT_INDIVIDUAL_DECL(samplingrate,   OPT_SAMPLING_RATE);
OPT_INDIVIDUAL_DECL(samples,        OPT_SAMPLES);
OPT_INDIVIDUAL_DECL(blocksize,      OPT_BLOCK_SIZE);

#endif // OPTION_HPP

