/*
 * avs_impl.cpp
 *  Definitions for cavs_type class
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"

#include "../../helper/dlogger.hpp"

namespace avsutil {
    // implementations for functions
    manager_type& manager(void) {
        static impl::cmanager_type manager;
        return manager;
    }

    namespace impl {
        void cavs_type::open(const char* avsfile) {
            DBGLOG("avsutil::impl::cavs_type::open(\"" << avsfile << "\")");

            try {
                // pack the filename as the argument of AviSynth filter
                AVSValue filename = avsfile;
                AVSValue args = AVSValue(&filename, 1);

                // load AviSynth script
                AVSValue imported = mv_se->Invoke("Import", args, 0);

                // get the clip and video informations
                mv_clip = imported.AsClip();

                // store filename
                mv_filepath = avsfile;
            }
            catch (AvisynthError& avserr) {
                mv_is_fine = false;
                mv_errmsg = avserr.msg;
            }
            catch (std::exception& ex) {
                mv_is_fine = false;
                mv_errmsg = ex.what();
            }
        }

        /*
         *  An object of a class cavs_type has a possession of mv_se,
         *  cvideo_type and caudio_type objects are just allowed to borrow
         *  mv_se.
         * */
        video_type& cavs_type::video(void) {
            const VideoInfo vi = mv_clip->GetVideoInfo();
            const cvideo_type::info_type info = {
                vi.HasVideo(),
                vi.width,
                vi.height,
                static_cast<double>(vi.num_frames) * vi.fps_denominator
                    / vi.fps_numerator,
                static_cast<double>(vi.fps_numerator) / vi.fps_denominator,
                vi.fps_numerator,
                vi.fps_denominator,
                vi.num_frames,
                cvideo_type::fourcc(vi.pixel_type),
                vi.BitsPerPixel(),
                vi.IsFieldBased(),
                vi.IsTFF()
            };

            if (mv_video == NULL) mv_video =
                new cvideo_type(mv_clip, mv_se.get(), info);
            return *mv_video;
        }

        audio_type& cavs_type::audio(void) {
            const VideoInfo vi = mv_clip->GetVideoInfo();
            const caudio_type::info_type info = {
                vi.HasAudio(),
                vi.AudioChannels(),
                caudio_type::bit_depth(vi.sample_type),
                (vi.sample_type == SAMPLE_FLOAT ? false : true),
                static_cast<double>(vi.num_audio_samples)
                    / vi.SamplesPerSecond(),
                vi.SamplesPerSecond(),
                vi.num_audio_samples,
                vi.BytesPerAudioSample()
            };

            if (mv_audio == NULL) mv_audio =
                new caudio_type(mv_clip, mv_se.get(), info);
            return *mv_audio;
        }
    }
}

