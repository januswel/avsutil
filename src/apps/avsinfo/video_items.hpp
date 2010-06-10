/*
 * video_items.hpp
 *  Declarations and definitions for classes of video items
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef VIDEO_ITEMS_HPP
#define VIDEO_ITEMS_HPP

#include "item.hpp"

#include "../../include/avsutil.hpp"

#include "../../helper/gcd.hpp"

namespace avsinfo {
    namespace items {
        namespace video {
            // video item enumerations
            enum item_type {
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

            // declarations and definitions for classes of items to show
            typedef basic_item<avsutil::video_type::info_type, char> Item;

            class Width : public Item {
                protected:
                    const char_type* header(void) const { return "width"; }
                    const char_type* unit(void) const { return "px"; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        return tconv().strfrom(vi.width);
                    }
            };

            class Height : public Item {
                protected:
                    const char_type* header(void) const { return "height"; }
                    const char_type* unit(void) const { return "px"; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        return tconv().strfrom(vi.height);
                    }
            };

            class Ratio : public Item {
                protected:
                    const char_type* header(void) const { return "ratio"; }
                    const char_type* unit(void) const { return ""; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        const unsigned int gcd = util::math::calc_gcd(vi.width, vi.height);
                        return tconv().strfrom(vi.width/gcd) + ':' + tconv().strfrom(vi.height/gcd);
                    }
            };

            class Fps : public Item {
                protected:
                    const char_type* header(void) const { return "FPS"; }
                    const char_type* unit(void) const { return ""; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        return tconv().strfrom(vi.fps);
                    }
            };

            class FpsFraction : public Item {
                protected:
                    const char_type* header(void) const { return "FPS(fraction)"; }
                    const char_type* unit(void) const { return ""; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        return tconv().strfrom(vi.fps_numerator) + '/' + tconv().strfrom(vi.fps_denominator);
                    }
            };

            class VideoTime : public Item {
                protected:
                    const char_type* header(void) const { return "time of video"; }
                    const char_type* unit(void) const { return "sec"; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        return tconv().strfrom(vi.time);
                    }
            };

            class Frames : public Item {
                protected:
                    const char_type* header(void) const { return "a number of frames"; }
                    const char_type* unit(void) const { return ""; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        return tconv().strfrom(vi.numof_frames);
                    }
            };

            class ColorSpace : public Item {
                protected:
                    const char_type* header(void) const { return "color space"; }
                    const char_type* unit(void) const { return ""; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        return string_type(vi.fourcc_name(vi.color_space));
                    }
            };

            class Bpp : public Item {
                protected:
                    const char_type* header(void) const { return "bits per pixel"; }
                    const char_type* unit(void) const { return "bits"; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        return tconv().strfrom(vi.bpp);
                    }
            };

            class InterlaceType : public Item {
                protected:
                    const char_type* header(void) const { return "interlace type"; }
                    const char_type* unit(void) const { return ""; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        return is_human_friendly
                            ? string_type(vi.is_fieldbased ? "field based" : "frame based")
                            : tconv().strfrom(vi.is_fieldbased);
                    }
            };

            class FieldOrder : public Item {
                protected:
                    const char_type* header(void) const { return "field order"; }
                    const char_type* unit(void) const { return ""; }
                    string_type value(const avsutil::video_type::info_type& vi) const {
                        return is_human_friendly
                            ? string_type(vi.is_tff ? "top field first" : "bottom field first")
                            : tconv().strfrom(vi.is_tff);
                    }
            };
        }
    }
}

#endif // VIDEO_ITEMS_HPP

