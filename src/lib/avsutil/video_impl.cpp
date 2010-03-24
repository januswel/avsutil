/*
 * video_impl.cpp
 *  Definitions for CVideo class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"
#include "../debuglogger/debuglogger.hpp"

namespace avsutil {
    namespace impl {
        CVideo::CVideo(CAvs* avs, std::auto_ptr<VideoInfo> info)
            : mv_avs(avs), mv_info(info) {
                DBGLOG(FUNCNAME << "(CAvs*, std::auto_ptr<VideoInfo>)");
                DBGLOG("video stream "      << (mv_info->exists ? "exists" : "doesn't exist"));
                DBGLOG("width: "            << mv_info->width << "px");
                DBGLOG("height: "           << mv_info->height << "px");
                DBGLOG("fps: "              << mv_info->fps << "(" << mv_info->fps_numerator << "/" << mv_info->fps_denominator << ")");
                DBGLOG("time: "             << mv_info->time << "sec (" << mv_info->numof_frames << "frames)");
                DBGLOG("color space: "      << mv_info->color_space);
                DBGLOG("bits per pixel: "   << mv_info->bpp);
                DBGLOG((mv_info->is_fieldbased ? (mv_info->is_tff ? "field based(top field first)" : "field based(bottom field first)") : "frame based"));
            }

        CVideo::~CVideo(void) {
            DBGLOG(FUNCNAME << "(void)");
        }
    }
}

