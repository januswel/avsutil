/*
 * avsutil.hpp
 *  Utility classes for AviSynth
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 *
 *      IAvs:           An interface class to handle avs file
 *      IAudio:         An interface class to handle an audio stream in avs file
 *      AudioInfo:      A struct that has informations about audio stream
 *      AvsUtilError:   An exception class
 * */

#ifndef AVSUTIL_HPP
#define AVSUTIL_HPP

#include <stdexcept>
#include <ostream>

namespace avsutil {
    class IAvs;
    class IAudio;
    struct AudioInfo;

    // instantiate the object about avs
    // the returned pointer MUST be "delete"ed
    IAvs* CreateAvsObj(void);
    IAvs* CreateAvsObj(const char* filename);   // with a filename

    /*
     * A class has basic features about avs,
     * by wrapping classes defined in avisynth.h,
     * e.g. opening avs file and getting audio informations.
     * */
    class IAvs {
        public:
            // to open a file
            virtual void open(const char* avsfile) = 0;
            // get informations
            virtual const char* filename(void) = 0;
            virtual bool is_fine(void) = 0;
            virtual const char* errmsg(void) = 0;
            // get audio
            // the returned pointer MUST be "delete"ed
            virtual IAudio* audio(void) = 0;

            // typical destructor
            virtual ~IAvs(void) {};
    };

    class IAudio {
        public:
            // get informations
            virtual const AudioInfo& info(void) = 0;
            // output audio data in the format of RIFF linear PCM
            virtual void write(std::ostream&) const = 0;
            // setter for the function to show progress
            virtual void progress_callback(void (*progress_callback)(const unsigned __int64 processed_samples, const unsigned __int64 total_samples)) = 0;
            // setter for a samples of a buffer
            // default is 4096
            // an actual buffer size is:
            //      min     buf_samples *  1 byte ( 8bit, mono)
            //      max     buf_samples * 24 byte (32bit, 5.1ch)
            virtual void buf_samples(const unsigned __int32) = 0;

            // typical destructor
            virtual ~IAudio(void) {};
    };

    // informations of audio stream
    struct AudioInfo {
        bool exists;
        unsigned __int64 samples;       // number of samples
        unsigned __int32 sampling_rate; // a.k.a sampling frequency
        unsigned __int16 channels;      // left, [right, [center, ...]]
        unsigned __int16 bit_depth;     // a.k.a sample precision
        unsigned __int16 block_size;    // channels * (bit_depth / 8)
    };

    class AvsUtilError : public std::domain_error {
        public:
            AvsUtilError(const char* errmsg) : std::domain_error(errmsg) {};
    };

    // output audio data in the format of RIFF linear PCM
    std::ostream& operator <<(std::ostream&, const IAudio&);
    std::ostream& operator <<(std::ostream&, const IAudio* const);
};



#endif // AVSUTIL_HPP
