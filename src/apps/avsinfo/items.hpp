/*
 * items.hpp
 *  Declarations and definitions of items to show
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef ITEMS_HPP
#define ITEMS_HPP

#include "../../include/avsutil.hpp"
#include "../../helper/observer.hpp"
#include "../../helper/tconv.hpp"

#include <bitset>
#include <ostream>
#include <string>
#include <vector>

// for convenience
#define ITEM_CLASS_DECL(_name)                              \
class _name : public Item {                                 \
    protected:                                              \
        const char* header(void) const;                     \
        const char* unit(void) const;                       \
        const std::string value(const info_t& vi) const;    \
};

namespace avsinfo {
    namespace items {
        // human-friendly or machine-friendly
        class Notation : public pattern::observer::basic_subject<bool> {
            private:
                // status
                bool is_human_friendly;

            protected:
                // implementation of virtual function
                const state_t& state(void) const { return is_human_friendly; }

            public:
                // constructor
                explicit Notation(bool is_human_friendly = true)
                    : is_human_friendly(is_human_friendly) { notify(); }

                // setter
                inline void human_friendly(void) {
                    is_human_friendly = true;
                    notify();
                }
                inline void machine_friendly(void) {
                    is_human_friendly = false;
                    notify();
                }
        };

        // the base class to represent an item to show
        template<typename T>
            class basic_item : public pattern::observer::basic_observer<bool> {
                public:
                    typedef T   info_t;

                protected:
                    // member variables
                    bool is_human_friendly;
                    const std::string delimiter;
                    static const unsigned int header_size;
                    // wrapper for utility object
                    inline util::string::converter& tconv(void) const {
                        static util::string::converter tconv;
                        return tconv;
                    }

                    // virtual functions
                    // these functions determin the outputed content
                    virtual const char* header(void) const = 0;
                    virtual const char* unit(void) const = 0;
                    virtual const std::string value(const info_t&) const = 0;

                public:
                    // constructor
                    basic_item(void) : delimiter(": ") {}
                    // destructor
                    virtual ~basic_item(void) {}

                    // output informations to ostream object
                    template<typename charT>
                    inline std::ostream& output(std::basic_ostream<charT>& out, const info_t& info) const {
                        if (is_human_friendly)
                            return out
                                << std::setw(header_size) << header() << delimiter
                                << value(info) << unit() << endl;
                        else
                            return out << value(info) << endl;
                    }

                    // implementation of virtual function
                    void update(const state_t& s) { is_human_friendly = s; }
            };

        // items to show, for video stream
        namespace video {
            typedef basic_item<avsutil::VideoInfo> Item;

            // the declarations of classes for items to show
            ITEM_CLASS_DECL(Width)
            ITEM_CLASS_DECL(Height)
            ITEM_CLASS_DECL(Ratio)
            ITEM_CLASS_DECL(Fps)
            ITEM_CLASS_DECL(FpsFraction)
            ITEM_CLASS_DECL(VideoTime)
            ITEM_CLASS_DECL(Frames)
            ITEM_CLASS_DECL(ColorSpace)
            ITEM_CLASS_DECL(Bpp)
            ITEM_CLASS_DECL(InterlaceType)
            ITEM_CLASS_DECL(FieldOrder)

            // for convenience
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

                NUMOF_ITEMS
            };

            struct traits {
                typedef Item                base_t;
                typedef item_t              itemkind_t;
                typedef avsutil::VideoInfo  info_t;
                static inline unsigned int numof(void) { return NUMOF_ITEMS; }
                static Item* create_item(item_t item) {
                    switch (item) {
                        case WIDTH:             return new Width;
                        case HEIGHT:            return new Height;
                        case RATIO:             return new Ratio;
                        case FPS:               return new Fps;
                        case FPS_FRACTION:      return new FpsFraction;
                        case VIDEO_TIME:        return new VideoTime;
                        case FRAMES:            return new Frames;
                        case COLOR_SPACE:       return new ColorSpace;
                        case BPP:               return new Bpp;
                        case INTERLACE_TYPE:    return new InterlaceType;
                        case FIELD_ORDER:       return new FieldOrder;
                        default:                return NULL;
                    }
                }
            };
        }

        // items to show, for audio stream
        namespace audio {
            typedef basic_item<avsutil::AudioInfo> Item;

            // the declarations of classes for items to show
            ITEM_CLASS_DECL(Channels)
            ITEM_CLASS_DECL(BitDepth)
            ITEM_CLASS_DECL(SampleType)
            ITEM_CLASS_DECL(AudioTime)
            ITEM_CLASS_DECL(SamplingRate)
            ITEM_CLASS_DECL(Samples)
            ITEM_CLASS_DECL(BlockSize)

            // for convenience
            enum item_t {
                CHANNELS,
                BIT_DEPTH,
                SAMPLE_TYPE,
                AUDIO_TIME,
                SAMPLING_RATE,
                SAMPLES,
                BLOCK_SIZE,

                NUMOF_ITEMS
            };

            struct traits {
                typedef Item                base_t;
                typedef item_t              itemkind_t;
                typedef avsutil::AudioInfo  info_t;
                static inline unsigned int numof(void) { return NUMOF_ITEMS; }
                static Item* create_item(item_t item) {
                    switch (item) {
                        case CHANNELS:      return new Channels;
                        case BIT_DEPTH:     return new BitDepth;
                        case SAMPLE_TYPE:   return new SampleType;
                        case AUDIO_TIME:    return new AudioTime;
                        case SAMPLING_RATE: return new SamplingRate;
                        case SAMPLES:       return new Samples;
                        case BLOCK_SIZE:    return new BlockSize;
                        default:            return NULL;
                    }
                }
            };
        }

        // the base class to represent items that should be outputed
        // This class also contains the order to show.
        // TODO: use reference-count pointer for base_t*
        template<typename traitsT, int numof>
            class basic_items {
                private:
                    typedef typename traitsT::base_t        base_t;
                    typedef std::vector<base_t*>            items_t;
                    typedef typename items_t::iterator      items_it;
                    typedef typename traitsT::itemkind_t    itemkind_t;
                    typedef typename traitsT::info_t        info_t;

                private:
                    // member variables
                    // items to show, and showing order
                    items_t items;
                    // flags to check dupricative assignment
                    std::bitset<numof> specified_flag;
                    // human-friendly or machine-friendly
                    Notation mv_notation;

                public:
                    // add an item to show at the end of the order if still not specified.
                    basic_items<traitsT, numof>& add_item(itemkind_t itemkind) {
                        if (!specified_flag[itemkind]) {
                            base_t* new_item = traitsT::create_item(itemkind);
                            items.push_back(new_item);
                            mv_notation.attach(new_item);
                            specified_flag.set(itemkind);
                        }
                        return *this;
                    }

                    // setter for status
                    inline basic_items<traitsT, numof>&  notation(bool is_human_friendly) {
                        is_human_friendly
                            ? mv_notation.human_friendly()
                            : mv_notation.machine_friendly();
                        return *this;
                    }

                    // output contents
                    template<typename charT>
                    void output(std::basic_ostream<charT>& out, const info_t& info) {
                        for (items_it it = items.begin(); it != items.end(); ++it) {
                            (*it)->output(out, info);
                        }
                    }
            };

        typedef basic_items<video::traits, video::NUMOF_ITEMS> VideoItems;
        typedef basic_items<audio::traits, audio::NUMOF_ITEMS> AudioItems;

        namespace video {
            // for convenience
            void add_all_items(VideoItems& items);
        }

        namespace audio {
            // for convenience
            void add_all_items(AudioItems& items);
        }
    }
}

#endif // ITEMS_HPP

