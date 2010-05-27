/*
 * item.hpp
 *  Declarations and definitions for the classes to format and show data
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#ifndef ITEM_HPP
#define ITEM_HPP

#include "../../helper/typeconv.hpp"
#include "../../helper/observer.hpp"

#include <locale>

namespace avsinfo {
    namespace items {
        // formating traits
        template<typename Char> struct format_default_traits;
        template<> struct format_default_traits<char> {
            typedef char    char_type;
            static const unsigned int header_margin = 24;
            static bool is_alignment(void) { return true; }
            static bool is_preceding_delimiter(void) { return true; }
            static const char_type* delimiter(void) { return ": "; }
            static bool is_unit(void) { return true; }
        };
        template<> struct format_default_traits<wchar_t> {
            typedef wchar_t char_type;
            static const unsigned int header_margin = 24;
            static bool is_alignment(void) { return true; }
            static bool is_preceding_delimiter(void) { return true; }
            static const char_type* delimiter(void) { return L": "; }
            static bool is_unit(void) { return true; }
        };

        // the base class to represent an item to show
        template<
            typename Info, typename Char,
            typename FormatTraits = format_default_traits<Char> >
        class basic_item : public pattern::observer::basic_observer<bool> {
            public:
                typedef Info                                info_type;
                typedef Char                                char_type;
                typedef std::basic_string<char_type>        string_type;
                typedef std::basic_stringstream<char_type>  stringstream_type;

            protected:
                // member variables
                bool is_human_friendly;

                static util::string::typeconverter& tconv(void) {
                    static util::string::typeconverter
                        tconv(std::locale::classic());
                    return tconv;
                }

                static stringstream_type& ss(void) {
                    static std::stringstream ss;
                    return ss;
                }

                // virtual functions
                // these functions determine the content to be outputed
                virtual const char_type* header(void) const = 0;
                virtual const char_type* unit(void) const = 0;
                virtual string_type value(const info_type&) const = 0;

            public:
                // implementations for virtual function of the super class
                // pattern::observer::basic_observer
                void update_state(const state_type& s) {
                    is_human_friendly = s;
                }

                // destructor
                virtual ~basic_item(void) {}

                // convert informations to string
                string_type to_string(const info_type& info) const {
                    if (!is_human_friendly) {
                        return value(info);
                    }

                    ss().clear();
                    ss().str("");
                    if (FormatTraits::is_alignment()) {
                        ss()
                            << std::left
                            << std::setw(FormatTraits::header_margin);

                        if (FormatTraits::is_preceding_delimiter()) {
                            ss()
                                << string_type(header())
                                   + FormatTraits::delimiter();
                        }
                        else {
                            ss() << header() << FormatTraits::delimiter();
                        }
                    }
                    else {
                        ss() << header() << FormatTraits::delimiter();
                    }
                    ss() << value(info);
                    if (FormatTraits::is_unit()) ss() << unit();

                    return ss().str();
                }
        };
    }
}

#endif // ITEM_HPP

