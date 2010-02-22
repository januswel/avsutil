/*
 * avs2wav.cpp
 *  extract audio data as RIFF WAV format, from avs file
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "../avsutil.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

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

// forward declarations
// typical one
void usage(ostream& out);
// return true if redirected to file or connected to pipe
bool is_connected(void);
// set stdout to binary mode
void set_stdout_binary(void);
// convert string to number
unsigned int str2num(const char* str);
// callback function for IAudio::write()
void progress_cl(const unsigned __int64, const unsigned __int64);
// output audio informations
ostream& operator <<(ostream&, const AudioInfo&);

int main(const int argc, const char* argv[]) {
    // constants
    const unsigned int header_width = 14;
    const unsigned int buf_samples_def = 4096;
    const unsigned int buf_size_def = 4096;
    const unsigned int buf_samples_min = 1;
    const unsigned int buf_size_min = 2;

    // analyze options
    string inputfile;
    string outputfile;
    unsigned int buf_samples = buf_samples_def;
    unsigned int buf_size = buf_size_def;
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        if (arg[0] == '-') {
            switch (arg[1]) {
                case 'h':
                    usage(cout);
                    exit(0);
            }
            if (i + 1 <= argc) {
                switch (arg[1]) {
                    case 'b':
                        buf_size = str2num(argv[i + 1]);
                        if (buf_size < buf_size_min) buf_size = buf_size_min;
                        ++i;
                        break;
                    case 's':
                        buf_samples = str2num(argv[i + 1]);
                        if (buf_samples < buf_samples_min) buf_samples = buf_samples_min;
                        ++i;
                        break;
                    case 'o':
                        outputfile = argv[i + 1];
                        ++i;
                        break;
                    default:
                        cerr << "Unknown option: \"-" << arg[1] << "\" is ignored." << endl;
                        break;
                }
            }
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
            exit(1);
        }

        auto_ptr<IAudio> audio(avs->audio());
        AudioInfo ai = audio->info();

        if (!ai.exists) {
            cerr << "no audio in the file: " << inputfile << endl;
            exit(1);
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
            if (outputfile.empty()) outputfile.assign(argv[1]).append(".wav");
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
            << setw(header_width) << "source:"      << inputfile << endl
            << setw(header_width) << "destination:" << outputfile << endl
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
        exit(1);
    }

    return 0;
}

// definitions of functions
void usage(ostream& out) {
    out << "Usage: avs2wav [options] <inputfile> [| othercommands]\n"
        << "Options:\n"
        << "    -s N            Set a number of samples used when reading to N.\n"
        << "                    min: 1, default: 4096.\n"
        << "    -b N            Set buffer size used when writing to N.\n"
        << "                    Mutiple of a value of \"-s\" is preferable.\n"
        << "                    min: 2, default: 4096.\n"
        << "    -o <filename>   Output to filename.  This is ignored when redirected\n"
        << "                    to file or conneted to other command with pipe.\n"
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

unsigned int str2num(const char* str) {
    stringstream ss;
    unsigned int num;
    ss << dec << str;
    ss >> num;
    return num;
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
            stringstream ss;
            ss << dec << ai.channels;
            channels.assign(ss.str()).append("ch");
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
