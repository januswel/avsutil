/*
 * video_impl.cpp
 *  Definitions for CVideo class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"
#include "../../helper/dlogger.hpp"

namespace avsutil {
    namespace impl {
        CVideo::CVideo(CAvs* avs, std::auto_ptr<VideoInfo> info)
            : mv_avs(avs), mv_info(info) {
                DBGLOG("avsutil::impl::CVideo::CVideo(CAvs*, std::auto_ptr<VideoInfo>)\n"
                        << "video stream "      << (mv_info->exists ? "exists" : "doesn't exist") << "\n"
                        << "width: "            << mv_info->width << "px\n"
                        << "height: "           << mv_info->height << "px\n"
                        << "fps: "              << mv_info->fps << "(" << mv_info->fps_numerator << "/" << mv_info->fps_denominator << ")\n"
                        << "time: "             << mv_info->time << "sec (" << mv_info->numof_frames << "frames)\n"
                        << "color space: "      << mv_info->color_space << "\n"
                        << "bits per pixel: "   << mv_info->bpp << "\n"
                        << (mv_info->is_fieldbased ? (mv_info->is_tff ? "field based(top field first)" : "field based(bottom field first)") : "frame based"));
            }

        CVideo::~CVideo(void) {
            DBGLOG("avsutil::impl::CVideo::~CVideo(void)");
        }
    }
}

