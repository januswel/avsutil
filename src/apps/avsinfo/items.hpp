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
#include "../../helper/typeconv.hpp"

#include <bitset>
#include <locale>
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
}

namespace avsinfo {
    namespace items {
        // human-friendly or machine-friendly
        class Notation : public pattern::observer::basic_subject<bool> {
            private:
                // status
                bool is_human_friendly;

            protected:
                // implementation of virtual function
                const state_type& subject_state(void) const { return is_human_friendly; }

            public:
                // constructor
                explicit Notation(bool is_human_friendly = true)
                    : is_human_friendly(is_human_friendly) { notify_state(); }

                // setter
                inline void human_friendly(void) {
                    is_human_friendly = true;
                    notify_state();
                }
                inline void machine_friendly(void) {
                    is_human_friendly = false;
                    notify_state();
                }
        };

        struct item_traits {
            static const unsigned int header_size;
            static const bool is_alignment;
            static const bool is_preceding_delimiter;
            static const char* delimiter(void);
            static const bool is_unit;
        };

        // the base class to represent an item to show
        template<typename infoT, typename itemTraitsT = item_traits>
            class basic_item : public pattern::observer::basic_observer<bool> {
                public:
                    typedef infoT                           info_t;
                    typedef basic_item<info_t, itemTraitsT> this_t;

                protected:
                    // member variables
                    bool is_human_friendly;
                    // wrapper for utility object
                    inline util::string::typeconverter& tconv(void) const {
                        static util::string::typeconverter tconv(std::locale::classic());
                        return tconv;
                    }

                    // virtual functions
                    // these functions determin the outputed content
                    virtual const char* header(void) const = 0;
                    virtual const char* unit(void) const = 0;
                    virtual const std::string value(const info_t&) const = 0;

                public:
                    // destructor
                    virtual ~basic_item(void) {}
                    // assignment operator
                    inline this_t& operator=(const this_t& rhs) {
                        is_human_friendly = rhs.is_human_friendly;
                        return *this;
                    }

                    // output informations to ostream object
                    template<typename charT>
                        std::basic_ostream<charT>& output(std::basic_ostream<charT>& out, const info_t& info) const {
                            if (is_human_friendly) {
                                if (itemTraitsT::is_alignment) {
                                    out << std::setw(itemTraitsT::header_size);
                                    if (itemTraitsT::is_preceding_delimiter) {
                                        out << std::string(header()) + itemTraitsT::delimiter();
                                    }
                                    else {
                                        out << header() << itemTraitsT::delimiter();
                                    }
                                }
                                else {
                                    out << header() << itemTraitsT::delimiter();
                                }
                                out << value(info);
                                if (itemTraitsT::is_unit) out << unit();
                                return out << endl;
                            }
                            else {
                                return out << value(info) << endl;
                            }
                        }

                    // implementation of virtual function
                    void update_state(const state_type& s) { is_human_friendly = s; }
            };

        // items to show, for video stream
        namespace video {
            typedef basic_item<avsutil::VideoInfo> Item;

            // the declarations of classes for items to show
            ITEM_CLASS_DECL(Width);
            ITEM_CLASS_DECL(Height);
            ITEM_CLASS_DECL(Ratio);
            ITEM_CLASS_DECL(Fps);
            ITEM_CLASS_DECL(FpsFraction);
            ITEM_CLASS_DECL(VideoTime);
            ITEM_CLASS_DECL(Frames);
            ITEM_CLASS_DECL(ColorSpace);
            ITEM_CLASS_DECL(Bpp);
            ITEM_CLASS_DECL(InterlaceType);
            ITEM_CLASS_DECL(FieldOrder);

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
        }

        // items to show, for audio stream
        namespace audio {
            typedef basic_item<avsutil::AudioInfo> Item;

            // the declarations of classes for items to show
            ITEM_CLASS_DECL(Channels);
            ITEM_CLASS_DECL(BitDepth);
            ITEM_CLASS_DECL(SampleType);
            ITEM_CLASS_DECL(AudioTime);
            ITEM_CLASS_DECL(SamplingRate);
            ITEM_CLASS_DECL(Samples);
            ITEM_CLASS_DECL(BlockSize);

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
        }

        template<typename infoT> struct info_traits;
        template<> struct info_traits<avsutil::VideoInfo> {
            // typedefs
            typedef avsutil::VideoInfo  info_t;
            typedef basic_item<info_t>  base_t;
            typedef video::item_t       itemkind_t;
            // static variables
            static const unsigned int numof_items = video::NUMOF_ITEMS;
            // static functions
            static base_t* create_item(itemkind_t item) {
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
                    case video::INTERLACE_TYPE: return new video::InterlaceType;
                    case video::FIELD_ORDER:    return new video::FieldOrder;
                    default:                    return NULL;
                }
            }
        };

        template<> struct info_traits<avsutil::AudioInfo> {
            // typedefs
            typedef avsutil::AudioInfo  info_t;
            typedef basic_item<info_t>  base_t;
            typedef audio::item_t       itemkind_t;
            // static variables
            static const unsigned int numof_items = audio::NUMOF_ITEMS;
            // static functions
            static base_t* create_item(itemkind_t item) {
                switch (item) {
                    case audio::CHANNELS:       return new audio::Channels;
                    case audio::BIT_DEPTH:      return new audio::BitDepth;
                    case audio::SAMPLE_TYPE:    return new audio::SampleType;
                    case audio::AUDIO_TIME:     return new audio::AudioTime;
                    case audio::SAMPLING_RATE:  return new audio::SamplingRate;
                    case audio::SAMPLES:        return new audio::Samples;
                    case audio::BLOCK_SIZE:     return new audio::BlockSize;
                    default:                    return NULL;
                }
            }
        };

        // the base class to represent items that should be outputed
        // This class also contains the order to show.
        // TODO: use reference-count pointer for base_t*
        template<typename infoT, typename traitsT = info_traits<infoT> >
            class basic_items {
                private:
                    typedef infoT                           info_t;
                    typedef typename traitsT::base_t        base_t;
                    typedef std::vector<base_t*>            items_t;
                    typedef typename items_t::iterator      items_it;
                    typedef typename traitsT::itemkind_t    itemkind_t;
                    typedef basic_items<infoT>              this_t;

                private:
                    // member variables
                    // items to show, and showing order
                    items_t items;
                    // flags to check dupricative assignment
                    std::bitset<traitsT::numof_items> specified_flag;
                    // human-friendly or machine-friendly
                    Notation mv_notation;

                public:
                    // add an item to show at the end of the order if still not specified.
                    this_t& add_item(itemkind_t itemkind) {
                        if (!specified_flag[itemkind]) {
                            base_t* new_item = traitsT::create_item(itemkind);
                            items.push_back(new_item);
                            mv_notation.attach_observer(new_item);
                            specified_flag.set(itemkind);
                        }
                        return *this;
                    }

                    // setter for status
                    inline this_t& notation(bool is_human_friendly) {
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

        typedef basic_items<avsutil::VideoInfo> VideoItems;
        typedef basic_items<avsutil::AudioInfo> AudioItems;

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

