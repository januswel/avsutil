/*
 * avsutil.hpp
 *  Utility classes for AviSynth
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVSUTIL_HPP
#define AVSUTIL_HPP

#include <ostream>
#include <stdexcept>
#include <stdint.h>

namespace avsutil {
    // forward declarations
    class video_type;
    class frame_type;
    class audio_type;

    /*
     * A class has basic features about AVS,
     * by wrapping classes defined in avisynth.h,
     * e.g. opening AVS file and getting audio informations.
     * */
    class avs_type {
        public:
            // to open a file
            virtual void open(const char*) = 0;
            // get informations
            virtual const char* filepath(void) const = 0;
            virtual bool is_fine(void) const = 0;
            virtual const char* errmsg(void) const = 0;

            virtual video_type* video(void) = 0;
            virtual audio_type* audio(void) = 0;

            // typical destructor
            virtual ~avs_type(void) {}
    };

    // These instantiate the object about AVS.
    // The returned pointer MUST be "delete"ed.
    avs_type* create_avs(void);
    avs_type* create_avs(const char* filename);

    // for a video stream
    class video_type {
        public:
            // informations of a video stream
            struct info_type {
                /*
                 *  Enumerations for representation as color spaces (pixel
                 *  type) by FOURCC
                 *      refer: http://www.fourcc.org/
                 * */
                enum fourcc_type {
                    UNKOWN,

                    /*
                     *  In AviSynth, this denote data is represented as raw DIB
                     *  - Device Independent Bitmap that doesn't have pallette
                     *  data and bytes per pixel is 24 or 32.
                     *  This is an alias for BI_RGB
                     * */
                    RGB,

                    /*
                     *  YUV 4:2:2 packed
                     *  byte ordering in a macropixel is YUYV.
                     * */
                    YUY2,

                    /*
                     *  YUV 4:2:0 planar
                     *  1 sample per 2x2 for V and U, plane order is YVU
                     * */
                    YV12,

                    /*
                     *  YUV 4:2:0 planar
                     *  1 sample per 2x2 for U and V, plane order is YUV.
                     *  a.k.a IYUV
                     * */
                    I420
                };

                const char* fourcc_name(void) const {
                    switch(color_space) {
                        case RGB:       return "RGB";
                        case YUY2:      return "YUY2";
                        case YV12:      return "YV12";
                        case I420:      return "I420";
                        case UNKOWN:
                        default:        return "unkown";
                    };
                }

                bool exists;
                uint32_t width;             // a number of horizontal pixels
                uint32_t height;            // a number of scan lines
                double time;                //   numof_frames * fps_denominator
                                            // / fps_numerator
                double fps;                 // fps_numerator / fps_denominator
                uint32_t fps_numerator;
                uint32_t fps_denominator;
                uint32_t numof_frames;      // a number of frames
                fourcc_type color_space;    // represented by FOURCC
                uint16_t bpp;               // effective bit per pixcel
                bool is_fieldbased;         // field-based if true
                                            // frame-based if false
                bool is_tff;                // Top Field First if true,
                                            // Bottom Field First if false
                /*
                 *  This should be evaluated only when is_fieldbased is true.
                 *  AviSynth doesn't contain this information (in 2.58)
                 * */
                //bool is_progressive;        // progressive if true
                                              // interlaced if false
            };

        public:
            // get informations
            virtual const info_type& info(void) const = 0;
            virtual frame_type* frame(uint32_t) = 0;

            // typical destructor
            virtual ~video_type(void) {}
    };

    // for a frame in video stream
    class frame_type {
        public:
            // informations of a frame
            struct info_type {
                uint16_t width;
                uint16_t pitch;
                uint16_t height;
            };

            // typical destructor
            virtual ~frame_type(void) {}

            // get informations
            virtual const info_type& info(void) const = 0;
            // output frame data in the format of 24bit Windows Bitmap
            virtual void write(std::ostream&) const = 0;
    };

    // output frame data in the format of 24bit Windows Bitmap
    std::ostream& operator <<(std::ostream&, const frame_type&);
    std::ostream& operator <<(std::ostream&, const frame_type* const);

    // for an audio stream
    class audio_type {
        public:
            // informations of audio stream
            struct info_type {
                bool exists;
                uint16_t channels;      // left, [right, [center, ...]]
                uint16_t bit_depth;     // a.k.a sample precision
                bool is_int;            // int if true, float if false
                double time;            // numof_samples / sampling_rate
                uint32_t sampling_rate; // a.k.a sampling frequency
                uint64_t numof_samples; // a number of samples
                uint16_t block_size;    // bytes per sample:
                                        // channels * (bit_depth / 8)
            };

            typedef void (*progress_callback_type)(
                    const uint64_t processed_samples,
                    const uint64_t total_samples);

            // get informations
            virtual const info_type& info(void) const = 0;
            // output audio data in the format of RIFF linear PCM
            virtual void write(std::ostream&) const = 0;
            // setter for the function to show progress
            virtual void progress_callback(progress_callback_type) = 0;
            // setter for a samples of a buffer
            // default is 4096
            // an actual buffer size is:
            //      min     buf_samples *  1 byte ( 8bit, mono)
            //      max     buf_samples * 24 byte (32bit, 5.1ch)
            virtual void buf_samples(const uint32_t) = 0;

            // typical destructor
            virtual ~audio_type(void) {}
    };

    // output audio data in the format of RIFF linear PCM
    std::ostream& operator <<(std::ostream&, const audio_type&);
    std::ostream& operator <<(std::ostream&, const audio_type* const);
};

#endif // AVSUTIL_HPP

