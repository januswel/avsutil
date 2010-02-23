/*
 * wav.cpp
 *  Definitions for "wav.hpp"
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

// TODO: convert little <=> big endian

#include "wav.hpp"
#include "../../helper/cast.hpp"
#include "../debuglogger/debuglogger.hpp"

namespace wav {
    bool WavHeader::read(std::istream& in) {
        DBGLOG(FUNCNAME << "(std::istream&)");
        in.read(general_type, 4);
        in.read(pointer_cast<char*>(&riff_size), sizeof(riff_size));
        in.read(riff_type, 4);
        in.read(fmt_mark, 4);
        in.read(pointer_cast<char*>(&fmt_chunk_size), sizeof(fmt_chunk_size));
        in.read(pointer_cast<char*>(&fmt_type), sizeof(fmt_type));
        in.read(pointer_cast<char*>(&channels), sizeof(channels));
        in.read(pointer_cast<char*>(&sampling_rate), sizeof(sampling_rate));
        in.read(pointer_cast<char*>(&data_per_sec), sizeof(data_per_sec));
        in.read(pointer_cast<char*>(&block_size), sizeof(block_size));
        in.read(pointer_cast<char*>(&bit_depth), sizeof(bit_depth));
        in.read(data_mark, 4);
        in.read(pointer_cast<char*>(&data_size), sizeof(data_size));
        return (static_cast<std::streamoff>(in.tellg()) == static_cast<std::streamoff>(wav::total_header_size));
    }

    bool WavHeader::write(std::ostream& out) const {
        DBGLOG(FUNCNAME << "(std::ostream&)");
        out.write(general_type, 4);
        out.write(constpointer_cast<const char*>(&riff_size), sizeof(riff_size));
        out.write(riff_type, 4);
        out.write(fmt_mark, 4);
        out.write(constpointer_cast<const char*>(&fmt_chunk_size), sizeof(fmt_chunk_size));
        out.write(constpointer_cast<const char*>(&fmt_type), sizeof(fmt_type));
        out.write(constpointer_cast<const char*>(&channels), sizeof(channels));
        out.write(constpointer_cast<const char*>(&sampling_rate), sizeof(sampling_rate));
        out.write(constpointer_cast<const char*>(&data_per_sec), sizeof(data_per_sec));
        out.write(constpointer_cast<const char*>(&block_size), sizeof(block_size));
        out.write(constpointer_cast<const char*>(&bit_depth), sizeof(bit_depth));
        out.write(data_mark, 4);
        out.write(constpointer_cast<const char*>(&data_size), sizeof(data_size));
        return (static_cast<std::streamoff>(out.tellp()) == static_cast<std::streamoff>(wav::total_header_size));
    }

    std::istream& operator >>(std::istream& in, WavHeader& wh) {
        DBGLOG(FUNCNAME << "(std::istream&, const wav::WavHeader&)");
        if (wh.read(in) == true) return in;
        else throw new std::domain_error("something is wrong in reading wav header.");
    }

    std::ostream& operator <<(std::ostream& out, const WavHeader& wh) {
        DBGLOG(FUNCNAME << "(std::ostream&, const wav::WavHeader&)");
        if (wh.write(out) == true) return out;
        else throw new std::domain_error("something is wrong in writing wav header.");
    }
}
