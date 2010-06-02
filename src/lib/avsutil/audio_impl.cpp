/*
 * audio_impl.cpp
 *  Definitions for caudio_type class
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"

#include "../../helper/dlogger.hpp"
#include "../../helper/wav.hpp"

#include <vector>

namespace avsutil {
    namespace impl {
        caudio_type::caudio_type(PClip clip, IScriptEnvironment* se)
            : mv_clip(clip), mv_se(se), mv_progress_callback(NULL),
            mv_buf_samples(mv_buf_samples_default) {
                DBGLOG("avsutil::impl::caudio_type::"
                        "caudio_type(PClip, IScriptEnvironment*)\n");
                const VideoInfo vi = mv_clip->GetVideoInfo();
                info_type info = {
                    vi.HasAudio(),
                    vi.AudioChannels(),
                    bit_depth(vi.sample_type),
                    (vi.sample_type == SAMPLE_FLOAT ? false : true),
                    static_cast<double>(vi.num_audio_samples)
                        / vi.SamplesPerSecond(),
                    vi.SamplesPerSecond(),
                    vi.num_audio_samples,
                    vi.BytesPerAudioSample()
                };
                mv_info = info;

                DBGLOG( "\n"
                        "exists: " << mv_info.exists << "\n"
                        "channels: " << mv_info.channels << "\n"
                        "bit_depth: " << mv_info.bit_depth << "\n"
                        "is_int: " << mv_info.is_int << "\n"
                        "time: " << mv_info.time << "\n"
                        "sampling_rate: " << mv_info.sampling_rate << "\n"
                        "numof_samples: " << mv_info.numof_samples << "\n"
                        "block_size: " << mv_info.block_size << "\n");
            }

        void caudio_type::write_header(std::ostream& out) const {
            DBGLOG("avsutil::impl::caudio_type::write_header(std::ostream&)");

            format::riff_wav::elements_type elements = {
                mv_info.channels,
                mv_info.bit_depth,
                static_cast<uint32_t>(mv_info.numof_samples),
                mv_info.sampling_rate
            };
            format::riff_wav::header_type header(elements);

            out << header;
        }

        void caudio_type::write_data(std::ostream& out) const {
            DBGLOG("avsutil::impl::caudio_type::write_data(std::ostream&)");

            uint32_t buf_size =
                mv_buf_samples * mv_info.channels * (mv_info.bit_depth / 8);
            DBGLOG("actual buffer size is: "
                    << mv_buf_samples << " * " << mv_info.channels
                    << " * " << (mv_info.bit_depth / 8) << " = " << buf_size);

            std::vector<char> b(buf_size);
            b.reserve(buf_size);
            char* buf = &b[0];

            uint64_t start = 0;
            uint64_t times = mv_info.numof_samples / mv_buf_samples;
            uint64_t reminder = mv_info.numof_samples - times * mv_buf_samples;

            for (uint64_t i = 0; i < times; start += mv_buf_samples, ++i) {
                audio_data(buf, start, mv_buf_samples);
                out.write(buf, buf_size);
            }

            if (reminder > 0) {
                audio_data(buf, start, reminder);
                out.write(buf, mv_info.block_size * static_cast<size_t>(reminder));
                start += reminder;
            }
        }

        void caudio_type::write_data(std::ostream& out, progress_callback_type callback) const {
            DBGLOG("avsutil::impl::caudio_type::write_data(std::ostream&)");

            uint32_t buf_size =
                mv_buf_samples * mv_info.channels * (mv_info.bit_depth / 8);
            DBGLOG("actual buffer size is: "
                    << mv_buf_samples << " * " << mv_info.channels
                    << " * " << (mv_info.bit_depth / 8) << " = " << buf_size);

            std::vector<char> b(buf_size);
            b.reserve(buf_size);
            char* buf = &b[0];

            uint64_t start = 0;
            uint64_t times = mv_info.numof_samples / mv_buf_samples;
            uint64_t reminder = mv_info.numof_samples - times * mv_buf_samples;

            for (uint64_t i = 0; i < times; start += mv_buf_samples, ++i) {
                (*callback)(start, mv_info.numof_samples);
                audio_data(buf, start, mv_buf_samples);
                out.write(buf, buf_size);
            }

            if (reminder > 0) {
                (*callback)(start, mv_info.numof_samples);
                audio_data(buf, start, reminder);
                out.write(buf, mv_info.block_size * static_cast<size_t>(reminder));
                start += reminder;
            }

            (*callback)(start, mv_info.numof_samples);
        }
    }

    std::ostream&
    operator <<(std::ostream& out, const audio_type& audio) {
        DBGLOG( "avsutil::operator <<"
                "(std::ostream&, const avsutil::audio_type&)");

        audio.write(out);
        return out;
    }

    std::ostream&
    operator <<(std::ostream& out, const audio_type* const audio) {
        DBGLOG( "avsutil::operator <<"
                "(std::ostream&, const avsutil::audio_type* const)");

        audio->write(out);
        return out;
    }
}

