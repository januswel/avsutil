/*
 * items.hpp
 *  Definitions of items to show
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef ITEMS_HPP
#define ITEMS_HPP

namespace avsinfo {
    namespace items {
        // items to show, for video stream
        namespace video {
            enum item_t {
                WIDTH,
                HEIGHT,
                RATIO,
                FPS,
                FPS_FRACTION,
                VIDEO_TIME,
                FRAMES,
                COLOR_SPACE,
                BPP,
                INTERLACE_TYPE,
                FIELD_ORDER,

                num
            };

            namespace {
                const char* header[] = {
                    "width",
                    "height",
                    "ratio",
                    "FPS",
                    "FPS(fraction)",
                    "time of video",
                    "a number of frames",
                    "color space",
                    "bits per pixel",
                    "interlace type",
                    "field order",
                };

                const char* unit[] = {
                    "px",
                    "px",
                    "",
                    "",
                    "",
                    "sec",
                    "",
                    "",
                    "bits",
                    "",
                    "",
                };
            }
        }

        // items to show, for audio stream
        namespace audio {
            enum item_t {
                CHANNELS,
                BIT_DEPTH,
                SAMPLE_TYPE,
                AUDIO_TIME,
                SAMPLING_RATE,
                SAMPLES,
                BLOCK_SIZE,

                num
            };

            namespace {
                const char* header[] = {
                    "channels",
                    "bit depth",
                    "sample type",
                    "time of audio",
                    "sampling rate",
                    "a number of samples",
                    "block size",
                };

                const char* unit[] = {
                    "",
                    "bits",
                    "",
                    "sec",
                    "KHz",
                    "",
                    "bytes",
                };
            }
        }
    }
}

#endif // ITEMS_HPP

