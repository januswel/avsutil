/*
 * avs2wav.cpp
 *  extract audio data as RIFF WAV format, from avs file
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "../../include/avsutil.hpp"
#include "avs2wav.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include "../../helper/converter.hpp"
#include <cstdlib> // for std::exit()

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

using namespace std;
using namespace avsutil;

// global object
// progresss is abbr of "progress stream"
// this stream is used in progress_cl()
// streambuf is stdout fow now
ostream progresss(cout.rdbuf());

// converter routing through std::string;
util::string::converter conv;

// forward declarations
// typical one
void usage(ostream& out);
// the another typical
void version_license(ostream& out);
// return true if redirected to file or connected to pipe
bool is_connected(void);
// set stdout to binary mode
void set_stdout_binary(void);
// callback function for IAudio::write()
void progress_cl(const unsigned __int64, const unsigned __int64);
// output audio informations
ostream& operator <<(ostream&, const AudioInfo&);

int main(const unsigned int argc, const char* argv[]) {
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
            return 0;
        }
        else if (arg == "-v" || arg == "--version") {
            version_license(cout);
            return 0;
        }
        else if (arg == "-b" || arg == "--buffers") {
            buf_size = conv.strto<unsigned int>(argv[++i]);
            if (buf_size < buf_size_min) {
                cerr << "Size of buffers for output is required at least: "
                     << buf_size_min << endl
                     << "Check the argument with \"-b\" option." << endl;
                exit(1);
            }
        }
        else if (arg == "-s" || arg == "--samples") {
            buf_samples = conv.strto<unsigned int>(argv[++i]);
            if (buf_samples < buf_samples_min) {
                cerr << "A number of samples processed at one time is required at least: "
                     << buf_samples_min << endl
                     << "Check the argument with \"-s\" option." << endl;
                exit(1);
            }
        }
        else if (arg == "-o" || arg == "--output") {
            outputfile = argv[++i];
        }
        else if (arg[0] == '-') {
            cerr << "Unknown option: \"" << arg << '"' << endl;
            usage(cerr);
            exit(1);
        }
        else {
            inputfile = arg;
        }
    }

    if (inputfile.empty()) {
        cerr << "Specify <inputfile>." << endl;
        usage(cerr);
        exit(1);
    }

    try {
        auto_ptr<IAvs> avs(CreateAvsObj(inputfile.c_str()));
        if (!avs->is_fine()) {
            cerr << avs->errmsg() << endl;
            exit(2);
        }

        auto_ptr<IAudio> audio(avs->audio());
        AudioInfo ai = audio->info();

        if (!ai.exists) {
            cerr << "no audio in the file: " << inputfile << endl;
            exit(2);
        }

        // preparations
        // output and information stream (to stdout for now)
        ostream outputs(cout.rdbuf());
        ostream infos(cout.rdbuf());
        // settings for stream
        progresss << right << fixed << setprecision(2);
        infos << left;
        // this is true if redirected to file or connected to pipe
        bool is_stdout = !is_connected();
        // settings for infos, outputs and progresss
        if (is_stdout) {
            // output to file
            if (outputfile.empty()) outputfile.assign(inputfile).append(".wav");
            // create filebuf and set it output stream
            filebuf* fbuf = new filebuf;
            fbuf->open(outputfile.c_str(), ios::out | ios::binary | ios::trunc);
            outputs.rdbuf(fbuf);
        }
        else {
            // redirected or connected pipe
            // output to stdout
            outputfile.assign("stdout");
            progresss.rdbuf(cerr.rdbuf());
            infos.rdbuf(cerr.rdbuf());
            set_stdout_binary();
        }

        infos
            << setw(header_width) << "source:"                << inputfile << endl
            << setw(header_width) << "destination:"           << outputfile << endl
            << setw(header_width) << "buffers for output:"    << buf_size << " bytes" << endl
            << setw(header_width) << "processed at one time:" << buf_samples << " samples" << endl
            << ai;

        std::vector<char> internalbuf(buf_size);
        outputs.rdbuf()->pubsetbuf(&internalbuf[0], buf_size);
        audio->buf_samples(buf_samples);
        audio->progress_callback(progress_cl);
        outputs << audio.get();

        infos
            << endl
            << "done." << endl
            << endl;
    }
    catch (exception& ex) {
        cerr << endl << ex.what() << endl;
        exit(3);
    }

    return 0;
}

// definitions of functions
void usage(ostream& out) {
    out << "Usage: avs2wav [options] <inputfile> [| othercommands]\n"
        << "Options:\n"
        << "    -h, --help      Shows this help\n"
        << "    -v, --version   Shows version and license informations.\n"
        << "\n"
        << "    -s N            Sets a number of samples processed at one time to N.\n"
        << "                    min: 1, default: 4096.\n"
        << "    --samples N     Same as \"-s\"\n"
        << "    -b N            Sets size of buffer for output to N.\n"
        << "                    Mutiple of a value of \"-s\" is preferable.\n"
        << "                    min: 2, default: 4096.\n"
        << "    --buffers N     Same as \"-b\"\n"
        << "\n"
        << "    -o <file>       Sets a name of output file to <file>.  This is\n"
        << "                    ignored when redirected to file or conneted to\n"
        << "                    other command with pipe.\n"
        << "    --output <file> Same as \"-o\"\n"
        << endl;
}

void version_license(ostream& out) {
    out << "avs2wav version " << version << '\n'
        << "Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>\n"
        << '\n'
        << "This program is free software: you can redistribute it and/or modify\n"
        << "it under the terms of the GNU General Public License as published by\n"
        << "the Free Software Foundation, either version 3 of the License, or\n"
        << "(at your option) any later version.\n"
        << '\n'
        << "This program is distributed in the hope that it will be useful,\n"
        << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        << "GNU General Public License for more details.\n"
        << '\n'
        << "You should have received a copy of the GNU General Public License\n"
        << "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
        << endl;
}

bool is_connected(void) {
#ifdef WIN32
    return !_isatty(_fileno(stdout));
#else
    return !isatty(fileno(stdout));
#endif
}

void set_stdout_binary(void) {
#ifdef WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif
}

void progress_cl(const unsigned __int64 processed, const unsigned __int64 max) {
    float percentage = (static_cast<float>(processed) / static_cast<float>(max)) * 100;
    progresss
        << "\rprocessing... "
        << setw(10) << processed << "/" << max << " samples"
        << " (" << setw(6) << percentage << "%)";
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
