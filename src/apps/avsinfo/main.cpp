/*
 * avsinfo main.cpp
 *  Shows informations of AviSynth script
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "../../include/avsutil.hpp"
#include "avsinfo.hpp"
#include "items.hpp"

#include <iomanip>
#include <iostream>
#include <locale>
#include <memory>   // for std::auto_ptr
#include <string>

// using namespaces
using namespace std;
using namespace avsutil;
using namespace avsinfo::general;
using namespace avsinfo::items;

int main(const int argc, const char* const argv[]) {
    // set global locale to use non-ascii characters for filenames
    locale::global(locale(""));

    string inputfile;
    bool no_options_flag = true;
    bool is_human_friendly = true;
    VideoItems video_items;
    AudioItems audio_items;
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
            is_human_friendly = false;
            continue;
        }
        else if (arg == "-a" || arg == "--all") {
            video::add_all_items(video_items);
            audio::add_all_items(audio_items);
            no_options_flag = false;
            continue;
        }
        else if (arg == "-i" || arg == "--video") {
            video::add_all_items(video_items);
            no_options_flag = false;
            continue;
        }
        else if (arg == "-u" || arg == "--audio") {
            audio::add_all_items(audio_items);
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
                video::add_all_items(video_items);
                audio::add_all_items(audio_items);
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
    auto_ptr<Video> video(avs->video());
    const VideoInfo vi = video->info();

    if (vi.exists) {
        video_items.notation(is_human_friendly).output(cout, vi);
    }
    else {
        cerr << "<inputfile> has no video stream: " << inputfile << endl;
    }

    // audio stream items
    auto_ptr<Audio> audio(avs->audio());
    const AudioInfo ai = audio->info();

    if (ai.exists) {
        audio_items.notation(is_human_friendly).output(cout, ai);
    }
    else {
        cerr << "<inputfile> has no audio stream: " << inputfile << endl;
    }

    return OK;
}

