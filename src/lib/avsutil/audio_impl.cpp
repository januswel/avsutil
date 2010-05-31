/*
 * audio_impl.cpp
 *  Definitions for CAudio class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"
#include "../../helper/wav.hpp"
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

            format::riff_wav::elements_type elements = {
                mv_info->channels,
                mv_info->bit_depth,
                static_cast<uint32_t>(mv_info->numof_samples),
                mv_info->sampling_rate
            };
            format::riff_wav::header_type header(elements);

            out << header;
        }

        void CAudio::write_data(std::ostream& out) const {
            DBGLOG("avsutil::impl::CAudio::write_data(std::ostream&)");

            uint32_t buf_size = mv_buf_samples * mv_info->channels * (mv_info->bit_depth / 8);
            std::vector<char> buf(buf_size);
            DBGLOG("actual buffer size is: "
                    << mv_buf_samples << " * " << mv_info->channels << " * " << (mv_info->bit_depth / 8)
                    << " = " << buf_size);

            uint64_t start = 0;
            uint64_t times = mv_info->numof_samples / mv_buf_samples;
            uint64_t reminder = mv_info->numof_samples - times * mv_buf_samples;

            for (uint64_t i = 0; i < times; start += mv_buf_samples, ++i) {
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

