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
// callback function for IAudio::write()
void progress_cl(const unsigned __int64, const unsigned __int64);
// output audio informations
ostream& operator <<(ostream&, const AudioInfo&);

int main(const int argc, const char* argv[]) {
    // constants
    static const unsigned int header_width = 14;

    // check arguments
    if (argc < 2) {
        usage(cerr);
        cerr << "Specify <inputfile>." << endl;
        exit(1);
    }

    // input file
    string inputfile(argv[1]);

    try {
        auto_ptr<IAvs> avs(CreateAvsObj(inputfile.c_str()));
        auto_ptr<IAudio> audio(avs->audio());
        AudioInfo ai = audio->info();

        if (!ai.exists) {
            cerr << "no audio in the file: " << inputfile << endl;
            exit(1);
        }

        // preparations
        string outputfile;
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
            outputfile.assign(argv[1]).append(".wav");
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
    out << "Usage: avs2wav <inputfile> [| othercommands]" << endl;
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
        << endl
        << setw(header_width) << "bit depth "     << ' ' << ai.bit_depth << "bit" << endl
        << setw(header_width) << "channels "      << ' ' << channels << endl
        << setw(header_width) << "sampling rate " << ' ' << sampling_rate << "kHz" << endl
        << setw(header_width) << "samples "       << ' ' << ai.samples << endl
        << endl;

    return out;
}
