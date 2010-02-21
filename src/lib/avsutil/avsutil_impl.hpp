/*
 * avsutil_impl.hpp
 *  Implementations for "avsutil.hpp"
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVSUTIL_IMPL_HPP
#define AVSUTIL_IMPL_HPP

#include "../../avsutil.hpp"
#include <memory>
#include <string>
#include <ostream>

namespace avsutil {
    namespace impl {
        class CAudio;
        class CAvs : public IAvs {
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
                void pack_audioinfo(AudioInfo* ai);
                const unsigned int bitdepth(const VideoInfo& vi) const;

            public:
                // constructors and destructor
                CAvs(void);
                CAvs(const char*);
                ~CAvs(void);

                // implementations for interfaces
                void open(const char* avsfile);
                const char* filename(void) { return mv_filename.c_str(); };
                bool is_fine(void) { return mv_is_fine; }
                const char* errmsg(void) { return mv_errmsg.c_str(); };
                IAudio* audio(void);

                // utilities
                void getaudio(char* buf, const unsigned __int64 start, const unsigned __int64 count);
        };

        class CAudio : public IAudio {
            private:
                CAvs* mv_avs;
                std::auto_ptr<AudioInfo> mv_info;
                void (*mv_progress_callback)(const unsigned __int64, const unsigned __int64);
                unsigned __int32 mv_buf_samples;
                static const unsigned __int64 mv_buf_samples_default = 4096;

            private:
                // utilities
                void write_header(std::ostream&) const;
                void write_data(std::ostream&) const;

            public:
                // constructor and destructor
                CAudio(CAvs* avs, std::auto_ptr<AudioInfo> info);
                ~CAudio(void);

                // implementations for interfaces
                AudioInfo& info(void) { return *(mv_info.get()); };
                void write(std::ostream&) const;
                void progress_callback(void (*progress_callback)(const unsigned __int64, const unsigned __int64)) {
                    mv_progress_callback = progress_callback;
                };
                void buf_samples(const unsigned __int32 buf_samples) {
                    mv_buf_samples = buf_samples;
                };
        };
    }
}

#endif // AVSUTIL_IMPL_HPP
