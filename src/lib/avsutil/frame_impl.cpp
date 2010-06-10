/*
 * frame_impl.cpp
 *  remarks
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"

#include "../../helper/bmp.hpp"
#include "../../helper/cast.hpp"

namespace avsutil {
    namespace impl {
        void cframe_type::release(void) const {
            video->release_frame(this);
        }

        void cframe_type::write_header(std::ostream& out) const {
            format::windows_bitmap::elements_type elements = {
                mv_info.width,
                mv_info.height
            };
            format::windows_bitmap::header_type header(elements);
            out << header;
        }

        void cframe_type::write_data(std::ostream& out) const {
            out.write(
                    util::cast::constpointer_cast<const char*>(
                        frame->GetReadPtr()),
                    mv_info.pitch * mv_info.height);
        }
    }

    std::ostream&
    operator <<(std::ostream& out, const frame_type& frame) {
        DBGLOG( "avsutil::operator <<"
                "(std::ostream&, const avsutil::frame_type&)");

        frame.write(out);
        return out;
    }

    std::ostream&
    operator <<(std::ostream& out, const frame_type* const frame) {
        DBGLOG( "avsutil::operator <<"
                "(std::ostream&, const avsutil::frame_type* const)");

        frame->write(out);
        return out;
    }
}

