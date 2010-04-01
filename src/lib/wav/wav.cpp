/*
 * wav.cpp
 *  Definitions for "wav.hpp"
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

// TODO: convert little <=> big endian

#include "wav.hpp"
#include "../../helper/cast.hpp"
#include "../../helper/dlogger.hpp"

namespace wav {
    bool RiffHeader::read(std::istream& in) {
        DBGLOG("wav::RiffHeader::read(std::istream&)");

        in.read(util::cast::pointer_cast<char*>(&general_type), sizeof(general_type));
        in.read(util::cast::pointer_cast<char*>(&riff_size),    sizeof(riff_size));
        in.read(util::cast::pointer_cast<char*>(&wav_header),   sizeof(wav_header));

        return (static_cast<std::streamoff>(in.tellg()) == static_cast<std::streamoff>(wav::total_header_size));
    }

    bool RiffHeader::write(std::ostream& out) const {
        DBGLOG("wav::RiffHeader::write(std::ostream&)");

        out.write(util::cast::constpointer_cast<const char*>(&general_type), sizeof(general_type));
        out.write(util::cast::constpointer_cast<const char*>(&riff_size),    sizeof(riff_size));
        out.write(util::cast::constpointer_cast<const char*>(&wav_header),   sizeof(wav_header));

        return (static_cast<std::streamoff>(out.tellp()) == static_cast<std::streamoff>(wav::total_header_size));
    }

    std::istream& operator >>(std::istream& in, RiffHeader& wh) {
        DBGLOG("wav::operator >>(std::istream&, const wav::WavHeader&)");

        if (wh.read(in) == true) return in;
        else throw new std::domain_error("something is wrong in reading wav header.");
    }

    std::ostream& operator <<(std::ostream& out, const RiffHeader& wh) {
        DBGLOG("wav::operator <<(std::ostream&, const wav::WavHeader&)");

        if (wh.write(out) == true) return out;
        else throw new std::domain_error("something is wrong in writing wav header.");
    }
}

