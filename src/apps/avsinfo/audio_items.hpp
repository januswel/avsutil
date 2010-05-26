/*
 * audio_items.hpp
 *  Declarations and definitions for classes of audio items
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef AUDIO_ITEMS_HPP
#define AUDIO_ITEMS_HPP

#include "../../include/avsutil.hpp"

#include "item.hpp"

namespace avsinfo {
    namespace items {
        namespace audio {
            // audio item enumerations
            enum item_type {
                CHANNELS,
                BIT_DEPTH,
                SAMPLE_TYPE,
                AUDIO_TIME,
                SAMPLING_RATE,
                SAMPLES,
                BLOCK_SIZE,

                NUMOF_ITEMS
            };

            // declarations and definitions for classes of items to show
            typedef basic_item<avsutil::AudioInfo, char> Item;

            class Channels : public Item {
                protected:
                    const char* header(void) const { return "channels"; }
                    const char* unit(void) const { return ""; }
                    string_type value(const avsutil::AudioInfo& ai) const {
                        if (is_human_friendly) {
                            switch (ai.channels) {
                                case 1:
                                    return string_type("mono");
                                case 2:
                                    return string_type("stereo");
                                case 6:
                                    return string_type("5.1ch");
                                default:
                                    return tconv().strfrom(ai.channels) + "ch";
                            }
                        }
                        else {
                            return tconv().strfrom(ai.channels);
                        }
                    }
            };

            class BitDepth : public Item {
                protected:
                    const char* header(void) const { return "bit depth"; }
                    const char* unit(void) const { return "bits"; }
                    string_type BitDepth::value(const avsutil::AudioInfo& ai) const {
                        return tconv().strfrom(ai.bit_depth);
                    }
            };

            class SampleType : public Item {
                protected:
                    const char* header(void) const { return "sample type"; }
                    const char* unit(void) const { return ""; }
                    string_type value(const avsutil::AudioInfo& ai) const {
                        return is_human_friendly
                            ? std::string(ai.is_int ? "int" : "float")
                            : tconv().strfrom(ai.is_int);
                    }
            };

            class AudioTime : public Item {
                protected:
                    const char* header(void) const { return "time of audio"; }
                    const char* unit(void) const { return "sec"; }
                    string_type value(const avsutil::AudioInfo& ai) const {
                        return tconv().strfrom(ai.time);
                    }
            };

            class SamplingRate : public Item {
                protected:
                    const char* header(void) const { return "sampling rate"; }
                    const char* unit(void) const { return "KHz"; }
                    string_type value(const avsutil::AudioInfo& ai) const {
                        return is_human_friendly
                            ? tconv().strfrom(static_cast<double>(ai.sampling_rate) / 1000)
                            : tconv().strfrom(ai.sampling_rate);
                    }
            };

            class Samples : public Item {
                protected:
                    const char* header(void) const { return "a number of samples"; }
                    const char* unit(void) const { return ""; }
                    string_type value(const avsutil::AudioInfo& ai) const {
                        return tconv().strfrom(ai.numof_samples);
                    }
            };

            class BlockSize : public Item {
                protected:
                    const char* header(void) const { return "block size"; }
                    const char* unit(void) const { return "bytes"; }
                    string_type value(const avsutil::AudioInfo& ai) const {
                        return tconv().strfrom(ai.block_size);
                    }
            };
        }
    }
}

#endif // AUDIO_ITEMS_HPP

