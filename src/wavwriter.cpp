/*
 * wavwriter.cpp
 *  definitions of avsutil::WavWriter class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil.h"

namespace avsutil {
    // constants
    const unsigned __int16 WavWriter::header_size = 44;
    const unsigned __int16 WavWriter::wave_header_size = 36;
    const unsigned __int16 WavWriter::fmt_chunk_size = 16;
    const char* WavWriter::header_riff = "RIFF";
    const char* WavWriter::header_wave = "WAVE";
    const char* WavWriter::header_fmt  = "fmt ";
    const char* WavWriter::header_data = "data";

    /*
     * RIFF WAV header
     *      0                0x03              0x07              0x0b              0x0f
     *      +-----------------+-----------------+-----------------+-----------------+
     * 0x00 |     "RIFF"      |   riff size     |     "WAVE"      |     "fmt "      |
     *      +-----------------+--------+--------+-----------------+-----------------+
     * 0x10 |    chunk size   |fmt type|channels|  sampling rate  |  data per sec   |
     *      +--------+--------+--------+--------+-----------------+-----------------+
     * 0x20 |blk size|bit dep |     "data"      |    data size    |
     *      +--------+--------+-----------------+-----------------+
     * */
    void WavWriter::write_header() {
        // preparation
        AudioInfo ai = avs.get_audioinfo();
        __int32 data_size = (__int32)ai.samples * ai.block_size;
        __int32 riff_frame_size = data_size + wave_header_size;
        __int32 data_per_sec = ai.sampling_rate * ai.block_size;
        __int16 fmt_type = PCM;

        // first line
        fwrite(header_riff,      1, 4, audiofp);
        fwrite(&riff_frame_size, 4, 1, audiofp);
        fwrite(header_wave,      1, 4, audiofp);
        fwrite(header_fmt,       1, 4, audiofp);

        // second line
        fwrite(&fmt_chunk_size,   4, 1, audiofp);
        fwrite(&fmt_type,         2, 1, audiofp);
        fwrite(&ai.channels,      2, 1, audiofp);
        fwrite(&ai.sampling_rate, 4, 1, audiofp);
        fwrite(&data_per_sec,     4, 1, audiofp);

        // third line
        fwrite(&ai.block_size, 2, 1, audiofp);
        fwrite(&ai.bit_depth,  2, 1, audiofp);
        fwrite(header_data,    1, 4, audiofp);
        fwrite(&data_size,     4, 1, audiofp);

        // check
        if (ftell(audiofp) != header_size) {
            fclose(audiofp);
            throw AvsUtilError("Something is wrong at writing header.");
        }
    }

    /*
     * function IClip::GetAudio() returns audio data in the following format:
     *
     *  d:  data
     *  l:  left
     *  r:  right
     *  fl: front left
     *  fr: front right
     *  fc: front center
     *  lf: low frequency
     *  bl: back left
     *  br: back right
     *
     *  case monoral
     *      d0, d1, ... , dn
     *  case stereo
     *      l0, r0, l1, r1, ... , ln, rn
     *  case 5.1 ch (maybe)
     *      fl0, fr0, fc0, lf0, bl0, br0, ... , fln, frn, fcn, lfn, bln, brn
     * */
    void WavWriter::write_data(void (*progress)(const unsigned __int64, const unsigned __int64)) {
        AudioInfo ai = avs.get_audioinfo();
        std::vector<__int8> buf(buf_samples * ai.channels * (ai.bit_depth / 8));
        unsigned __int64 start = 0;
        unsigned __int64 times = ai.samples / buf_samples;
        unsigned __int64 reminder = ai.samples - times * buf_samples;
        for (unsigned __int64 i = 0; i < times ; start += buf_samples, ++i) {
            avs.get_audio(&buf[0], start, buf_samples);
            fwrite(&buf[0], ai.block_size, buf_samples, audiofp);
            if (progress != NULL) (*progress)(start + buf_samples, ai.samples);
        }
        if (reminder > 0) {
            avs.get_audio(&buf[0], start, reminder);
            fwrite(&buf[0], ai.block_size, static_cast<size_t>(reminder), audiofp);
            if (progress != NULL) (*progress)(start + reminder, ai.samples);
        }
    }
}
