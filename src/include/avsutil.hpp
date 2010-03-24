/*
 * avsutil.hpp
 *  Utility classes for AviSynth
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVSUTIL_HPP
#define AVSUTIL_HPP

#include <stdexcept>
#include <ostream>

namespace avsutil {
    // forward declarations
    class Avs;          // an interface class to handle AVS file
    class Video;        // an interface class to handle a video stream in AVS file
    struct VideoInfo;   // a struct that has informations about a video stream
    class Audio;        // an interface class to handle an audio stream in AVS file
    struct AudioInfo;   // a struct that has informations about an audio stream
    class AvsUtilError; // an exception class

    // These instantiate the object about AVS.
    // The returned pointer MUST be "delete"ed.
    Avs* CreateAvsObj(void);
    Avs* CreateAvsObj(const char* filename);   // with a filename

    /*
     * A class has basic features about AVS,
     * by wrapping classes defined in avisynth.h,
     * e.g. opening AVS file and getting audio informations.
     * */
    class Avs {
        public:
            // to open a file
            virtual void open(const char* avsfile) = 0;
            // get informations
            virtual const char* filename(void) = 0;
            virtual bool is_fine(void) = 0;
            virtual const char* errmsg(void) = 0;
            // get audio
            // the returned pointer MUST be "delete"ed
            virtual Video* video(void) = 0;
            // the returned pointer MUST be "delete"ed
            virtual Audio* audio(void) = 0;

            // typical destructor
            virtual ~Avs(void) {};
    };

    // for a video stream
    class Video {
        public:
            // get informations
            virtual const VideoInfo& info(void) = 0;

            // typical destructor
            virtual ~Video(void) {};
    };

    // informations of a video stream
    struct VideoInfo {
        // Enumerations for representation as color spaces (pixel type) by FOURCC
        // refer: http://www.fourcc.org/
        enum fourcc_t {
            UNKOWN,
            // In AviSynth, this denote data is represented as raw DIB
            // - Device Independent Bitmap that doesn't have pallette data
            // and bytes per pixel is 24 or 32.
            // This is an alias for BI_RGB
            RGB,
            // YUV 4:2:2 packed
            // byte ordering in a macropixel is YUYV.
            YUY2,
            // YUV 4:2:0 planar
            // 1 sample per 2x2 for V and U, plane order is YVU
            YV12,
            // YUV 4:2:0 planar
            // 1 sample per 2x2 for U and V, plane order is YUV.
            // a.k.a IYUV
            I420
        };

        bool exists;
        unsigned __int32 width;             // a number of horizontal pixels
        unsigned __int32 height;            // a number of scan lines
        double fps;                         // fps_numerator / fps_denominator
        double time;                        // numof_frames * fps_denominator / fps_numerator
        unsigned __int32 fps_numerator;
        unsigned __int32 fps_denominator;
        unsigned __int32 numof_frames;
        fourcc_t color_space;               // represented by FOURCC
        unsigned __int16 bpp;               // effective bit per pixcel
        bool is_fieldbased;                 // field-based if true, frame-based if false
        bool is_tff;                        // Top Field First if true, Bottom Field First if false
                                            // This should be evaluated only when is_fieldbased is true.
        // AviSynth doesn't contain this information (in 2.58)
        //bool is_progressive;              // progressive if true, interlaced if false
    };

    // for an audio stream
    class Audio {
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
            virtual ~Audio(void) {};
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

    // an exception that be thrown by functions in this namespace
    class AvsUtilError : public std::domain_error {
        public:
            AvsUtilError(const char* errmsg) : std::domain_error(errmsg) {};
    };

    // output audio data in the format of RIFF linear PCM
    std::ostream& operator <<(std::ostream&, const Audio&);
    std::ostream& operator <<(std::ostream&, const Audio* const);
};

#endif // AVSUTIL_HPP

