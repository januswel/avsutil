/*
 * avs_impl.cpp
 *  Definitions for CAvs class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"

#ifdef _DEBUG
#include <iostream>
#endif

namespace avsutil {
    // implementations for functions
    IAvs* CreateAvsObj(void) { return new impl::CAvs(); }
    IAvs* CreateAvsObj(const char* filename) {
        return new impl::CAvs(filename);
    }

    namespace impl {
        CAvs::CAvs(void) : mv_se(CreateScriptEnvironment()), mv_is_fine(true) {
#ifdef _DEBUG
            std::cerr << "avsutil::impl::CAvs::CAvs(void)" << std::endl;
#endif
        };

        CAvs::CAvs(const char* avsfile) : mv_se(CreateScriptEnvironment()), mv_is_fine(true) {
#ifdef _DEBUG
            std::cerr << "avsutil::impl::CAvs::CAvs(const char*)" << std::endl;
#endif
            open(avsfile);
        };

        CAvs::~CAvs(void) {
#ifdef _DEBUG
            std::cerr << "avsutil::impl::CAvs::~CAvs(void)" << std::endl;
#endif
        };

        void CAvs::open(const char* avsfile) {
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
                mv_errmsg = std::string("Specified file is not valid for AviSynth.\n") + avserr.msg;
            }
            catch (std::exception& ex) {
                mv_is_fine = false;
                mv_errmsg = ex.what();
            }
        }

        void CAvs::getaudio(char* buf, const unsigned __int64 start, const unsigned __int64 count) {
#ifdef _DEBUG
            std::cerr << "avsutil::impl::CAvs::getaudio(char*, " << start << ", " << count << ")" << std::endl;
#endif
            mv_clip->GetAudio(buf, start, count, mv_se.get());
        }

        IAudio* CAvs::audio(void) {
            // build a CAudio object
            std::auto_ptr<AudioInfo> ai(new AudioInfo); // ai has a possession of (AudioInfo*)
            pack_audioinfo(ai.get());                   // ai lends pack_audioinfo() (AudioInfo*)
            return new CAudio(this, ai);                // ai passes a possesion of (AudioInfo*) to CAudio()
        };

        void CAvs::pack_audioinfo(AudioInfo* ai) {
            VideoInfo vi = mv_clip->GetVideoInfo();
            ai->exists = vi.HasAudio();
            ai->samples = vi.num_audio_samples;
            ai->sampling_rate = vi.SamplesPerSecond();
            ai->channels = vi.AudioChannels();
            ai->bit_depth = bitdepth(vi);
            ai->block_size = vi.BytesPerAudioSample();
        }

        const unsigned int CAvs::bitdepth(const VideoInfo& vi) const {
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
                    throw std::range_error("unknown sample type");
            }
        }
    }
}
