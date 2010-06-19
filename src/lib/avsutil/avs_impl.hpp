/*
 * avs_impl.hpp
 *  Declarations and definitions for a class cavs_type
 *
 *  written by janus_wel<janus.wel.3@gmail.com>
 *  This source code is in public domain, and has NO WARRANTY.
 * */

#ifndef AVS_IMPL_HPP
#define AVS_IMPL_HPP

#include "../../include/avsutil.hpp"

#include "avisynth.h"

#include "video_impl.hpp"
#include "audio_impl.hpp"

#include <memory>
#include <string>

#include "../../helper/dlogger.hpp"

namespace avsutil {
    namespace impl {
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
                : mv_se(CreateScriptEnvironment()), mv_is_fine(true),
                  mv_video(NULL), mv_audio(NULL) {
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

                /*
                 *  An object of a class cavs_type has a possession of mv_se,
                 *  cvideo_type and caudio_type objects are just allowed to borrow
                 *  mv_se.
                 * */
                video_type& video(void) {
                    const VideoInfo& vi = mv_clip->GetVideoInfo();
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

                audio_type& audio(void) {
                    const VideoInfo& vi = mv_clip->GetVideoInfo();
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

            public:
                // utility functions
                // Opens AVS file and sets some member variables.
                void open(const char* avsfile) {
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

                // to identify
                bool is_me(const char* filepath) const {
                    return (mv_filepath.compare(filepath) == 0);
                }
        };
    }
}

#endif // AVS_IMPL_HPP

