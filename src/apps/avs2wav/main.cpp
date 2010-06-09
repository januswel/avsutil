/*
 * avs2wav main.cpp
 *  extract audio data as RIFF WAV format, from avs file
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avs2wav.hpp"
#include "main.hpp"

#include "../../include/avsutil.hpp"

#include "../../helper/elapsed.hpp"
#include "../../helper/io.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <memory>   // fot std::auto_ptr
#include <stdexcept>
#include <string>
#include <vector>

// using namespaces
using namespace std;
using namespace avsutil;

// global objects
util::string::typeconverter tconv(locale::classic());
util::string::check checker(locale::classic());
// progresss is abbr of "progress stream"
// this stream is used in progress_cl()
// streambuf is stdout fow now
ostream progresss(cout.rdbuf());

// forward declarations
// callback function for audio_type::write()
void progress_cl(const uint64_t, const uint64_t);
// output audio informations
ostream& operator <<(ostream&, const audio_type::info_type&);

int Main::main(void) {
    // constants
    const unsigned int header_width = 24;

    if (inputfile.empty()) {
        throw avs2wav_error(BAD_ARGUMENT, "Specify <inputfile>\n");
    }

    try {
        // read in avs file
        avs_type& avs = manager().load(inputfile.c_str());
        if (!avs.is_fine()) {
            throw avs2wav_error(BAD_AVS, avs.errmsg());
        }

        // get audio stream
        audio_type& audio = avs.audio();
        audio_type::info_type ai = audio.info();

        if (!ai.exists) {
            throw avs2wav_error(BAD_AVS,
                    "Specified file has no audio stream: " + inputfile);
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
        if (!util::io::is_redirected()) {
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
            util::io::set_stdout_binary();
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
        // set values to audio_type
        audio.buf_samples(buf_samples);
        audio.progress_callback(progress_cl);

        // do it
        outputs << audio;

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

void progress_cl(const uint64_t processed, const uint64_t max) {
    static util::time::elapsed elapsed_time;
    float percentage = (static_cast<float>(processed) / static_cast<float>(max)) * 100;
    progresss
        << "\r"
        << setw(10) << processed << "/" << max << " samples"
        << " (" << setw(6) << percentage << "%)"
        << " elapsed time " << elapsed_time() << " sec";
}

ostream& operator <<(ostream& out, const audio_type::info_type& ai) {
    // constants
    static const unsigned int header_width = 18;

    // preparations
    string channels;
    switch (ai.channels) {
        case 1: channels = "mono";   break;
        case 2: channels = "stereo"; break;
        case 6: channels = "5.1ch";  break;
        default:
            channels.assign(tconv.strfrom(ai.channels)).append("ch");
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

int main(const int argc, const char* argv[]) {
    try {
        std::locale::global(std::locale(""));
        Main main;
        main.analyze_option(argc, argv);
        main.preparation();
        return main.start();
    }
    catch (const avs2wav_error& ex) {
        std::cerr << ex.what() << std::endl;
        if (ex.return_value() == BAD_ARGUMENT) usage(std::cerr);
        return ex.return_value();
    }
    catch (const std::exception& ex) {
        std::cerr << "error: " << ex.what() << std::endl;
        return UNKNOWN;
    }
}

