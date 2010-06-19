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
#include "iframestream.hpp"

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
        class cvideo_type: public video_type {
            private:
                typedef std::list<iframestream*>  framestreams_type;

            private:
                // variables
                const PClip mv_clip;
                PClip mv_rgb_clip;
                IScriptEnvironment* mv_se;
                const info_type mv_info;
                framestreams_type framestreams;

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
                                  "color_space: " << mv_info.fourcc_name(mv_info.color_space) << "\n"
                                  "bpp: " << mv_info.bpp << "\n"
                                  "is_fieldbased: " << mv_info.is_fieldbased << "\n"
                                  "is_tff: " << mv_info.is_tff << "\n");
                      }

            public:
                // destructor
                ~cvideo_type(void) {
                    DBGLOG("avsutil::impl::cvideo_type::~cvideo_type(void)");
                    std::for_each(framestreams.rbegin(), framestreams.rend(),
                            util::algorithm::sweeper());
                }

            public:
                // equivalent operator
                bool operator==(const cvideo_type& rhs) {
                    return mv_clip == rhs.mv_clip;
                }
                // non-equivalent operator
                bool operator!=(const cvideo_type& rhs) {
                    return !(*this == rhs);
                }

            private:
                // Inhibits copy and assignment.
                // copy constructor
                explicit cvideo_type(const cvideo_type& rhs);
                // assignment operator
                cvideo_type& operator=(const cvideo_type& rhs);

            public:
                /*
                 *  Implementations for some member functions of a super class
                 *  video_type
                 * */
                const info_type& info(void) const { return mv_info; }
                std::istream& framestream(uint32_t n);

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

                void release_framestream(std::istream& target);
        };

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

                std::istream& stream(void);

            public:
                // utility functions
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
                // equivalent operator
                bool operator==(const cavs_type& rhs) {
                    return mv_clip == rhs.mv_clip;
                }
                // non-equivalent operator
                bool operator!=(const cavs_type& rhs) {
                    return !(*this == rhs);
                }

            private:
                // Inhibits copy and assignment.
                // copy constructor
                explicit cavs_type(const cavs_type& rhs);
                // assignment operator
                cavs_type& operator=(const cavs_type& rhs);

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
                // equivalent operator
                bool operator==(const cmanager_type& rhs) {
                    return this == &rhs;
                }
                // non-equivalent operator
                bool operator!=(const cmanager_type& rhs) {
                    return !(*this == rhs);
                }

            private:
                // Inhibits copy and assignment.
                // copy constructor
                explicit cmanager_type(const cmanager_type& rhs);
                // assignment operator
                cmanager_type& operator=(const cmanager_type& rhs);

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

