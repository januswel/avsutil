/*
 * wav.hpp
 *  A struct and constant variables to read / write headers in the form of wav - RIFF linear PCM
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 *
 * refer
 *  RIFF WAV specifications
 *      http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
 *
 * */

#ifndef WAV_HPP
#define WAV_HPP

#include <istream>
#include <ostream>

namespace wav {
    const unsigned __int16 total_header_size = 44;
    const unsigned __int16 riff_header_size = 36;
    const unsigned __int16 fmt_chunk_size = 16;
    const char general_type[4] = {'R', 'I', 'F', 'F'};
    const char riff_type[4] = {'W', 'A', 'V', 'E'};
    const char fmt_mark[4]  = {'f', 'm', 't', ' '};
    const char data_mark[4] = {'d', 'a', 't', 'a'};
    enum {LINEAR_PCM = 1};

    /*
     * RIFF WAV header
     *  data is sound data
     *  riff size:      data size + riff_header_size(36 byte)
     *  fmt chunk size:   sizeof(fmt type) + sizeof(channels)
     *                  + sizeof(sampling rate)
     *                  + sizeof(data per sec)
     *                  + sizeof(blk size) + sizeof(bit dep) = 16 byte
     *  fmt type:       linear PCM(0x01)
     *      0                0x03              0x07              0x0b              0x0f
     *      +-----------------+-----------------+-----------------------------------+
     * 0x00 |     "RIFF"      |   riff size     |     "WAVE"      |     "fmt "      |
     *      +-----------------+--------+--------+-----------------------------------+
     * 0x10 | fmt chunk size  |fmt type|channels|  sampling rate  |  data per sec   |
     *      +--------+--------+--------+--------+-----------------+-----------------+
     * 0x20 |blk size|bit dep |     "data"      |    data size    |
     *      +--------+--------+-----------------+-----------------+
     * */
    struct WavHeader {
        // data
        char general_type[4];
        unsigned __int32 riff_size;
        char riff_type[4];
        char fmt_mark[4];
        unsigned __int32 fmt_chunk_size;
        unsigned __int16 fmt_type;
        unsigned __int16 channels;
        unsigned __int32 sampling_rate;
        unsigned __int32 data_per_sec;
        unsigned __int16 block_size;
        unsigned __int16 bit_depth;
        char data_mark[4];
        unsigned __int32 data_size;

        // interfaces to accessing file
        bool read(std::istream &);
        bool write(std::ostream &) const;
    };
}

std::istream& operator >>(std::istream&, wav::WavHeader&);
std::ostream& operator <<(std::ostream&, const wav::WavHeader&);

#endif // WAV_HPP
