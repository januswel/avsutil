/*
 * main.cpp
 *  Definitions for main flow of the program
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "main.hpp"

#include "../../include/avsutil.hpp"

#include "../../helper/bmp.hpp"
#include "../../helper/algorithm.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <stdexcept>

using namespace std;
using namespace avsutil;

int Main::main(void) {
    // preparations and error handlings
    if (inputfile.empty()) {
        throw avs2bmp_error(BAD_ARGUMENT, "Specify <inputfile>\n");
    }

    if (base.empty()) base = inputfile;

    // Read in avs file.
    avs_type& avs = manager().load(inputfile.c_str());
    if (!avs.is_fine()) {
        throw avs2bmp_error(BAD_AVS, avs.errmsg());
    }
    video_type& video = avs.video();
    const video_type::info_type& info = video.info();

    // Generate actual target frames.
    for (timerange_array_type::const_iterator itr = timerange_array.begin();
            itr != timerange_array.end(); ++itr) {
        unsigned int first = static_cast<unsigned int>((*itr).first * info.fps) + 1;
        unsigned int last = static_cast<unsigned int>((*itr).second * info.fps) + 1;

        std::generate_n(
                std::back_inserter(target_frames), last - first,
                util::algorithm::counter<unsigned int>(first, 1));
    }

    if (target_frames.empty()) {
        throw avs2bmp_error(BAD_ARGUMENT,
                "Specify target frames by using the option"
                " \"-f|--frames\".\n");
    }

    // Compress if enable.
    target_frames.sort();
    target_frames.unique();

    if (target_frames.back() >= info.numof_frames) {
        throw avs2bmp_error(BAD_ARGUMENT,
                "Target frames should be smaller than a maximum frame of the"
                " avs file(" + tconv.strfrom(info.numof_frames) + "): "
                + tconv.strfrom(target_frames.back()));
    }

    // do it
    // preparations
    unsigned int i = 0;
    unsigned int n;
    const unsigned int total = target_frames.size();
    const unsigned int frac_width = tconv.strfrom(total).size() * 2 + 1;
    double percentage = 0;
    stringstream padding;
    padding.imbue(std::locale::classic());
    padding << right << setfill('0');
    cout << fixed << setprecision(2);
    for (target_frames_type::const_iterator itr = target_frames.begin();
            itr != target_frames.end(); ++itr) {
        // Get frame stream.
        n = *itr;
        std::istream& iframestream = video.framestream(n - 1);

        // Build a filename to output.
        padding.clear();
        padding.str("");
        padding << setw(digit) << n;
        string_type filename = base + '.' + padding.str() + ".bmp";

        // Show progresses.
        percentage = static_cast<double>(i) * 100 / total;
        cout
            << filename << " " << setw(frac_width) << ++i << "/" << total
            << "(" << setw(6) << percentage << "%)\n";

        // Open output file stream.
        ofstream fout(filename, ios::binary | ios::trunc);
        if (!fout.good()) {
            throw avs2bmp_error(FILE_IO,
                    "Can't open output file: " + filename);
        }

        // Write Windows Bitmap header.
        format::windows_bitmap::elements_type elements = {
            info.width,
            info.height
        };
        format::windows_bitmap::header_type header(elements);
        fout << header;

        // Write pixcel data.
        fout << iframestream.rdbuf();

        // Release frame stream.
        video.release_framestream(iframestream);
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

