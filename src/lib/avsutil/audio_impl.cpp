/*
 * audio_impl.cpp
 *  Definitions for CAudio class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"
#include "../wav/wav.hpp"
#include <vector>
#include "../../helper/dlogger.hpp"

namespace avsutil {
    namespace impl {
        CAudio::CAudio(CAvs* avs, std::auto_ptr<AudioInfo> info)
            : mv_avs(avs), mv_info(info), mv_progress_callback(NULL), mv_buf_samples(mv_buf_samples_default) {
                DBGLOG("avsutil::impl::CAudio::CAudio(CAvs*, std::auto_ptr<AudioInfo>)\n"
                        << "audio stream " << (mv_info->exists ? "exists" : "doesn't exist") << "\n"
                        << "a number of samples: " << mv_info->numof_samples << "\n"
                        << "sampling_rate: " << mv_info->sampling_rate << "\n"
                        << "channels: " << mv_info->channels << "\n"
                        << "bit_depth: " << mv_info->bit_depth << "(" << (mv_info->is_int ? "int" : "float") << ")\n"
                        << "block_size: " << mv_info->block_size);
            }

        CAudio::~CAudio(void) {
            DBGLOG("avsutil::impl::CAudio::~CAudio(void)");
        }

        void CAudio::write(std::ostream& out) const {
            DBGLOG("avsutil::impl::CAudio::write(std::ostream&)");

            write_header(out);
            write_data(out);
        }

        void CAudio::write_header(std::ostream& out) const {
            DBGLOG("avsutil::impl::CAudio::write_header(std::ostream&)");

            unsigned __int32 data_size = static_cast<unsigned __int32>(mv_info->numof_samples) * mv_info->block_size;
            unsigned __int32 riff_size = data_size + wav::riff_header_size;
            unsigned __int32 data_per_sec = mv_info->sampling_rate * mv_info->block_size;
            unsigned __int16 fmt_type = wav::LINEAR_PCM;

            wav::RiffHeader rh = {
                wav::general_type,
                riff_size,
                {
                    wav::riff_type,
                    {
                        wav::fmt_mark,
                        wav::fmt_chunk_size,
                        fmt_type,
                        mv_info->channels,
                        mv_info->sampling_rate,
                        data_per_sec,
                        mv_info->block_size,
                        mv_info->bit_depth
                    },
                    {
                        wav::data_mark,
                        data_size
                    }
                }
            };

            out << rh;
        }

        void CAudio::write_data(std::ostream& out) const {
            DBGLOG("avsutil::impl::CAudio::write_data(std::ostream&)");

            unsigned __int32 buf_size = mv_buf_samples * mv_info->channels * (mv_info->bit_depth / 8);
            std::vector<char> buf(buf_size);
            DBGLOG("actual buffer size is: "
                    << mv_buf_samples << " * " << mv_info->channels << " * " << (mv_info->bit_depth / 8)
                    << " = " << buf_size);

            unsigned __int64 start = 0;
            unsigned __int64 times = mv_info->numof_samples / mv_buf_samples;
            unsigned __int64 reminder = mv_info->numof_samples - times * mv_buf_samples;

            for (unsigned __int64 i = 0; i < times; start += mv_buf_samples, ++i) {
                if (mv_progress_callback != NULL) (*mv_progress_callback)(start, mv_info->numof_samples);
                mv_avs->audio_data(&buf[0], start, mv_buf_samples);
                out.write(&buf[0], buf_size);
            }

            if (reminder > 0) {
                if (mv_progress_callback != NULL) (*mv_progress_callback)(start, mv_info->numof_samples);
                mv_avs->audio_data(&buf[0], start, reminder);
                out.write(&buf[0], mv_info->block_size * static_cast<size_t>(reminder));
                start += reminder;
            }

            if (mv_progress_callback != NULL) (*mv_progress_callback)(start, mv_info->numof_samples);
        }
    }

    std::ostream& operator <<(std::ostream& out, const Audio& audio) {
        DBGLOG("avsutil::operator <<(std::ostream&, const avsutil::Audio&)");

        audio.write(out);
        return out;
    }

    std::ostream& operator <<(std::ostream& out, const Audio* const audio) {
        DBGLOG("avsutil::operator <<(std::ostream&, const avsutil::Audio* const)");

        audio->write(out);
        return out;
    }
}

