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
        frame_type& cvideo_type::frame(uint32_t n) {
            if (!mv_rgb_clip->GetVideoInfo().IsRGB24()) {
                DBGLOG("convert to RGB24");
                AVSValue clip = mv_rgb_clip;
                AVSValue args = AVSValue(&clip, 1);
                AVSValue converted =
                    mv_se->Invoke("ConvertToRGB24", args);
                mv_rgb_clip = converted.AsClip();
            }

            cframes_type::iterator found =
                std::find_if(
                        cframes.begin(), cframes.end(),
                        std::bind2nd(std::mem_fun(
                                &avsutil::impl::cframe_type::is_me
                                ), n));

            // found
            if (found != cframes.end()) return **found;

            // not found and create
            PVideoFrame frame = mv_rgb_clip->GetFrame(n, mv_se);
            const cframe_type::info_type info = {
                frame->GetRowSize() / 3,
                frame->GetPitch(),
                frame->GetHeight(),
                n
            };
            cframe_type* created = new cframe_type(frame , this, info);
            cframes.push_back(created);
            return *created;
        }

        void cvideo_type::release_frame(const cframe_type* const target) {
            cframes_type::iterator found =
                std::find(cframes.begin(), cframes.end(), target);

            if (found != cframes.end()) {
                delete *found;
                cframes.erase(found);
            }
        }
    }
}

