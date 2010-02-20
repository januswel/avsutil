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

using namespace std;
using namespace avsutil;

// global object
// pout is abbr of progress out
// this stream is used in progress_cl()
ostream pout(cout.rdbuf());

// forward declarations
// typical one
void usage(ostream& out);
// callback function for IAudio::write()
void progress_cl(const unsigned __int64, const unsigned __int64);
// output audio informations
ostream& operator <<(ostream&, const AudioInfo&);

int main(const int argc, const char* argv[]) {
    if (argc < 2) {
        usage(cerr);
        cerr << "Specify <inputfile>." << endl;
        exit(1);
    }

    string avsfile(argv[1]);
    string wavfile(argv[1]);
    wavfile.append(".wav");

    const unsigned int header_width = 14;
    cout << left
         << setw(header_width) << "source:"      << avsfile << endl
         << setw(header_width) << "destination:" << wavfile << endl;

    try {
        auto_ptr<IAvs> avs(CreateAvsObj(avsfile.c_str()));
        auto_ptr<IAudio> audio(avs->audio());
        AudioInfo ai = audio->info();

        if (!ai.exists) {
            cerr << "no audio in the file: " << avsfile << endl;
            exit(1);
        }

        cout << ai;

        pout << right << fixed << setprecision(2);
        audio->progress_callback(progress_cl);
        ofstream fout(wavfile.c_str(), ios::binary | ios::trunc);
        fout << audio.get();

        cout << endl
             << "done." << endl;
    }
    catch (exception& ex) {
        cerr << endl << ex.what() << endl;
        exit(1);
    }

    return 0;
}

// definitions of functions
void usage(ostream& out) {
    out << "Usage: avs2wav <inputfile>" << endl;
}

void progress_cl(const unsigned __int64 processed, const unsigned __int64 max) {
    float percentage = (static_cast<float>(processed) / static_cast<float>(max)) * 100;
    pout << "\rprocessing... "
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
