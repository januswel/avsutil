/*
 * audio_impl.hpp
 *  Declarations and definitions for a class caudio_type
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AUDIO_IMPL_HPP
#define AUDIO_IMPL_HPP

#include "../../include/avsutil.hpp"

#include "avisynth.h"

#include "iaudiostream.hpp"

#include <istream>

namespace avsutil {
    namespace impl {
        class caudio_type : public audio_type {
            private:
                // variables
                const PClip mv_clip;
                IScriptEnvironment* mv_se;
                const info_type mv_info;
                std::istream* mv_stream;

            public:
                // constructor
                explicit caudio_type(   PClip clip, IScriptEnvironment* se,
                                        const info_type& info)
                : mv_clip(clip), mv_se(se), mv_info(info), mv_stream(NULL) {
                    DBGLOG( "avsutil::impl::caudio_type::"
                            "caudio_type(PClip, IScriptEnvironment*,"
                            " const info_type&)\n"
                            "exists: " << mv_info.exists << "\n"
                            "channels: " << mv_info.channels << "\n"
                            "bit_depth: " << mv_info.bit_depth << "\n"
                            "is_int: " << mv_info.is_int << "\n"
                            "time: " << mv_info.time << "\n"
                            "sampling_rate: " << mv_info.sampling_rate << "\n"
                            "numof_samples: " << mv_info.numof_samples << "\n"
                            "block_size: " << mv_info.block_size << "\n");
                }

            public:
                // destructor
                ~caudio_type(void) {
                    DBGLOG("avsutil::impl::caudio_type::~caudio_type(void)");
                    if (mv_stream != NULL) delete mv_stream;
                }

            public:
                // equivalent operator
                bool operator==(const caudio_type& rhs) {
                    return mv_clip == rhs.mv_clip;
                }
                // non-equivalent operator
                bool operator!=(const caudio_type& rhs) {
                    return !(*this == rhs);
                }

            private:
                // Inhibits copy and assignment.
                // copy constructor
                explicit caudio_type(const caudio_type& rhs);
                // assignment operator
                caudio_type& operator=(const caudio_type& rhs);

            public:
                /*
                 *  Implementations for some member functions of a super class
                 *  audio_type.
                 * */
                const info_type& info(void) const { return mv_info; }

                std::istream& stream(void) {
                    DBGLOG( "avsutil::impl::caudio_type::"
                            "stream(void)");

                    if (mv_stream == NULL) {
                        mv_stream = new iaudiostream(mv_clip, mv_se);
                    }
                    return *mv_stream;
                }

            public:
                // utility functions
                static const unsigned int bit_depth(const int sample_type) {
                    DBGLOG( "avsutil::impl::caudio_type::"
                            "bitdepth(const int sample_type)");

                    switch (sample_type) {
                        case SAMPLE_INT8:
                            return 8;
                        case SAMPLE_INT16:
                            return 16;
                        case SAMPLE_INT24:
                            return 24;
                        case SAMPLE_INT32:
                            return 32;
                        case SAMPLE_FLOAT:
                            return sizeof(float) * 8;
                        default:
                            return 0;
                    }
                }
        };
    }
}

#endif // AUDIO_IMPL_HPP

