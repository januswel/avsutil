/*
 * avsutil_impl.hpp
 *  Implementations for "avsutil.hpp"
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVSUTIL_IMPL_HPP
#define AVSUTIL_IMPL_HPP

#include "avisynth.h"
#include "../../include/avsutil.hpp"
#include <memory>
#include <string>
#include <ostream>

namespace avsutil {
    namespace impl {
        class CAvs : public Avs {
            private:
                // types defined by avisynth.h
                std::auto_ptr<IScriptEnvironment> mv_se;
                PClip mv_clip;

            private:
                // additional types
                bool mv_is_fine;
                std::string mv_filename;
                std::string mv_errmsg;

            private:
                // utilities
                const VideoInfo::fourcc_t fourcc(const int pixel_type) const;
                const unsigned int bitdepth(const int sample_type) const;

            public:
                // constructors and destructor
                CAvs(void);
                CAvs(const char*);
                ~CAvs(void);

                // implementations for interfaces
                void open(const char* avsfile);
                const char* filename(void) { return mv_filename.c_str(); }
                bool is_fine(void) { return mv_is_fine; }
                const char* errmsg(void) { return mv_errmsg.c_str(); }
                Video* video(void);
                Audio* audio(void);

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
                void audio_data(char* buf, const uint64_t start, const uint64_t count);
        };

        class CVideo: public Video {
            private:
                CAvs* mv_avs;
                std::auto_ptr<VideoInfo> mv_info;

            public:
                // constructor and destructor
                CVideo(CAvs* avs, std::auto_ptr<VideoInfo> info);
                ~CVideo(void);

                // implementations for interfaces
                const VideoInfo& info(void) { return *(mv_info.get()); }
        };

        class CAudio : public Audio {
            private:
                CAvs* mv_avs;
                std::auto_ptr<AudioInfo> mv_info;
                void (*mv_progress_callback)(const uint64_t, const uint64_t);
                uint32_t mv_buf_samples;
                static const uint64_t mv_buf_samples_default = 4096;

            private:
                // utilities
                void write_header(std::ostream&) const;
                void write_data(std::ostream&) const;

            public:
                // constructor and destructor
                CAudio(CAvs* avs, std::auto_ptr<AudioInfo> info);
                ~CAudio(void);

                // implementations for interfaces
                AudioInfo& info(void) { return *(mv_info.get()); }
                void write(std::ostream&) const;
                void progress_callback(void (*progress_callback)(const uint64_t, const uint64_t)) {
                    mv_progress_callback = progress_callback;
                }
                void buf_samples(const uint32_t buf_samples) {
                    mv_buf_samples = buf_samples;
                }
        };
    }
}

#endif // AVSUTIL_IMPL_HPP

