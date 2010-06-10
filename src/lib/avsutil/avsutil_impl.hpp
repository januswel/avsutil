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

#include "../../helper/algorithm.hpp"
#include "../../helper/dlogger.hpp"

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <ostream>
#include <string>

namespace avsutil {
    namespace impl {
        // forward decralations
        class cvideo_type;

        class cframe_type : public frame_type {
            private:
                // variables
                const PVideoFrame frame;
                const info_type mv_info;
                cvideo_type* video;

            public:
                // constructor
                explicit cframe_type(   PVideoFrame frame, cvideo_type* video,
                                        const info_type& info)
                    : frame(frame), mv_info(info), video(video) {
                        DBGLOG( "avsutil::impl::cframe_type::"
                                "cframe_type(PVideoFrame)\n"
                                "width: " << mv_info.width << "\n"
                                "pitch: " << mv_info.pitch << "\n"
                                "height: " << mv_info.height << "\n");
                    }

            public:
                // destructor
                ~cframe_type(void) {
                    DBGLOG( "avsutil::impl::cframe_type::~cframe_type(void)");
                }

            public:
                /*
                 *  Implementations of some member functions of a super class
                 *  frame_type
                 * */
                const info_type& info(void) const { return mv_info; }

                void write(std::ostream& out) const {
                    DBGLOG("avsutil::impl::cframe_type::write(std::ostream&)");
                    write_header(out);
                    write_data(out);
                }

                void release(void) const;

            public:
                // utility functions
                bool is_me(uint32_t n) const { return mv_info.nth == n; }

            private:
                // utility functions
                void write_header(std::ostream&) const;
                void write_data(std::ostream&) const;
        };

        class cvideo_type: public video_type {
            private:
                typedef std::list<cframe_type*>  cframes_type;

            private:
                // variables
                const PClip mv_clip;
                PClip mv_rgb_clip;
                IScriptEnvironment* mv_se;
                const info_type mv_info;
                cframes_type cframes;

            public:
                // constructor
                explicit cvideo_type(   PClip clip, IScriptEnvironment* se,
                                        const info_type& info)
                    : mv_clip(clip), mv_rgb_clip(clip),
                      mv_se(se), mv_info(info) {
                          DBGLOG( "avsutil::impl::cvideo_type::"
                                  "cvideo_type(Pclip, IScriptEnvironment*)\n"
                                  "exists: " << mv_info.exists << "\n"
                                  "width: " << mv_info.width << "\n"
                                  "height: " << mv_info.height << "\n"
                                  "time: " << mv_info.time << "\n"
                                  "fps: " << mv_info.fps << "\n"
                                  "fps_numerator: " << mv_info.fps_numerator << "\n"
                                  "fps_denominator: " << mv_info.fps_denominator << "\n"
                                  "numof_frames: " << mv_info.numof_frames << "\n"
                                  "color_space: " << mv_info.fourcc_name() << "\n"
                                  "bpp: " << mv_info.bpp << "\n"
                                  "is_fieldbased: " << mv_info.is_fieldbased << "\n"
                                  "is_tff: " << mv_info.is_tff << "\n");
                      }

            public:
                // destructor
                ~cvideo_type(void) {
                    DBGLOG("avsutil::impl::cvideo_type::~cvideo_type(void)");
                    std::for_each(cframes.rbegin(), cframes.rend(),
                            util::algorithm::sweeper());
                }

            public:
                /*
                 *  Implementations for some member functions of a super class
                 *  video_type
                 * */
                const info_type& info(void) const { return mv_info; }
                frame_type& frame(uint32_t n);

            public:
                // utility functions
                static const info_type::fourcc_type
                fourcc(const int pixel_type) {
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

                void release_frame(const cframe_type* const target);
        };

        class caudio_type : public audio_type {
            private:
                // variables
                const PClip mv_clip;
                IScriptEnvironment* mv_se;
                const info_type mv_info;
                progress_callback_type mv_progress_callback;
                uint32_t mv_buf_samples;

                // constants
                static const uint64_t mv_buf_samples_default = 4096;

            public:
                // constructor
                explicit caudio_type(   PClip clip, IScriptEnvironment* se,
                                        const info_type& info)
                    : mv_clip(clip), mv_se(se), mv_info(info),
                      mv_progress_callback(NULL),
                      mv_buf_samples(mv_buf_samples_default) {
                        DBGLOG("avsutil::impl::caudio_type::"
                                "caudio_type(PClip, IScriptEnvironment*)\n"
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
                }

            public:
                /*
                 *  Implementations for some member functions of a super class
                 *  audio_type.
                 * */
                const info_type& info(void) const { return mv_info; }
                void write(std::ostream& out) const {
                    DBGLOG("avsutil::impl::caudio_type::write(std::ostream&)");
                    write_header(out);
                    (mv_progress_callback == NULL)
                        ? write_data(out)
                        : write_data(out, mv_progress_callback);
                }
                void
                progress_callback(progress_callback_type progress_callback) {
                    mv_progress_callback = progress_callback;
                }
                void buf_samples(const uint32_t buf_samples) {
                    mv_buf_samples = buf_samples;
                }

            public:
                // utility functions
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
                    DBGLOG("avsutil::impl::caudio_type::audio_data(char*, "
                            << start << ", " << count << ")");
                    mv_clip->GetAudio(buf, start, count, mv_se);
                }

                static const unsigned int bit_depth(const int sample_type) {
                    DBGLOG("avsutil::impl::caudio_type::"
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

            private:
                // utility functions
                void write_header(std::ostream&) const;
                void write_data(std::ostream&) const;
                void write_data(std::ostream&, progress_callback_type) const;
        };

        class cavs_type : public avs_type {
            private:
                // variables

                /*
                 *  "delete"ing the object of IScriptEnvironment in the
                 *  destructor in this class is a cause of an unknown exception
                 *  with the AviSynth function FFMpegSource that is difficult
                 *  to cope: maybe, because the time to call the destructor is
                 *  too early.
                 *  So there is a need for delaying a release of the object, by
                 *  using std::auto_ptr.
                 * */
                std::auto_ptr<IScriptEnvironment> mv_se;
                PClip mv_clip;
                bool mv_is_fine;
                std::string mv_filepath;
                std::string mv_errmsg;
                video_type* mv_video;
                audio_type* mv_audio;

            public:
                // constructor
                cavs_type(void)
                    : mv_se(CreateScriptEnvironment()),
                      mv_is_fine(true),
                      mv_video(NULL),
                      mv_audio(NULL) {
                    DBGLOG("avsutil::impl::cavs_type::cavs_type(void)");
                }

            public:
                // destructor
                ~cavs_type(void) {
                    DBGLOG("avsutil::impl::cavs_type::~cavs_type(void)");
                    if (mv_audio != NULL) delete mv_audio;
                    if (mv_video != NULL) delete mv_video;
                }

            public:
                /*
                 *  Implementations for some member functions of a super class
                 *  avs_type.
                 * */
                const char* filepath(void) const {
                    return mv_filepath.c_str();
                }
                bool is_fine(void) const { return mv_is_fine; }
                const char* errmsg(void) const { return mv_errmsg.c_str(); }

                video_type& video(void);
                audio_type& audio(void);

            public:
                // utility functions
                // Opens AVS file and sets some member variables.
                void open(const char* avsfile);
                // to identify
                bool is_me(const char* filepath) const {
                    return (mv_filepath.compare(filepath) == 0);
                }
        };

        class cmanager_type : public manager_type {
            private:
                typedef std::list<cavs_type*> cavses_type;

            private:
                cavses_type cavses;

            public:
                // constructor
                cmanager_type(void) {
                    DBGLOG("cavs_loader_type::cavs_loader_type(void)");
                }

            public:
                // destructor
                ~cmanager_type(void) {
                    DBGLOG("cavs_loader_type::~cavs_loader_type(void)");
                    std::for_each(
                            cavses.rbegin(), cavses.rend(),
                            util::algorithm::sweeper());
                }

            public:
                /*
                 *  Implementations of some member functions of a super class
                 *  manager_type.
                 * */
                avs_type& load(const char* file_path);
                void unload(const avs_type& target);
        };
    }
}

#endif // AVSUTIL_IMPL_HPP

