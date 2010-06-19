/*
 * video_impl.hpp
 *  Declarations and definitions for a class cvideo_type
 *
 *  written by janus_wel<janus.wel.3@gmail.com>
 *  This source code is in public domain, and has NO WARRANTY.
 * */

#ifndef VIDEO_IMPL_HPP
#define video_impl_hpp

#include "../../include/avsutil.hpp"

#include "avisynth.h"

#include "iframestream.hpp"

#include <algorithm>
#include <functional>
#include <istream>
#include <list>

#include "../../helper/algorithm.hpp"

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
                            "cvideo_type(Pclip, IScriptEnvironment*,"
                            " const info_type&)\n"
                            "exists: " << mv_info.exists << "\n"
                            "width: " << mv_info.width << "\n"
                            "height: " << mv_info.height << "\n"
                            "time: " << mv_info.time << "\n"
                            "fps: " << mv_info.fps << "\n"
                            "fps_numerator: " << mv_info.fps_numerator << "\n"
                            "fps_denominator: "
                                << mv_info.fps_denominator << "\n"
                            "numof_frames: " << mv_info.numof_frames << "\n"
                            "color_space: "
                                << mv_info.fourcc_name(mv_info.color_space)
                                << "\n"
                            "bpp: " << mv_info.bpp << "\n"
                            "is_fieldbased: " << mv_info.is_fieldbased << "\n"
                            "is_tff: " << mv_info.is_tff << "\n");
                }

            public:
                // destructor
                ~cvideo_type(void) {
                    DBGLOG("avsutil::impl::cvideo_type::~cvideo_type(void)");
                    std::for_each(
                            framestreams.rbegin(), framestreams.rend(),
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

                std::istream& framestream(uint32_t n) {
                    if (!mv_rgb_clip->GetVideoInfo().IsRGB24()) {
                        DBGLOG("convert to RGB24");
                        AVSValue clip = mv_rgb_clip;
                        AVSValue args = AVSValue(&clip, 1);
                        AVSValue converted =
                            mv_se->Invoke("ConvertToRGB24", args);
                        mv_rgb_clip = converted.AsClip();
                    }

                    framestreams_type::iterator found =
                        std::find_if(
                                framestreams.begin(), framestreams.end(),
                                std::bind2nd(std::mem_fun(
                                        &avsutil::impl::iframestream::is_me
                                        ), n));

                    // found
                    if (found != framestreams.end()) return **found;

                    // not found and create
                    DBGLOG("create new iframestream object with: " << n << "frame");
                    PVideoFrame frame = mv_rgb_clip->GetFrame(n, mv_se);
                    iframestream* created = new iframestream(frame, n);
                    framestreams.push_back(created);
                    return *created;
                }

                void release_framestream(std::istream& target) {
                    framestreams_type::iterator found =
                        std::find(
                                framestreams.begin(), framestreams.end(),
                                &target);

                    if (found != framestreams.end()) {
                        delete *found;
                        framestreams.erase(found);
                    }
                }

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
        };
    }
}

#endif // VIDEO_IMPL_HPP

