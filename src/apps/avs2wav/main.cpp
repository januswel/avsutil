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
#include "../../helper/wav.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <locale>
#include <memory>   // fot std::auto_ptr
#include <stdexcept>
#include <string>
#include <vector>

// using namespaces
using namespace std;
using namespace avsutil;

// forward declarations
// output audio informations
ostream& operator <<(ostream&, const audio_type::info_type&);

int Main::main(void) {
    // constants
    const unsigned int header_width = 24;

    if (inputfile.empty()) {
        throw avs2wav_error(BAD_ARGUMENT, "Specify <inputfile>\n");
    }

    // read in avs file
    avs_type& avs = manager().load(inputfile.c_str());
    if (!avs.is_fine()) {
        throw avs2wav_error(BAD_AVS, avs.errmsg());
    }

    // get audio stream
    audio_type& audio = avs.audio();
    const audio_type::info_type& info = audio.info();

    if (!info.exists) {
        throw avs2wav_error(BAD_AVS,
                "Specified file has no audio stream: " + inputfile);
    }

    // preparations
    // output and information stream (to stdout for now)
    ostream targetout(cout.rdbuf());
    ostream infoout(cout.rdbuf());

    // settings for infoout and targetout
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
        targetout.rdbuf(fbuf);
    }
    else {
        // if output to stdout

        // only to show
        outputfile.assign("stdout");
        // use stderr to show a progress of the process and informations of avs
        infoout.rdbuf(cerr.rdbuf());
        // set stdout to binary mode (Windows only)
        util::io::set_stdout_binary();
    }

    // showing informations
    infoout << left
        << setw(header_width) << "source:"              << inputfile << "\n"
        << setw(header_width) << "destination:"         << outputfile << "\n"
        << setw(header_width) << "buffers for output:"  << buf_size << " bytes\n"
        << info;

    // Applying manipulators to the stream to show informations.
    infoout << fixed << setprecision(2);

    // writing wav header
    format::riff_wav::elements_type elements = {
        info.channels,
        info.bit_depth,
        static_cast<uint32_t>(info.numof_samples),
        info.sampling_rate
    };
    format::riff_wav::header_type header(elements);
    targetout << header;

    // allocate buffer
    std::vector<char> buffer(buf_size);
    buffer.reserve(buf_size);
    char* buf = &buffer[0];
    std::istream& ain = audio.stream();

    // preparations for copying audio samples
    const unsigned int block_size = info.block_size;
    uint64_t numerator = 0;
    const uint64_t denominator = info.numof_samples;
    double percentage = 0;
    util::time::elapsed elapsed;
    // go!!
    while (ain.good()) {
        // read and write
        ain.read(buf, buf_size);
        targetout.write(buf, ain.gcount());

        // calculations to show progresses
        numerator += ain.gcount() / block_size;
        percentage = static_cast<double>(numerator) * 100 / denominator;

        // showing progresses
        infoout
            << "\r"
            << numerator << "/" << denominator << " samples"
            << " (" << percentage << "%)"
            << " elapsed " << elapsed() << " sec";
    }

    infoout
        << "\n\ndone.\n"
        << endl;

    return OK;
}

ostream& operator <<(ostream& out, const audio_type::info_type& info) {
    // constants
    static const unsigned int header_width = 18;

    // preparations
    string channels;
    switch (info.channels) {
        case 1: channels = "mono";   break;
        case 2: channels = "stereo"; break;
        case 6: channels = "5.1ch";  break;
        default:
            channels.assign(tconv.strfrom(info.channels)).append("ch");
            break;
    }
    float sampling_rate = static_cast<float>(info.sampling_rate) / 1000;

    // instantiate new ostream object and set streambuf of "out" to it
    // in order to save the formattings of "out"
    ostream o(out.rdbuf());
    o << left << setfill('.')
        << "\n"
        << setw(header_width) << "bit depth "     << ' ' << info.bit_depth << "bit\n"
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

