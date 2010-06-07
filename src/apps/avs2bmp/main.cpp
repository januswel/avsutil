/*
 * main.cpp
 *  Definitions for main flow of the program
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "main.hpp"

#include "../../include/avsutil.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <stdexcept>

using namespace std;
using namespace avsutil;

// global objects
util::string::typeconverter tconv(locale::classic());
util::string::check checker(locale::classic());

int Main::main(void) {
    auto_ptr<avs_type> avs(avsutil::create_avs(inputfile.c_str()));
    if (!avs->is_fine()) {
        throw avs2bmp_error(BAD_AVS, avs->errmsg());
    }
    video_type* video = avs->video();
    video_type::info_type info = video->info();

    // preparations and error handlings
    for (timerange_array_type::const_iterator itr = timerange_array.begin();
            itr != timerange_array.end(); ++itr) {
        unsigned int first = static_cast<unsigned int>((*itr).first
                * info.fps_numerator / info.fps_denominator) + 1;
        unsigned int last = static_cast<unsigned int>((*itr).second
                * info.fps_numerator / info.fps_denominator) + 1;
        for (unsigned int i = first; i <= last; ++i) {
            target_frames.push_back(i);
        }
    }

    if (target_frames.empty()) {
        throw avs2bmp_error(BAD_ARGUMENT,
                "Specify target frames by using the option"
                " \"-f|--frames\".\n");
    }

    target_frames.sort();
    target_frames.unique();

    if (target_frames.back() >= info.numof_frames) {
        throw avs2bmp_error(BAD_ARGUMENT,
                "Target frames should be smaller than a maximum frame of the"
                " avs file(" + tconv.strfrom(info.numof_frames) + "): "
                + tconv.strfrom(target_frames.back()));
    }

    // do it
    unsigned int i = 0;
    const unsigned int total = target_frames.size();
    const unsigned int frac_width = tconv.strfrom(total).size() * 2 + 1;
    stringstream padding;
    padding << right << setfill('0');
    for (target_frames_type::const_iterator itr = target_frames.begin();
            itr != target_frames.end(); ++itr) {
        unsigned int n = *itr;
        frame_type* frame = video->frame(n - 1);

        padding.clear();
        padding.str("");
        padding << setw(digit) << n;
        string_type filename = base + '.' + padding.str() + ".bmp";

        cout << fixed << setprecision(2)
            << filename << " " << setw(frac_width) << ++i << "/" << total
            << "(" << setw(6) << static_cast<double>(i) * 100 / total << ")\n";

        ofstream fout(filename, ios::binary | ios::trunc);
        if (!fout.good()) {
            throw avs2bmp_error(FILE_IO,
                    "Can't open output file: " + filename);
        }

        fout << frame;
    }

    return OK;
}

int main(const int argc, const char* const argv[]) {
    try {
        locale::global(locale(""));
        Main main;
        main.analyze_option(argc, argv);
        main.preparation();
        return main.start();
    }
    catch (const avs2bmp_error& ex) {
        cerr << ex.what() << endl;
        if (ex.return_value() == BAD_ARGUMENT) usage(cerr);
        return ex.return_value();
    }
    catch (const exception& ex) {
        cerr << "error: " << ex.what() << endl;
        return UNKNOWN;
    }
}

