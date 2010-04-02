/*
 * items.cpp
 *  Definitions for items.hpp
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "items.hpp"
#include "../../helper/gcd.hpp"

namespace avsinfo {
    namespace items {
        const unsigned int item_traits::header_size = 20;
        const bool item_traits::is_alignment = true;
        const bool item_traits::is_preceding_delimiter = false;
        const char* item_traits::delimiter(void) { return ": "; }
        const bool item_traits::is_unit = true;

        namespace video {
            // Width
            const char* Width::header(void) const { return "width"; }
            const char* Width::unit(void) const { return "px"; }
            const std::string Width::value(const avsutil::VideoInfo& vi) const {
                return tconv().strfrom(vi.width);
            }

            // Height
            const char* Height::header(void) const { return "height"; }
            const char* Height::unit(void) const { return "px"; }
            const std::string Height::value(const avsutil::VideoInfo& vi) const {
                return tconv().strfrom(vi.height);
            }

            // Ratio
            const char* Ratio::header(void) const { return "ratio"; }
            const char* Ratio::unit(void) const { return ""; }
            const std::string Ratio::value(const avsutil::VideoInfo& vi) const {
                const unsigned int gcd = util::math::calc_gcd(vi.width, vi.height);
                return tconv().strfrom(vi.width/gcd) + ':' + tconv().strfrom(vi.height/gcd);
            }

            // Fps
            const char* Fps::header(void) const { return "FPS"; }
            const char* Fps::unit(void) const { return ""; }
            const std::string Fps::value(const avsutil::VideoInfo& vi) const {
                return tconv().strfrom(vi.fps);
            }

            // FpsFraction
            const char* FpsFraction::header(void) const { return "FPS(fraction)"; }
            const char* FpsFraction::unit(void) const { return ""; }
            const std::string FpsFraction::value(const avsutil::VideoInfo& vi) const {
                return tconv().strfrom(vi.fps_numerator) + '/' + tconv().strfrom(vi.fps_denominator);
            }

            // VideoTime
            const char* VideoTime::header(void) const { return "time of video"; }
            const char* VideoTime::unit(void) const { return "sec"; }
            const std::string VideoTime::value(const avsutil::VideoInfo& vi) const {
                return tconv().strfrom(vi.time);
            }

            // Frames
            const char* Frames::header(void) const { return "a number of frames"; }
            const char* Frames::unit(void) const { return ""; }
            const std::string Frames::value(const avsutil::VideoInfo& vi) const {
                return tconv().strfrom(vi.numof_frames);
            }

            // ColorSpace
            const char* ColorSpace::header(void) const { return "color space"; }
            const char* ColorSpace::unit(void) const { return ""; }
            const std::string ColorSpace::value(const avsutil::VideoInfo& vi) const {
                return std::string(avsutil::VideoInfo::fourcc_name(vi.color_space));
            }

            // Bpp
            const char* Bpp::header(void) const { return "bits per pixel"; }
            const char* Bpp::unit(void) const { return "bits"; }
            const std::string Bpp::value(const avsutil::VideoInfo& vi) const {
                return tconv().strfrom(vi.bpp);
            }

            // InterlaceType
            const char* InterlaceType::header(void) const { return "interlace type"; }
            const char* InterlaceType::unit(void) const { return ""; }
            const std::string InterlaceType::value(const avsutil::VideoInfo& vi) const {
                return is_human_friendly
                    ? std::string(vi.is_fieldbased ? "field based" : "frame based")
                    : tconv().strfrom(vi.is_fieldbased);
            }

            // FieldOrder
            const char* FieldOrder::header(void) const { return "field order"; }
            const char* FieldOrder::unit(void) const { return ""; }
            const std::string FieldOrder::value(const avsutil::VideoInfo& vi) const {
                return is_human_friendly
                    ? std::string(vi.is_tff ? "top field first" : "bottom field first")
                    : tconv().strfrom(vi.is_tff);
            }

            void add_all_items(VideoItems& items) {
                items.add_item(WIDTH)
                    .add_item(HEIGHT)
                    .add_item(RATIO)
                    .add_item(FPS)
                    .add_item(FPS_FRACTION)
                    .add_item(VIDEO_TIME)
                    .add_item(FRAMES)
                    .add_item(COLOR_SPACE)
                    .add_item(BPP)
                    .add_item(INTERLACE_TYPE)
                    .add_item(FIELD_ORDER);
            }
        }

        namespace audio {
            // Channels
            const char* Channels::header(void) const { return "channels"; }
            const char* Channels::unit(void) const { return ""; }
            const std::string Channels::value(const avsutil::AudioInfo& ai) const {
                if (is_human_friendly) {
                    switch (ai.channels) {
                        case 1:
                            return std::string("mono");
                        case 2:
                            return std::string("stereo");
                        case 6:
                            return std::string("5.1ch");
                        default:
                            return tconv().strfrom(ai.channels) + "ch";
                    }
                }
                else {
                    return tconv().strfrom(ai.channels);
                }
            }

            // BitDepth
            const char* BitDepth::header(void) const { return "bit depth"; }
            const char* BitDepth::unit(void) const { return "bits"; }
            const std::string BitDepth::value(const avsutil::AudioInfo& ai) const {
                return tconv().strfrom(ai.bit_depth);
            }

            // SampleType
            const char* SampleType::header(void) const { return "sample type"; }
            const char* SampleType::unit(void) const { return ""; }
            const std::string SampleType::value(const avsutil::AudioInfo& ai) const {
                return is_human_friendly
                    ? std::string(ai.is_int ? "int" : "float")
                    : tconv().strfrom(ai.is_int);
            }

            // AudioTime
            const char* AudioTime::header(void) const { return "time of audio"; }
            const char* AudioTime::unit(void) const { return "sec"; }
            const std::string AudioTime::value(const avsutil::AudioInfo& ai) const {
                return tconv().strfrom(ai.time);
            }

            // SamplingRate
            const char* SamplingRate::header(void) const { return "sampling rate"; }
            const char* SamplingRate::unit(void) const { return "KHz"; }
            const std::string SamplingRate::value(const avsutil::AudioInfo& ai) const {
                return is_human_friendly
                    ? tconv().strfrom(static_cast<double>(ai.sampling_rate) / 1000)
                    : tconv().strfrom(ai.sampling_rate);
            }

            // Samples
            const char* Samples::header(void) const { return "a number of samples"; }
            const char* Samples::unit(void) const { return ""; }
            const std::string Samples::value(const avsutil::AudioInfo& ai) const {
                return tconv().strfrom(ai.numof_samples);
            }

            // BlockSize
            const char* BlockSize::header(void) const { return "block size"; }
            const char* BlockSize::unit(void) const { return "bytes"; }
            const std::string BlockSize::value(const avsutil::AudioInfo& ai) const {
                return tconv().strfrom(ai.block_size);
            }

            void add_all_items(AudioItems& items) {
                items.add_item(CHANNELS)
                    .add_item(BIT_DEPTH)
                    .add_item(SAMPLE_TYPE)
                    .add_item(AUDIO_TIME)
                    .add_item(SAMPLING_RATE)
                    .add_item(SAMPLES)
                    .add_item(BLOCK_SIZE);
            }
        }
    }
}

