/*
 * video_impl.cpp
 *  Definitions for cvideo_type class
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"

#include "../../helper/dlogger.hpp"

namespace avsutil {
    namespace impl {
        cvideo_type::cvideo_type(PClip clip, IScriptEnvironment* se)
            : mv_clip(clip), mv_rgb_clip(clip), mv_se(se) {
                DBGLOG( "avsutil::impl::cvideo_type::"
                        "cvideo_type(Pclip, IScriptEnvironment*)");

                const VideoInfo vi = mv_clip->GetVideoInfo();
                info_type info = {
                    vi.HasVideo(),
                    vi.width,
                    vi.height,
                    static_cast<double>(vi.num_frames) * vi.fps_denominator
                        / vi.fps_numerator,
                    static_cast<double>(vi.fps_numerator) / vi.fps_denominator,
                    vi.fps_numerator,
                    vi.fps_denominator,
                    vi.num_frames,
                    fourcc(vi.pixel_type),
                    vi.BitsPerPixel(),
                    vi.IsFieldBased(),
                    vi.IsTFF()
                };
                mv_info = info;

                DBGLOG( "\n"
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
    }
}

