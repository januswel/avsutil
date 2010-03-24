/*
 * avs_impl.cpp
 *  Definitions for CAvs class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"
#include "../debuglogger/debuglogger.hpp"

namespace avsutil {
    // implementations for functions
    Avs* CreateAvsObj(void) {
        DBGLOG(FUNCNAME << "(void)");

        return new impl::CAvs();
    }
    Avs* CreateAvsObj(const char* filename) {
        DBGLOG(FUNCNAME << "(\"" << filename << "\")");

        return new impl::CAvs(filename);
    }

    namespace impl {
        CAvs::CAvs(void) : mv_se(CreateScriptEnvironment()), mv_is_fine(true) {
            DBGLOG(FUNCNAME << "(void)");
        }

        CAvs::CAvs(const char* avsfile) : mv_se(CreateScriptEnvironment()), mv_is_fine(true) {
            DBGLOG(FUNCNAME << "(const char*)");

            open(avsfile);
        }

        CAvs::~CAvs(void) {
            DBGLOG(FUNCNAME << "(void)");
        }

        void CAvs::open(const char* avsfile) {
            DBGLOG(FUNCNAME << "(\"" << avsfile << "\")");

            try {
                // pack the filename as the argument of AviSynth filter
                AVSValue filename = avsfile;
                AVSValue args = AVSValue(&filename, 1);

                // load AviSynth script
                AVSValue imported = mv_se->Invoke("Import", args, 0);

                // get the clip and video informations
                mv_clip = imported.AsClip();

                // store filename
                mv_filename.assign(avsfile);
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

        void CAvs::audio_data(char* buf, const unsigned __int64 start, const unsigned __int64 count) {
            DBGLOG(FUNCNAME << "(char*, " << start << ", " << count << ")");

            mv_clip->GetAudio(buf, start, count, mv_se.get());
        }

        Video* CAvs::video(void) {
            DBGLOG(FUNCNAME << "(void)");

            ::VideoInfo vi = mv_clip->GetVideoInfo();

            // build a CVideo object
            std::auto_ptr<VideoInfo> pi(new VideoInfo(
                        vi.HasVideo(),
                        vi.width,
                        vi.height,
                        static_cast<double>(vi.fps_numerator) / vi.fps_denominator,
                        static_cast<double>(vi.num_frames) * vi.fps_denominator / vi.fps_numerator,
                        vi.fps_numerator,
                        vi.fps_denominator,
                        vi.num_frames,
                        fourcc(vi.pixel_type),
                        vi.BitsPerPixel(),
                        vi.IsFieldBased(),
                        vi.IsTFF()
                        ));

            return new CVideo(this, pi);
        }

        const VideoInfo::fourcc_t CAvs::fourcc(const int pixel_type) const {
            switch (pixel_type) {
                case ::VideoInfo::CS_BGR:   return VideoInfo::RGB;
                case ::VideoInfo::CS_YUY2:  return VideoInfo::YUY2;
                case ::VideoInfo::CS_YV12:  return VideoInfo::YV12;
                case ::VideoInfo::CS_I420:  return VideoInfo::I420;
                default:                    return VideoInfo::UNKOWN;
            }
        }

        Audio* CAvs::audio(void) {
            DBGLOG(FUNCNAME << "(void)");

            ::VideoInfo vi = mv_clip->GetVideoInfo();

            // build a CAudio object
            std::auto_ptr<AudioInfo> ai(new AudioInfo(
                        vi.HasAudio(),
                        vi.num_audio_samples,
                        vi.SamplesPerSecond(),
                        vi.AudioChannels(),
                        bitdepth(vi.sample_type),
                        vi.BytesPerAudioSample()
                        ));

            return new CAudio(this, ai);
        }

        const unsigned int CAvs::bitdepth(const int sample_type) const {
            DBGLOG(FUNCNAME << "(const ::VideoInfo&)");

            switch (sample_type) {
                case SAMPLE_INT8:
                    return 8;
                case SAMPLE_INT16:
                    return 16;
                case SAMPLE_INT24:
                    return 24;
                case SAMPLE_INT32:
                    return 32;
                case SAMPLE_FLOAT:
                    return sizeof(float) * 8;
                default:
                    return 0;
            }
        }
    }
}

