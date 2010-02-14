/*
 * avs2wav.cpp
 *  extract audio data as RIFF WAV format, from avs file
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "../avisynth.h"
#include "../avsutil.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>

using namespace std;
using namespace avsutil;

// callback function for WavWriter::write()
void progress_cl(const unsigned __int64, const unsigned __int64);

int main(const int argc, const char* argv[]) {
    if (argc < 2) {
        cerr << "specify file name." << endl;
        exit(1);
    }

    string avsfile(argv[1]);
    string wavfile(argv[1]);
    wavfile.append(".wav");
    cout << "source:      " << avsfile << endl;
    cout << "destination: " << wavfile << endl;

    try {
        WavWriter ww;
        ww.open(avsfile);
        AudioInfo ai = ww.get_audioinfo();

        if (!ai.exists) {
            cerr << "no audio in the file: " << avsfile << endl;
            exit(1);
        }

        cout << endl;
        cout << "bit depth:     " << ai.bit_depth << endl;
        cout << "channels:      " << ai.channels << endl;
        cout << "sampling rate: " << ai.sampling_rate << endl;
        cout << "samples:       " << ai.samples << endl;
        cout << endl;

        ww.write(wavfile, progress_cl);

        cout << endl;
        cout << "done." << endl;
    }
    catch (exception& ex) {
        cerr << endl << ex.what() << endl;
        exit(1);
    }

    return 0;
}

void progress_cl(const unsigned __int64 processed, const unsigned __int64 max) {
    float percentage = (static_cast<float>(processed) / static_cast<float>(max)) * 100;
    cout << "\rprocessing... "
        << setw(10) << right << processed << "/" << max
        << "(" << setprecision(2) << setw(6) << right << fixed << percentage << "%)";
}
