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
    // utilities
    // This function return little-endian value.
    // RIFF WAV file is written as little-endian, then we've met our needs by this.
    inline const unsigned __int32 quartet2int(const char _1, const char _2, const char _3, const char _4) {
        return static_cast<unsigned __int32>(_1) + (_2 << 8) + (_3 << 16) + (_4 << 24);
    }

    // constants
    const unsigned __int16 total_header_size    = 44;
    const unsigned __int16 riff_header_size     = 36;
    const unsigned __int16 fmt_chunk_size       = 16;
    const unsigned __int32 general_type         = quartet2int('R', 'I', 'F', 'F');
    const unsigned __int32 riff_type            = quartet2int('W', 'A', 'V', 'E');
    const unsigned __int32 fmt_mark             = quartet2int('f', 'm', 't', ' ');
    const unsigned __int32 data_mark            = quartet2int('d', 'a', 't', 'a');
    enum {LINEAR_PCM = 1};

    /*
     * RIFF WAV header
     *  data is sound data
     *  riff size:      data size + riff_header_size(36 byte)
     *  fmt chunk size:   sizeof(fmt type) + sizeof(channels)
     *                  + sizeof(sampling rate)
     *                  + sizeof(data per sec)
     *                  + sizeof(blk size) + sizeof(bit dep) = 16 byte
     *
     *  RIFF header has:
     *      "RIFF"
     *      riff size
     *      WAV header
     *
     *  WAV header has:
     *      "WAVE"
     *      fmt chunk
     *      data chunk
     *
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
    struct FmtChunk {
        unsigned __int32 fmt_mark;
        unsigned __int32 fmt_chunk_size;
        unsigned __int16 fmt_type;
        unsigned __int16 channels;
        unsigned __int32 sampling_rate;
        unsigned __int32 data_per_sec;
        unsigned __int16 block_size;
        unsigned __int16 bit_depth;
    };

    struct DataChunk {
        unsigned __int32 data_mark;
        unsigned __int32 data_size;
    };

    struct WavHeader {
        unsigned __int32 riff_type;
        FmtChunk fmt_chunk;
        DataChunk data_chunk;
    };

    struct RiffHeader {
        unsigned __int32 general_type;
        unsigned __int32 riff_size;
        WavHeader wav_header;

        // interfaces to accessing file
        bool read(std::istream &);
        bool write(std::ostream &) const;
    };

    std::istream& operator >>(std::istream&, RiffHeader&);
    std::ostream& operator <<(std::ostream&, const RiffHeader&);
}

#endif // WAV_HPP

