/*
 * iframestream.hpp
 *  Declarations and definitions of a class iframestream and framestreambuf
 *
 *  written by janus_wel<janus.wel.3@gmail.com>
 *  This source code is in public domain, and has NO WARRANTY.
 * */

#ifndef IFRAMESTREAM_HPP
#define IFRAMESTREAM_HPP

#include <istream>

#include "../../helper/cast.hpp"
#include "../../helper/dlogger.hpp"

namespace avsutil {
    namespace impl {
        class framestreambuf : public std::streambuf {
            private:
                const PVideoFrame frame;
                char* beginning;
                const uint32_t pitch;
                const uint32_t height;
                const uint32_t size;

            public:
                // constructor
                framestreambuf(PVideoFrame frame)
                    : frame(frame),
                      beginning(
                              const_cast<char*>(
                                  util::cast::constpointer_cast<const char*>(
                                      frame->GetReadPtr()))),
                      pitch(frame->GetPitch()), height(frame->GetHeight()),
                      size(pitch * height) {
                        DBGLOG( "framestreambuf::framestreambuf"
                                "(PVideoFrame)");
                        setg(beginning, beginning, beginning + size);
                    }

                // destructor
                ~framestreambuf(void) {
                    DBGLOG("framestreambuf::~framestreambuf(void)");
                }

            private:
                // copy constructor
                framestreambuf(const framestreambuf& rhs);
                // assignment operator
                framestreambuf& operator=(const framestreambuf& rhs);

            protected:
                pos_type seekoff(
                        off_type off, std::ios_base::seekdir way,
                        std::ios_base::openmode which =
                        std::ios_base::in | std::ios_base::out) {
                    DBGLOG( "framestreambuf::seekoff(" << off << ", "
                            << way << ", " << which << ")");

                    // Does only if a stream is opened in an input mode
                    if ((which & std::ios_base::in) == std::ios_base::in) {
                        uint64_t current_pos = gptr() - eback();

                        DBGLOG("current position: " << current_pos);

                        // In case of tellg()
                        if (way == std::ios_base::cur && off == 0) {
                            DBGLOG("call by tellg()");
                            // The current position may be different from
                            // eback(), after setg().
                            return pos_type(off_type(current_pos));
                        }

                        // In case of seekg()
                        DBGLOG("call by seekg()");
                        uint64_t target;
                        switch (way) {
                            case std::ios_base::beg:
                                target = off;
                                break;
                            case std::ios_base::cur:
                                target = current_pos + off;
                                break;
                            case std::ios_base::end:
                                target = size - off - 1;
                                break;
                            default:
                                return pos_type(off_type(-1));
                        }

                        DBGLOG("0 <= " << target << " < " << size << "?");
                        if (target < 0 || size <= target) {
                            DBGLOG("out of range. failed to seekoff");
                            return pos_type(off_type(-1));
                        }
                        DBGLOG("OK");

                        setg(beginning, beginning + target, beginning + size);

                        return pos_type(off_type(target));
                    }
                    return pos_type(off_type(-1));
                }

                pos_type seekpos(
                        pos_type sp,
                        std::ios_base::openmode which =
                        std::ios_base::in | std::ios_base::out) {
                    DBGLOG( "framestreambuf::seekpos("
                            << sp << ", " << which << ")");
                    return seekoff(off_type(sp), std::ios_base::beg, which);
                }
        };

        class iframestream : public std::istream {
            private:
                const std::streambuf* internal_buf;
                unsigned int n;

            public:
                // constructor
                iframestream(PVideoFrame frame, unsigned int n)
                : std::istream(new framestreambuf(frame)),
                  internal_buf(rdbuf()), n(n) {
                    DBGLOG( "iframestream::iframestream"
                            "(PVideoFrame, " << n << ")");
                }

                // destructor
                ~iframestream(void) {
                    DBGLOG("iframestream::~iframestream(void)");
                    if (rdbuf() == internal_buf) {
                        DBGLOG( "delete the internal streambuf.");
                        delete rdbuf();
                    }
                }

                // utility function
                bool is_me(unsigned int n) { return (this->n == n); }

            private:
                // copy constructor
                iframestream(const iframestream& rhs);
                // assignment operator
                iframestream& operator=(const iframestream& rhs);
        };
    }
}

#endif // IFRAMESTREAM_HPP

