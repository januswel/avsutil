/*
 * audio_impl.cpp
 *  Definitions for CAudio class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"
#include "../wav/wav.hpp"
#include <vector>
#include "../debuglogger/debuglogger.hpp"

namespace avsutil {
    namespace impl {
        CAudio::CAudio(CAvs* avs, std::auto_ptr<AudioInfo> info)
            : mv_avs(avs), mv_info(info), mv_progress_callback(NULL), mv_buf_samples(mv_buf_samples_default) {
                DBGLOG(FUNCNAME << "(CAvs*, std::auto_ptr<AudioInfo>)@");
            }

        CAudio::~CAudio(void) {
            DBGLOG(FUNCNAME << "(void)");
        }

        void CAudio::write(std::ostream& out) const {
            DBGLOG(FUNCNAME << "(std::ostream&)");
            write_header(out);
            write_data(out);
        }

        void CAudio::write_header(std::ostream& out) const {
            DBGLOG(FUNCNAME << "(std::ostream&)");
            unsigned __int32 data_size = static_cast<unsigned __int32>(mv_info->samples) * mv_info->block_size;
            unsigned __int32 riff_size = data_size + wav::riff_header_size;
            unsigned __int32 data_per_sec = mv_info->sampling_rate * mv_info->block_size;
            unsigned __int16 fmt_type = wav::LINEAR_PCM;
            wav::WavHeader wh = {
                {wav::general_type[0], wav::general_type[1], wav::general_type[2], wav::general_type[3]},
                riff_size,
                {wav::riff_type[0] ,wav::riff_type[1] ,wav::riff_type[2] ,wav::riff_type[3]},
                {wav::fmt_mark[0], wav::fmt_mark[1], wav::fmt_mark[2], wav::fmt_mark[3]},
                wav::fmt_chunk_size,
                fmt_type,
                mv_info->channels,
                mv_info->sampling_rate,
                data_per_sec,
                mv_info->block_size,
                mv_info->bit_depth,
                {wav::data_mark[0], wav::data_mark[1], wav::data_mark[2], wav::data_mark[3]},
                data_size,
            };
            out << wh;
        }

        /*
         * function getaudio() - IClip::GetAudio() returns audio data in the following format:
         *
         *  case mono
         *      d0, d1, ... , dn
         *  case stereo
         *      l0, r0, l1, r1, ... , ln, rn
         *  case 5.1 ch (maybe)
         *      fl0, fr0, fc0, lf0, bl0, br0, ... , fln, frn, fcn, lfn, bln, brn
         *
         *  notes
         *      d:  data            (for mono)
         *      l:  left            (for stereo)
         *      r:  right           (for stereo)
         *      fl: front left      (for 5.1ch)
         *      fr: front right     (for 5.1ch)
         *      fc: front center    (for 5.1ch)
         *      lf: low frequency   (for 5.1ch)
         *      bl: back left       (for 5.1ch)
         *      br: back right      (for 5.1ch)
         * */
        void CAudio::write_data(std::ostream& out) const {
            DBGLOG(FUNCNAME << "(std::ostream&)");

            unsigned __int32 buf_size = mv_buf_samples * mv_info->channels * (mv_info->bit_depth / 8);
            std::vector<char> buf(buf_size);
            DBGLOG("actual buffer size is: "
                    << mv_buf_samples << " * " << mv_info->channels << " * " << (mv_info->bit_depth / 8)
                    << " = " << buf_size);

            unsigned __int64 start = 0;
            unsigned __int64 times = mv_info->samples / mv_buf_samples;
            unsigned __int64 reminder = mv_info->samples - times * mv_buf_samples;

            for (unsigned __int64 i = 0; i < times; start += mv_buf_samples, ++i) {
                if (mv_progress_callback != NULL) (*mv_progress_callback)(start, mv_info->samples);
                mv_avs->getaudio(&buf[0], start, mv_buf_samples);
                out.write(&buf[0], buf_size);
            }

            if (reminder > 0) {
                if (mv_progress_callback != NULL) (*mv_progress_callback)(start, mv_info->samples);
                mv_avs->getaudio(&buf[0], start, reminder);
                out.write(&buf[0], mv_info->block_size * static_cast<size_t>(reminder));
                start += reminder;
            }

            if (mv_progress_callback != NULL) (*mv_progress_callback)(start, mv_info->samples);
        }
    }
}

std::ostream& operator <<(std::ostream& out, const avsutil::IAudio& audio) {
    DBGLOG(FUNCNAME << "(std::ostream&, const avsutil::IAudio&)");
    audio.write(out);
    return out;
}

std::ostream& operator <<(std::ostream& out, const avsutil::IAudio* const audio) {
    DBGLOG(FUNCNAME << "(std::ostream&, const avsutil::IAudio* const)");
    audio->write(out);
    return out;
}
