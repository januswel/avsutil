/*
 * avsinfo main.cpp
 *  Shows informations of AviSynth script
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "../../include/avsutil.hpp"
#include "avsinfo.hpp"
#include "items.hpp"
#include "../../helper/tconv.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <locale>
#include <memory>   // for std::auto_ptr
#include <bitset>

// using namespaces
using namespace std;
using namespace avsutil;
using namespace avsinfo::general;
using namespace avsinfo::items;

// forward declarations
template <typename T> void print_video_items(const video::item_t item, const T value);
template <typename T> void print_audio_items(const audio::item_t item, const T value);
const string readable_channles(const unsigned int channels);
const unsigned int calc_gcd(unsigned int a, unsigned int b);
const string width_to_height_ratio(const unsigned int width, const unsigned int height);

// constants
const unsigned int header_max = 20;
const char* header_delimeter = ": ";

// global objects
util::string::converter conv;
bool machine_flag = false;

int main(const int argc, const char* const argv[]) {
    // set global locale to use non-ascii characters for filenames
    locale::global(locale(""));

    string inputfile;
    bool no_options_flag = true;
    bitset<video::num> video_flags;
    bitset<audio::num> audio_flags;
    for (int i = 1; i < argc; ++i) {
        const string arg(argv[i]);
        if (arg == "-h" || arg == "--help") {
            usage(cout);
            return OK;
        }
        else if (arg == "-v" || arg == "--version") {
            version_license(cout);
            return OK;
        }
        else if (arg == "-m" || arg == "--machine") {
            machine_flag = true;
            continue;
        }
        else if (arg == "-a" || arg == "--all") {
            video_flags.set();
            audio_flags.set();
            no_options_flag = false;
            continue;
        }
        else if (arg == "-i" || arg == "--video") {
            video_flags.set();
            no_options_flag = false;
            continue;
        }
        else if (arg == "-u" || arg == "--audio") {
            audio_flags.set();
            no_options_flag = false;
            continue;
        }
        else if (arg[0] == '-') {
            cerr << "Unknown option: \"" << arg << '"' << endl;
            usage(cerr);
            return BAD_ARG;
        }
        else {
            if (no_options_flag) {
                video_flags.set();
                audio_flags.set();
            }
            inputfile = arg;
            break;
        }
    }

    if (inputfile.empty()) {
        cerr << "Specify <inputfile>." << endl;
        usage(cerr);
        return BAD_ARG;
    }

    // preparations
    auto_ptr<Avs> avs(CreateAvsObj(inputfile.c_str()));
    if (!avs->is_fine()) {
        cout << avs->errmsg() << endl;
        return BAD_AVS;
    }

    cout << left;

    // video stream items
    if (video_flags.any()) {
        auto_ptr<Video> video(avs->video());
        const VideoInfo vi = video->info();

        if (vi.exists) {
            using namespace avsinfo::items::video;

            if (video_flags[WIDTH])             print_video_items(WIDTH, vi.width);
            if (video_flags[HEIGHT])            print_video_items(HEIGHT, vi.height);
            if (video_flags[RATIO])             print_video_items(RATIO, width_to_height_ratio(vi.width, vi.height));
            if (video_flags[FPS])               print_video_items(FPS, vi.fps);
            if (video_flags[FPS_FRACTION]) {
                print_video_items(FPS_FRACTION, conv.strfrom(vi.fps_numerator) + '/' + conv.strfrom(vi.fps_denominator));
            }
            if (video_flags[VIDEO_TIME])        print_video_items(VIDEO_TIME, vi.time);
            if (video_flags[FRAMES])            print_video_items(FRAMES, vi.numof_frames);
            if (video_flags[COLOR_SPACE])       print_video_items(COLOR_SPACE, vi.color_space);
            if (video_flags[BPP])               print_video_items(BPP, vi.bpp);
            if (video_flags[INTERLACE_TYPE]) {
                if (machine_flag) print_video_items(INTERLACE_TYPE, vi.is_fieldbased);
                else print_video_items(INTERLACE_TYPE, (vi.is_fieldbased ? "field based" : "frame based"));
            }
            if (video_flags[FIELD_ORDER]) {
                if (machine_flag) print_video_items(FIELD_ORDER, vi.is_tff);
                else print_video_items(FIELD_ORDER, (vi.is_tff ? "top field first" : "bottom field first"));
            }
        }
        else {
            cerr << "<inputfile> has no video stream: " << inputfile << endl;
        }
    }

    // audio stream items
    if (audio_flags.any()) {
        auto_ptr<Audio> audio(avs->audio());
        const AudioInfo ai = audio->info();

        if (ai.exists) {
            using namespace avsinfo::items::audio;

            if (audio_flags[CHANNELS]) {
                if (machine_flag) print_audio_items(CHANNELS, ai.channels);
                else print_audio_items(CHANNELS, readable_channles(ai.channels));
            }
            if (audio_flags[BIT_DEPTH])     print_audio_items(BIT_DEPTH, ai.bit_depth);
            if (audio_flags[SAMPLE_TYPE]) {
                if (machine_flag) print_audio_items(SAMPLE_TYPE, ai.is_int);
                else print_audio_items(SAMPLE_TYPE, (ai.is_int ? "int" : "float"));
            }
            if (audio_flags[AUDIO_TIME])    print_audio_items(AUDIO_TIME, ai.time);
            if (audio_flags[SAMPLING_RATE]) {
                if (machine_flag) print_audio_items(SAMPLING_RATE, ai.sampling_rate);
                else print_audio_items(SAMPLING_RATE, static_cast<double>(ai.sampling_rate) / 1000);
            }
            if (audio_flags[SAMPLES])       print_audio_items(SAMPLES, ai.numof_samples);
            if (audio_flags[BLOCK_SIZE])    print_audio_items(BLOCK_SIZE, ai.block_size);
        }
        else {
            cerr << "<inputfile> has no audio stream: " << inputfile << endl;
        }
    }

    return OK;
}

// function definitions
template <typename T>
void print_video_items(const video::item_t item, const T value) {
    if (!machine_flag) cout << setw(header_max) << video::header[item] << header_delimeter;
    cout << value;
    if (!machine_flag) cout << video::unit[item];
    cout << endl;
}

template <typename T>
void print_audio_items(const audio::item_t item, const T value) {
    if (!machine_flag) cout << setw(header_max) << audio::header[item] << header_delimeter;
    cout << value;
    if (!machine_flag) cout << audio::unit[item];
    cout << endl;
}

const string readable_channles(const unsigned int channels) {
    switch (channels) {
        case 1:
            return string("mono");
        case 2:
            return string("stereo");
        case 6:
            return string("5.1ch");
        default:
            return conv.strfrom(channels) + "ch";
    }
}

// gcd is abbr for greatest common divisor
// Euclidean algorithm
const unsigned int calc_gcd(unsigned int a, unsigned int b) {
    if (b == 0) return a;
    for (;;) {
        a %= b;
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
    }
}

const string width_to_height_ratio(const unsigned int width, const unsigned int height) {
    const unsigned int gcd = calc_gcd(width, height);
    return conv.strfrom(width/gcd) + ':' + conv.strfrom(height/gcd);
}

