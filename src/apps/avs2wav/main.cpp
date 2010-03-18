/*
 * avs2wav main.cpp
 *  extract audio data as RIFF WAV format, from avs file
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "../../include/avsutil.hpp"
#include "avs2wav.h"
#include "../../helper/tconv.hpp"
#include "../../helper/elapsed.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <locale>
#include <vector>
#include <stdexcept>
#include <memory>   // fot std::auto_ptr

#ifdef _MSC_VER
#   include <io.h>      // for _isatty(1), _setmode(2)
#   include <fcntl.h>   // for _setmode(2)
#   include <stdio.h>   // for _fileno(1)
#else
#   include <unistd.h>  // for fileno(1)
#   include <stdio.h>   // for isatty(1)
#endif

// using namespaces
using namespace std;
using namespace avsutil;

// global object
// progresss is abbr of "progress stream"
// this stream is used in progress_cl()
// streambuf is stdout fow now
ostream progresss(cout.rdbuf());

// type converter routing through std::string;
util::string::converter conv;

// forward declarations
// return true if redirected to file or connected to pipe
bool is_connected(void);
// set stdout to binary mode
void set_stdout_binary(void);
// callback function for IAudio::write()
void progress_cl(const unsigned __int64, const unsigned __int64);
// output audio informations
ostream& operator <<(ostream&, const AudioInfo&);

int main(const unsigned int argc, const char* argv[]) {
    // set global locale to use non-ascii characters for filenames
    locale::global(locale(""));

    // constants
    const unsigned int header_width = 24;
    const unsigned int buf_samples_def = 4096;
    const unsigned int buf_size_def = 4096;
    const unsigned int buf_samples_min = 1;
    const unsigned int buf_size_min = 2;

    // analyze options
    string inputfile;
    string outputfile;
    unsigned int buf_samples = buf_samples_def;
    unsigned int buf_size = buf_size_def;
    for (unsigned int i = 1; i < argc; ++i) {
        const string arg(argv[i]);
        if (arg == "-h" || arg == "--help") {
            usage(cout);
            return OK;
        }
        else if (arg == "-v" || arg == "--version") {
            version_license(cout);
            return OK;
        }
        else if (arg == "-b" || arg == "--buffers") {
            buf_size = conv.strto<unsigned int>(argv[++i]);
            if (buf_size < buf_size_min) {
                cerr << "Size of buffers for output is required at least: "
                     << buf_size_min << endl
                     << "Check the argument with \"-b\" option." << endl;
                return BAD_ARG;
            }
        }
        else if (arg == "-s" || arg == "--samples") {
            buf_samples = conv.strto<unsigned int>(argv[++i]);
            if (buf_samples < buf_samples_min) {
                cerr << "A number of samples processed at one time is required at least: "
                     << buf_samples_min << endl
                     << "Check the argument with \"-s\" option." << endl;
                return BAD_ARG;
            }
        }
        else if (arg == "-o" || arg == "--output") {
            outputfile = argv[++i];
        }
        else if (arg[0] == '-') {
            cerr << "Unknown option: \"" << arg << '"' << endl;
            usage(cerr);
            return BAD_ARG;
        }
        else {
            inputfile = arg;
        }
    }

    if (inputfile.empty()) {
        cerr << "Specify <inputfile>." << endl;
        usage(cerr);
        return BAD_ARG;
    }

    try {
        // read in avs file
        auto_ptr<IAvs> avs(CreateAvsObj(inputfile.c_str()));
        if (!avs->is_fine()) {
            cerr << avs->errmsg() << endl;
            return BAD_AVS;
        }

        // get audio stream
        auto_ptr<IAudio> audio(avs->audio());
        AudioInfo ai = audio->info();

        if (!ai.exists) {
            cerr << "no audio in the file: " << inputfile << endl;
            return BAD_AVS;
        }

        // preparations
        // output and information stream (to stdout for now)
        ostream outputs(cout.rdbuf());
        ostream infos(cout.rdbuf());
        // apply manipulators to form data
        progresss << right << fixed << setprecision(2);
        infos << left;

        // settings for infos, outputs and progresss
        // this is true if redirected to file or connected to pipe
        if (!is_connected()) {
            // if output to file

            // set output filename if it isn't decided still
            if (outputfile.empty()) outputfile.assign(inputfile).append(".wav");

            // create filebuf and set it output stream
            filebuf* fbuf = new filebuf;
            fbuf->open(outputfile.c_str(), ios::out | ios::binary | ios::trunc);
            if (!fbuf->is_open()) {
                cerr << "Can't open file to write: " << outputfile << endl;
                return UNKNOWN;
            }

            // set filebuf to output stream
            outputs.rdbuf(fbuf);
        }
        else {
            // if output to stdout

            // only to show
            outputfile.assign("stdout");
            // use stderr to show a progress of the process and informations of avs
            progresss.rdbuf(cerr.rdbuf());
            infos.rdbuf(cerr.rdbuf());
            // set stdout to binary mode (Windows only)
            set_stdout_binary();
        }

        infos
            << setw(header_width) << "source:"                << inputfile << endl
            << setw(header_width) << "destination:"           << outputfile << endl
            << setw(header_width) << "buffers for output:"    << buf_size << " bytes" << endl
            << setw(header_width) << "processed at one time:" << buf_samples << " samples" << endl
            << ai;

        // allocate buffer
        std::vector<char> internalbuf(buf_size);
        outputs.rdbuf()->pubsetbuf(&internalbuf[0], buf_size);
        // set values to IAudio
        audio->buf_samples(buf_samples);
        audio->progress_callback(progress_cl);

        // do it
        outputs << audio.get();

        infos
            << endl
            << endl
            << "done." << endl
            << endl;
    }
    catch (exception& ex) {
        cerr << endl << ex.what() << endl;
        return UNKNOWN;
    }

    return OK;
}

// definitions of functions
bool inline is_connected(void) {
#ifdef _MSC_VER
    return !_isatty(_fileno(stdout));
#else
    return !isatty(fileno(stdout));
#endif
}

void inline set_stdout_binary(void) {
#ifdef _MSC_VER
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}

void progress_cl(const unsigned __int64 processed, const unsigned __int64 max) {
    static util::time::elapsed elapsed_time;
    float percentage = (static_cast<float>(processed) / static_cast<float>(max)) * 100;
    progresss
        << "\r"
        << setw(10) << processed << "/" << max << " samples"
        << " (" << setw(6) << percentage << "%)"
        << " elapsed time " << elapsed_time.get() << " sec";
}

ostream& operator <<(ostream& out, const AudioInfo& ai) {
    // constants
    static const unsigned int header_width = 18;

    // preparations
    string channels;
    switch (ai.channels) {
        case 1: channels = "mono";   break;
        case 2: channels = "stereo"; break;
        case 6: channels = "5.1ch";  break;
        default:
            channels.assign(conv.strfrom(ai.channels)).append("ch");
            break;
    }
    float sampling_rate = static_cast<float>(ai.sampling_rate) / 1000;

    // instantiate new ostream object and set streambuf of "out" to it
    // in order to save the formattings of "out"
    ostream o(out.rdbuf());
    o << left << setfill('.')
        << "\n"
        << setw(header_width) << "bit depth "     << ' ' << ai.bit_depth << "bit\n"
        << setw(header_width) << "channels "      << ' ' << channels << "\n"
        << setw(header_width) << "sampling rate " << ' ' << sampling_rate << "kHz\n"
        << endl;

    return out;
}
