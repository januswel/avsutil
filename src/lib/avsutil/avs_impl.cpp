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
    IAvs* CreateAvsObj(void) {
        DBGLOG(FUNCNAME << "(void)");
        return new impl::CAvs();
    }
    IAvs* CreateAvsObj(const char* filename) {
        DBGLOG(FUNCNAME << "(\"" << filename << "\")");
        return new impl::CAvs(filename);
    }

    namespace impl {
        CAvs::CAvs(void) : mv_se(CreateScriptEnvironment()), mv_is_fine(true) {
            DBGLOG(FUNCNAME << "(void)");
        };

        CAvs::CAvs(const char* avsfile) : mv_se(CreateScriptEnvironment()), mv_is_fine(true) {
            DBGLOG(FUNCNAME << "(const char*)");
            open(avsfile);
        };

        CAvs::~CAvs(void) {
            DBGLOG(FUNCNAME << "(void)");
        };

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

        void CAvs::getaudio(char* buf, const unsigned __int64 start, const unsigned __int64 count) {
            DBGLOG(FUNCNAME << "(char*, " << start << ", " << count << ")");
            mv_clip->GetAudio(buf, start, count, mv_se.get());
        }

        IVideo* CAvs::video(void) {
            DBGLOG(FUNCNAME << "(void)");

            // build a CVideo object
            std::auto_ptr<VideoInfo> pi(new VideoInfo); // pi has a possession of (VideoInfo*)
            pack_videoinfo(pi.get());                   // pi lends pack_videoinfo() (VideoInfo*)
            return new CVideo(this, pi);                // pi passes a possesion of (VideoInfo*) to CVideo()
        };

        void CAvs::pack_videoinfo(VideoInfo* pi) {
            DBGLOG(FUNCNAME << "(VideoInfo*)");
            ::VideoInfo vi = mv_clip->GetVideoInfo();

            pi->exists          = vi.HasVideo();
            pi->width           = vi.width;
            pi->height          = vi.height;
            pi->fps             = static_cast<double>(vi.fps_numerator) / vi.fps_denominator;
            pi->time            = static_cast<double>(vi.num_frames) * vi.fps_denominator / vi.fps_numerator;
            pi->fps_numerator   = vi.fps_numerator;
            pi->fps_denominator = vi.fps_denominator;
            pi->numof_frames    = vi.num_frames;
            pi->color_space     = get_fourcc(vi.pixel_type);
            pi->bpp             = vi.BitsPerPixel();
            pi->is_fieldbased   = vi.IsFieldBased();
            pi->is_tff          = vi.IsTFF();
        }

        VideoInfo::fourcc_t CAvs::get_fourcc(int pixel_type) {
            switch (pixel_type) {
                case ::VideoInfo::CS_BGR:   return VideoInfo::RGB;
                case ::VideoInfo::CS_YUY2:  return VideoInfo::YUY2;
                case ::VideoInfo::CS_YV12:  return VideoInfo::YV12;
                case ::VideoInfo::CS_I420:  return VideoInfo::I420;
                default:                    return VideoInfo::UNKOWN;
            }
        }

        IAudio* CAvs::audio(void) {
            DBGLOG(FUNCNAME << "(void)");

            // build a CAudio object
            std::auto_ptr<AudioInfo> ai(new AudioInfo); // ai has a possession of (AudioInfo*)
            pack_audioinfo(ai.get());                   // ai lends pack_audioinfo() (AudioInfo*)
            return new CAudio(this, ai);                // ai passes a possesion of (AudioInfo*) to CAudio()
        };

        void CAvs::pack_audioinfo(AudioInfo* ai) {
            DBGLOG(FUNCNAME << "(AudioInfo*)");
            ::VideoInfo vi = mv_clip->GetVideoInfo();
            ai->exists = vi.HasAudio();
            ai->samples = vi.num_audio_samples;
            ai->sampling_rate = vi.SamplesPerSecond();
            ai->channels = vi.AudioChannels();
            ai->bit_depth = bitdepth(vi);
            ai->block_size = vi.BytesPerAudioSample();
        }

        const unsigned int CAvs::bitdepth(const ::VideoInfo& vi) const {
            DBGLOG(FUNCNAME << "(const ::VideoInfo&)");
            switch (vi.sample_type) {
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

