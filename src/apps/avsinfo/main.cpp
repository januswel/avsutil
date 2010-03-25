/*
 * avsinfo main.cpp
 *  AviSynth script validator
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "../../include/avsutil.hpp"
#include "avsinfo.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <locale>
#include <memory>   // for std::auto_ptr

using namespace std;
using namespace avsutil;

int main(const int argc, const char* const argv[]) {
    // set global locale to use non-ascii characters for filenames
    locale::global(locale(""));

    string inputfile;
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
        else if (arg[0] == '-') {
            cerr << "Unknown option: \"" << arg << '"' << endl;
            usage(cerr);
            return BAD_ARG;
        }
        else {
            inputfile = arg;
            break;
        }
    }

    if (inputfile.empty()) {
        cerr << "Specify <inputfile>." << endl;
        usage(cerr);
        return BAD_ARG;
    }

    auto_ptr<Avs> avs(CreateAvsObj(argv[1]));
    if (!avs->is_fine()) {
        cout << avs->errmsg() << endl;
        return BAD_AVS;
    }

    auto_ptr<Video> video(avs->video());
    const VideoInfo vi = video->info();
    cout << left << setfill('.')
        << setw(24) << "video stream "          << ' ' << (vi.exists ? "yes" : "no") << "\n"
        << setw(24) << "size "                  << ' ' << vi.width << " x " << vi.height << " px\n"
        << setw(24) << "FPS "                   << ' ' << vi.fps
                                                << " (" << vi.fps_numerator << '/' << vi.fps_denominator << ")\n"
        << setw(24) << "time "                  << ' ' << vi.time << " sec\n"
        << setw(24) << "a number of frames "    << ' ' << vi.numof_frames << "\n"
        << setw(24) << "color space "           << ' ' << vi.color_space << "\n"
        << setw(24) << "bits per pixel "        << ' ' << vi.bpp << "\n"
        << setw(24) << "interlace type "        << ' ' << (vi.is_fieldbased
                ? (vi.is_tff ? "field based(TFF)" : "field based(BFF)")
                : "frame based") << "\n"
        << endl;

    auto_ptr<Audio> audio(avs->audio());
    const AudioInfo ai = audio->info();

    cout
        << setw(24) << "audio stream "          << ' ' << (ai.exists ? "yes" : "no") << "\n"
        << setw(24) << "channels "              << ' ' << ai.channels << "\n"
        << setw(24) << "bit depth "             << ' ' << ai.bit_depth << " bit (" << (ai.is_int ? "int" : "float") << ")\n"
        << setw(24) << "time "                  << ' ' << ai.time << " sec\n"
        << setw(24) << "sampling rate "         << ' ' << (static_cast<double>(ai.sampling_rate) / 1000) << " KHz\n"
        << setw(24) << "a number of samples "   << ' ' << ai.numof_samples << "\n"
        << setw(24) << "bytes per sample "      << ' ' << ai.block_size << " bytes\n"
        << endl;

    return OK;
}

