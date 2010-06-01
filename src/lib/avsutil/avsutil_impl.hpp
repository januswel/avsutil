/*
 * avsutil_impl.hpp
 *  Implementations for "avsutil.hpp"
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVSUTIL_IMPL_HPP
#define AVSUTIL_IMPL_HPP

#include "../../include/avsutil.hpp"

#include "avisynth.h"

#include "../../helper/dlogger.hpp"

#include <ostream>
#include <string>

namespace avsutil {
    namespace impl {
        class cvideo_type: public video_type {
            private:
                PClip mv_clip;
                IScriptEnvironment* mv_se;
                info_type mv_info;

            private:
                // utility functions
                const info_type::fourcc_type
                fourcc(const int pixel_type) const {
                    switch (pixel_type) {
                        case VideoInfo::CS_BGR24:
                        case VideoInfo::CS_BGR32:   return info_type::RGB;
                        case VideoInfo::CS_YUY2:    return info_type::YUY2;
                        case VideoInfo::CS_YV12:    return info_type::YV12;
                        case VideoInfo::CS_I420:    return info_type::I420;
                        case VideoInfo::CS_UNKNOWN:
                        default:                    return info_type::UNKOWN;
                    }
                }

            public:
                // constructor and destructor
                cvideo_type(PClip clip, IScriptEnvironment* se);
                ~cvideo_type(void) {
                    DBGLOG("avsutil::impl::cvideo_type::~cvideo_type(void)");
                }

                // implementations for the member functions of the super class
                // video_type
                const info_type& info(void) const { return mv_info; }
        };

        class caudio_type : public audio_type {
            private:
                // member variables
                PClip mv_clip;
                IScriptEnvironment* mv_se;
                info_type mv_info;
                progress_callback_type mv_progress_callback;
                uint32_t mv_buf_samples;

                // constants
                static const uint64_t mv_buf_samples_default = 4096;

            private:
                void write_header(std::ostream&) const;
                void write_data(std::ostream&) const;

                // utility functions
                const unsigned int bit_depth(const int sample_type) const {
                    DBGLOG("avsutil::impl::cavs_type::"
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

            public:
                // constructor and destructor
                caudio_type(PClip clip, IScriptEnvironment* se);
                ~caudio_type(void) {
                    DBGLOG("avsutil::impl::caudio_type::~caudio_type(void)");
                }

                // implementations for the member functions of super class
                // audio_type
                const info_type& info(void) const { return mv_info; }
                void write(std::ostream& out) const {
                    DBGLOG("avsutil::impl::caudio_type::write(std::ostream&)");
                    write_header(out);
                    write_data(out);
                }
                void
                progress_callback(progress_callback_type progress_callback) {
                    mv_progress_callback = progress_callback;
                }
                void buf_samples(const uint32_t buf_samples) {
                    mv_buf_samples = buf_samples;
                }

                // utilities
                /*
                 * function audio_data() returns audio data in the following format:
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
                void
                audio_data( char* buf,
                            const uint64_t start, const uint64_t count) const {
                    DBGLOG("avsutil::impl::cavs_type::audio_data(char*, "
                            << start << ", " << count << ")");
                    mv_clip->GetAudio(buf, start, count, mv_se);
                }
        };

        class cavs_type : public avs_type {
            private:
                // types defined by avisynth.h
                IScriptEnvironment* mv_se;
                PClip mv_clip;

                // member variable
                bool mv_is_fine;
                std::string mv_filename;
                std::string mv_errmsg;
                video_type* mv_video;
                audio_type* mv_audio;

            public:
                // constructors and destructor
                cavs_type(void)
                    : mv_se(CreateScriptEnvironment()),
                      mv_is_fine(true),
                      mv_video(NULL),
                      mv_audio(NULL) {
                    DBGLOG("avsutil::impl::cavs_type::cavs_type(void)");
                }
                ~cavs_type(void) {
                    DBGLOG("avsutil::impl::cavs_type::~cavs_type(void)");
                    delete mv_se;
                    if (mv_video != NULL) delete mv_video;
                    if (mv_audio != NULL) delete mv_audio;
                }

                // implementations for the member functions of the super class
                // avs_type
                void open(const char* avsfile);

                const char* filename(void) { return mv_filename.c_str(); }
                bool is_fine(void) { return mv_is_fine; }
                const char* errmsg(void) { return mv_errmsg.c_str(); }
                video_type* video(void) {
                    if (mv_video == NULL) mv_video =
                        new cvideo_type(mv_clip, mv_se);
                    return mv_video;
                }
                audio_type* audio(void) {
                    if (mv_audio == NULL) mv_audio =
                        new caudio_type(mv_clip, mv_se);
                    return mv_audio;
                }
        };
    }
}

#endif // AVSUTIL_IMPL_HPP

