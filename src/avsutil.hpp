/*
 * avsutil.h
 *  utility classes for AviSynth
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 *
 *      AudioInfo
 *      Avs
 *      FileWriter <- AudioWriter <- WavWriter
 *
 * refer
 *  RIFF WAV specifications
 *      http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
 *
 * */

#ifndef AVSUTIL
#define AVSUTIL

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

    /*
     * A class has basic features about avs,
     * by wrapping classes defined in avisynth.h,
     * e.g. opening avs file and getting audio informations.
     * */
    class Avs {
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
            Avs() {};
            Avs(const std::string& avsfile) { open(avsfile); };
            virtual ~Avs() {};
            void open(const std::string& avsfile);
            const AudioInfo& get_audioinfo() const { return ai; };
            void get_audio(void* buf, const __int64 start, const __int64 count) { clip->GetAudio(buf, start, count, se); };
    };

    /*
     * An ancestor class for writing file.
     * This is an abstract class.
     * */
    class FileWriter {
        public:
            virtual bool write(const std::string& wavfile, void (*progress)(const unsigned __int64, const unsigned __int64)) = 0;
    };

    /*
     * A super class for writing audio file.
     * This is abstract class.
     * In order to make the class to write audio file in your favorite format,
     * implement write_header(), write_footer() and write_data(),
     * in your sub classes for this.
     * */
    class AudioWriter : public FileWriter {
        protected:
            FILE* audiofp;    // file pointer for the file
            Avs avs;

            virtual void write_header() = 0;
            virtual void write_footer() = 0;
            virtual void write_data(void (*progress)(const unsigned __int64, const unsigned __int64)) = 0;

        public:
            AudioWriter() {};
            AudioWriter(const std::string& avsfile) : avs(avsfile) {};
            virtual ~AudioWriter() {};

            const Avs& get_avs() const { return avs; };
            void open(const std::string& avsfile) { avs.open(avsfile); };
            virtual bool write(const std::string& wavfile, void (*progress)(const unsigned __int64, const unsigned __int64) = NULL);
    };

    class WavWriter : public AudioWriter {
        private:
            // constants
            static const unsigned __int16 header_size;
            static const unsigned __int16 wave_header_size;
            static const unsigned __int16 fmt_chunk_size;
            static const char* header_riff;
            static const char* header_wave;
            static const char* header_fmt;
            static const char* header_data;
            enum {PCM = 1};     // only linear PCM format for now

            // default of buf_samples
            //  actual buffer size is:
            //      min     buf_samples *  1 byte ( 8bit, mono)
            //      max     buf_samples * 24 byte (32bit, 5.1ch)
            static const unsigned __int32 buf_samples_def = 0x1000;

            // a number of samples that processed at one time
            unsigned int buf_samples;

        protected:
            virtual void write_header();
            virtual void write_footer() {};
            virtual void write_data(void (*progress)(const unsigned __int64, const unsigned __int64));

        public:
            WavWriter(const unsigned int n = buf_samples_def)
                : buf_samples(n) {};
            WavWriter(const std::string& avsfile, const unsigned int n = buf_samples_def)
                : buf_samples(n), AudioWriter(avsfile) {};
            virtual ~WavWriter() {};
    };

    class AvsUtilError : public std::domain_error {
        public:
            AvsUtilError(const std::string& errmsg) : std::domain_error(errmsg) {};
    };
};

#endif
