/*
 * items.hpp
 *  Declarations and definitions for the classes to output item informations
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef ITEMS_HPP
#define ITEMS_HPP

#include "video_items.hpp"
#include "audio_items.hpp"

#include "../../include/avsutil.hpp"

#include "../../helper/observer.hpp"
#include "../../helper/algorithm.hpp"

#include <bitset>
#include <list>
#include <ostream>
#include <stdexcept>

namespace avsinfo {
    namespace items {
        // infomation traits
        template<typename Info> struct info_traits;
        // for video
        template<> struct info_traits<avsutil::VideoInfo> {
            typedef avsutil::VideoInfo  info_type;
            typedef video::Item         item_type;
            typedef video::item_type    itemkind_type;

            static const unsigned int numof_items = video::NUMOF_ITEMS;
            static item_type* create_item(itemkind_type item) {
                switch (item) {
                    case video::WIDTH:          return new video::Width;
                    case video::HEIGHT:         return new video::Height;
                    case video::RATIO:          return new video::Ratio;
                    case video::FPS:            return new video::Fps;
                    case video::FPS_FRACTION:   return new video::FpsFraction;
                    case video::VIDEO_TIME:     return new video::VideoTime;
                    case video::FRAMES:         return new video::Frames;
                    case video::COLOR_SPACE:    return new video::ColorSpace;
                    case video::BPP:            return new video::Bpp;
                    case video::INTERLACE_TYPE: return
                                                new video::InterlaceType;
                    case video::FIELD_ORDER:    return new video::FieldOrder;

                    default: throw std::logic_error("unknown video item type");
                }
            }
        };
        // for audio
        template<> struct info_traits<avsutil::AudioInfo> {
            typedef avsutil::AudioInfo  info_type;
            typedef audio::Item         item_type;
            typedef audio::item_type    itemkind_type;

            static const unsigned int numof_items = audio::NUMOF_ITEMS;
            static item_type* create_item(itemkind_type item) {
                switch (item) {
                    case audio::CHANNELS:       return new audio::Channels;
                    case audio::BIT_DEPTH:      return new audio::BitDepth;
                    case audio::SAMPLE_TYPE:    return new audio::SampleType;
                    case audio::AUDIO_TIME:     return new audio::AudioTime;
                    case audio::SAMPLING_RATE:  return new audio::SamplingRate;
                    case audio::SAMPLES:        return new audio::Samples;
                    case audio::BLOCK_SIZE:     return new audio::BlockSize;

                    default: throw std::logic_error("unknown audio item type");
                }
            }
        };

        // human-friendly or machine-friendly
        class Notation : public pattern::observer::basic_subject<bool> {
            private:
                // status
                bool is_human_friendly;

            protected:
                // implementations for virtual function of the super class
                // pattern::observer::basic_subject
                const state_type& subject_state(void) const {
                    return is_human_friendly;
                }

            public:
                // constructor
                explicit Notation(bool is_human_friendly = true)
                    : is_human_friendly(is_human_friendly) { notify_state(); }

                // setter
                void human_friendly(void) {
                    is_human_friendly = true;
                    notify_state();
                }
                void machine_friendly(void) {
                    is_human_friendly = false;
                    notify_state();
                }
        };

        // the base class to represent items that should be outputed
        // This class also contains the order to show.
        template<typename Info, typename InfoTraits = info_traits<Info> >
        class basic_items {
            private:
                typedef Info                                info_type;
                typedef typename InfoTraits::item_type      item_type;
                typedef std::list<item_type*>               item_array_type;
                typedef typename InfoTraits::itemkind_type  itemkind_type;

            private:
                // member variables
                // items to show, and showing order
                item_array_type items;
                // flags to check dupricative assignment
                std::bitset<InfoTraits::numof_items> specified_flag;
                // human-friendly or machine-friendly
                Notation mv_notation;

            public:
                ~basic_items(void) {
                    std::for_each(
                            items.begin(), items.end(),
                            util::algorithm::sweeper());
                }
                // add an item to show at the end of the order if still not
                // specified.
                basic_items& add_item(itemkind_type itemkind) {
                    if (!specified_flag[itemkind]) {
                        item_type* new_item =
                            InfoTraits::create_item(itemkind);
                        items.push_back(new_item);
                        mv_notation.attach_observer(new_item);
                        specified_flag.set(itemkind);
                    }
                    return *this;
                }

                // setter for status
                basic_items& notation(bool is_human_friendly) {
                    is_human_friendly
                        ? mv_notation.human_friendly()
                        : mv_notation.machine_friendly();
                    return *this;
                }

                bool empty(void) { return items.empty(); }

                // output contents
                template<typename Char>
                void
                output(std::basic_ostream<Char>& out, const info_type& info) {
                    util::algorithm::print_op<std::basic_string<Char> >(
                            items.begin(), items.end(),
                            out, "\n",
                            std::bind2nd(
                                std::mem_fun(&item_type::to_string), info));
                }
            };

        typedef basic_items<avsutil::VideoInfo> VideoItems;
        typedef basic_items<avsutil::AudioInfo> AudioItems;

        void add_all_video_items(VideoItems& items) {
            using namespace avsinfo::items::video;
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

        void add_all_audio_items(AudioItems& items) {
            using namespace avsinfo::items::audio;
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

#endif // ITEMS_HPP

