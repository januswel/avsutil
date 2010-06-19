/*
 * avsinfo main.cpp
 *  Shows informations of AviSynth script
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsinfo.hpp"
#include "main.hpp"
#include "items.hpp"

#include "../../include/avsutil.hpp"

#include <iomanip>
#include <iostream>
#include <locale>
#include <stdexcept>

// using namespaces
using namespace std;
using namespace avsutil;
using namespace avsinfo::items;

int Main::main(void) {
    if (inputfile.empty()) {
        throw avsinfo_error(BAD_ARGUMENT, "Specify <inputfile>\n");
    }

    if (video_items.empty() && audio_items.empty()) {
        add_all_video_items(video_items);
        add_all_audio_items(audio_items);
    }

    // preparations
    avs_type& avs = manager().load(inputfile.c_str());
    if (!avs.is_fine()) {
        throw avsinfo_error(BAD_AVS, avs.errmsg());
    }

    // video items
    const video_type::info_type& video_info = avs.video().info();;

    if (video_info.exists) {
        video_items.notation(is_human_friendly).output(cout, video_info);
    }
    else {
        throw avsinfo_error(
                BAD_AVS,
                "<inputfile> has no video: " + inputfile);
    }

    // audio items
    const audio_type::info_type& audio_info = avs.audio().info();

    if (audio_info.exists) {
        audio_items.notation(is_human_friendly).output(cout, audio_info);
    }
    else {
        throw avsinfo_error(
                BAD_AVS,
                "<inputfile> has no audio: " + inputfile);
    }

    return OK;
}

int main(const int argc, const char* const argv[]) {
    try {
        std::locale::global(std::locale(""));
        Main main;
        main.analyze_option(argc, argv);
        main.preparation();
        return main.start();
    }
    catch (const avsinfo_error& ex) {
        std::cerr << ex.what() << std::endl;
        if (ex.return_value() == BAD_ARGUMENT) usage(std::cerr);
        return ex.return_value();
    }
    catch (const std::exception& ex) {
        std::cerr << "error: " << ex.what() << std::endl;
        return UNKNOWN;
    }
}

