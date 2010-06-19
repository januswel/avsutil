/*
 * video_impl.cpp
 *  Definitions for cvideo_type class
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"

namespace avsutil {
    namespace impl {
        std::istream& cvideo_type::framestream(uint32_t n) {
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

        void cvideo_type::release_framestream(std::istream& target) {
            framestreams_type::iterator found =
                std::find(framestreams.begin(), framestreams.end(), &target);

            if (found != framestreams.end()) {
                delete *found;
                framestreams.erase(found);
            }
        }
    }
}

