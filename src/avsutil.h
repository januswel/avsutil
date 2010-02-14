/*
 * avsutil.h
 *  utility classes for AviSynth
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 *
 *      AvsUtil <- Avs2File <- Avs2Audio <- Avs2Wav
 *
 * refer
 *  RIFF WAV specifications
 *      http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
 *
 * */

#ifndef AVS2WAV
#define AVS2WAV

#include "avisynth.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <stdio.h>

namespace avsutil {
    // informations of audio stream
    struct AudioInfo {
        bool exists;
        unsigned __int64 samples;       // number of samples
        unsigned __int32 sampling_rate; // a.k.a sampling frequency
        unsigned __int16 channels;      // left, [right, [center, ...]]
        unsigned __int16 bit_depth;     // a.k.a sample precision
        unsigned __int16 block_size;    // channels * (bit_depth / 8)
    };

    class AvsUtil {
        protected:
            IScriptEnvironment* se;
            PClip clip;
            VideoInfo vi;
            AudioInfo ai;

            void pack_audio_info() {
                ai.exists        = vi.HasAudio();
                ai.samples       = vi.num_audio_samples;
                ai.sampling_rate = vi.SamplesPerSecond();
                ai.channels      = vi.AudioChannels();
                ai.bit_depth     = get_bit_depth();
                ai.block_size    = vi.BytesPerAudioSample();
            };

            unsigned int get_bit_depth() const {
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
            };

        public:
            AvsUtil() {};
            AvsUtil(const std::string& avsfile) { open(avsfile); };
            virtual ~AvsUtil() {};
            void open(const std::string& avsfile);
            const AudioInfo& get_audioinfo() const { return ai; };
    };

    class Avs2File : public AvsUtil {
        protected:
            FILE* wavfp;                // file pointer for the wav file

        public:
            Avs2File() : AvsUtil() {};
            Avs2File(const std::string& avsfile) : AvsUtil(avsfile) {};
            virtual ~Avs2File() {};
            virtual bool write(const std::string& wavfile) = 0;
    };

    class Avs2Audio : public Avs2File {
        protected:
            static const unsigned __int32 buf_samples_def = 1024;

            // a number of samples that processed at one time
            unsigned int buf_samples;

            virtual void write_header() {};
            virtual void write_data() {};
            virtual void write_footer() {};

        public:
            // build the object ScriptEnvironment
            Avs2Audio(const unsigned int n = buf_samples_def)
                : buf_samples(n) {};
            Avs2Audio(const std::string& avsfile, const unsigned int n = buf_samples_def)
                : Avs2File(avsfile), buf_samples(n) {};
            virtual ~Avs2Audio() {};

            virtual bool write(const std::string& wavfile);
    };

    class Avs2Wav : public Avs2Audio {
        private:
            static const unsigned __int16 header_size;
            static const unsigned __int16 wave_header_size;
            static const unsigned __int16 fmt_chunk_size;
            static const char* header_riff;
            static const char* header_wave;
            static const char* header_fmt;
            static const char* header_data;
            enum {PCM = 1};

        protected:
            virtual void write_header();
            virtual void write_data();

        public:
            // build the object ScriptEnvironment
            Avs2Wav(const unsigned int n = buf_samples_def)
                : Avs2Audio(n) {};
            Avs2Wav(const std::string& avsfile, const unsigned int n = buf_samples_def)
                : Avs2Audio(avsfile, n) {};
            virtual ~Avs2Wav() {};
    };

    class Avs2FileError : public std::domain_error {
        public:
            Avs2FileError(const std::string& errmsg) : std::domain_error(errmsg) {};
    };
};

#endif
