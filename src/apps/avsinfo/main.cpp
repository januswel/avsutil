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
#include <memory>   // for std::auto_ptr
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

    // video stream items
    const video_type::info_type vi = avs.video().info();;

    if (vi.exists) {
        video_items.notation(is_human_friendly).output(cout, vi);
    }
    else {
        cerr << "<inputfile> has no video stream: " << inputfile << endl;
    }

    // audio stream items
    const audio_type::info_type ai = avs.audio().info();

    if (ai.exists) {
        audio_items.notation(is_human_friendly).output(cout, ai);
    }
    else {
        cerr << "<inputfile> has no audio stream: " << inputfile << endl;
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

