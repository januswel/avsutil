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
        cframe_type::cframe_type(PVideoFrame frame, uint32_t n)
        : frame(frame) {
            DBGLOG( "avsutil::impl::cframe_type::"
                    "cframe_type(PVideoFrame)");

            mv_info.width = frame->GetRowSize() / 3;
            mv_info.pitch = frame->GetPitch();
            mv_info.height = frame->GetHeight();
            mv_info.nth = n;

            DBGLOG( "\n"
                    "width: " << mv_info.width << "\n"
                    "pitch: " << mv_info.pitch << "\n"
                    "height: " << mv_info.height << "\n");
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
            out.write(util::cast::constpointer_cast<const char*>(frame->GetReadPtr()),
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

