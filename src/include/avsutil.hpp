/*
 * avsutil.hpp
 *  Utility classes for AviSynth
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AVSUTIL_HPP
#define AVSUTIL_HPP

#include <istream>
#include <stdint.h>

namespace avsutil {
    // forward declarations
    struct manager_type;
    struct avs_type;
    struct video_type;
    struct audio_type;

    /*
     *  A function to start.
     *  Usage:
     *
     *      using namespace avsutil;
     *
     *      // note: binding by reference
     *      avs_type& avs = manager().load("funny_animal.avs");
     * */
    manager_type& manager(void);

    /*
     *  a class to manage AVS files
     * */
    struct manager_type {
        /*
         *  Reads the file that is located on "filepath" and returns the
         *  reference of avs_type.
         * */
        virtual avs_type& load(const char* filepath) = 0;

        /*
         *  Use this member function when you don't need an object of a class
         *  avs_type any longer and you are nervous about a space efficiency.
         * */
        virtual void unload(const avs_type& avs) = 0;

        // destructor
        virtual ~manager_type(void) {}
    };

    /*
     *  A class that has basic features about AVS, by wrapping classes defined
     *  in avisynth.h.  E.g.:
     *
     *      - opening AVS file
     *      - getting the object to get video informations / frame data
     *      - getting the object to get audio informations / samples
     * */
    struct avs_type {
        // Returns informations
        // a file path of specified AVS
        virtual const char* filepath(void) const = 0;
        // a status of an object
        virtual bool is_fine(void) const = 0;
        // Some messages to show causes of an error when is_fine() returns
        // false.
        virtual const char* errmsg(void) const = 0;

        // Returns the objects to treat video/audio.
        virtual video_type& video(void) = 0;
        virtual audio_type& audio(void) = 0;

        // destructor
        virtual ~avs_type(void) {}
    };

    // a class for a video
    struct video_type {
        // informations of a video
        struct info_type {
            /*
             *  Enumerations for representations as the color spaces (pixel
             *  type) by FOURCC
             *
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

            static const char* fourcc_name(fourcc_type color_space) {
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

        // Returns informations about a video.
        virtual const info_type& info(void) const = 0;
        // Returns a nth frame stream object.
        virtual std::istream& framestream(uint32_t n) = 0;
        virtual void release_framestream(std::istream& target) = 0;

        // destructor
        virtual ~video_type(void) {}
    };

    // a class for an audio
    struct audio_type {
        // informations of an audio
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

        // Returns informations about an audio.
        virtual const info_type& info(void) const = 0;
        // Returns input stream for audio samples.
        virtual std::istream& stream(void) = 0;

        // destructor
        virtual ~audio_type(void) {}
    };
};

#endif // AVSUTIL_HPP

