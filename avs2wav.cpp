/*
 * avs2wav.cpp
 *  extract audio data as RIFF WAV format, from avs file
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avisynth.h"
#include "avsutil.h"
#include <iostream>
#include <string>
#include <stdexcept>

using namespace std;
using namespace avsutil;

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
        Avs2Wav a2w;
        a2w.open(avsfile);
        AudioInfo ai = a2w.get_audioinfo();

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
        cout << "processing..." << endl;

        a2w.write(wavfile);

        cout << "finished." << endl;
    }
    catch (exception& ex) {
        cerr << endl << ex.what() << endl;
        exit(1);
    }

    return 0;
}
