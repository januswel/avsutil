/*
 * audio_impl.cpp
 *  Definitions for caudio_type class
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"

#include "../../helper/dlogger.hpp"

namespace avsutil {
    namespace impl {
        class avsaudiostreambuf : public std::streambuf {
            private:
                PClip clip;
                IScriptEnvironment* se;

                // fundamental informations about AVS file
                uint32_t sample_size;
                uint64_t numof_samples;

                /*
                 *  A unit of both is sample.
                 *      current: a position that is the first element of buffer
                 *      next:    a position that is the beggining to get
                 *               samples next underflow()
                 * */
                uint64_t page_current;
                uint64_t page_next;

                // a number of samples to get from AVS file
                uint32_t samples_at_a_time;

                // buffer controlling
                char* buf;
                bool is_internal_buf;

                // constants
                static const uint32_t buf_size_def = 4096;

            public:
                // constructor
                avsaudiostreambuf(PClip clip, IScriptEnvironment* se)
                    : clip(clip), se(se),
                    sample_size(clip->GetVideoInfo().BytesPerAudioSample()),
                    numof_samples(clip->GetVideoInfo().num_audio_samples),
                    page_current(0), page_next(0),
                    samples_at_a_time(buf_size_def / sample_size),
                    buf(new char[sample_size * samples_at_a_time]),
                    is_internal_buf(true) {
                        DBGLOG( "avsaudiostreambuf::avsaudiostreambuf"
                                "(PClip, IScriptEnvironment*)");
                    }

                // destructor
                ~avsaudiostreambuf(void) {
                    DBGLOG( "avsaudiostreambuf::~avsaudiostreambuf(void)");
                    if (is_internal_buf) {
                        DBGLOG( "delete the internal buffer");
                        delete[] buf;
                    }
                }

            private:
                // copy constructor
                avsaudiostreambuf(const avsaudiostreambuf& rhs);
                // assignment operator
                avsaudiostreambuf& operator=(const avsaudiostreambuf& rhs);

            protected:
                std::streambuf* setbuf(char_type* s, std::streamsize n) {
                    DBGLOG( "avsaudiostreambuf::setbuf(char_type*, "
                            << n << ")");

                    // Delete the existing buffer if that was allocated by
                    // itself.
                    if (is_internal_buf) {
                        DBGLOG("delete the internal buffer");
                        delete[] buf;
                        is_internal_buf = false;
                    }

                    if (gptr() && eback()) {
                        DBGLOG("reset eback(), gptr(), egptr()");
                        page_current += (gptr() - eback()) / sample_size;
                        setg(NULL, NULL, NULL);
                    }
                    page_next = page_current;
                    buf = s;
                    samples_at_a_time = static_cast<uint32_t>(n) / sample_size;

                    return this;
                }

                pos_type seekoff(
                        off_type off, std::ios_base::seekdir way,
                        std::ios_base::openmode which =
                        std::ios_base::in | std::ios_base::out) {
                    DBGLOG( "avsaudiostreambuf::seekoff("
                            << off << ", " << way << ", " << which << ")");

                    // Does only if a stream is opened in an input mode
                    if ((which & std::ios_base::in) == std::ios_base::in) {
                        uint64_t current_pos = (gptr() && eback())
                            ? page_current + (gptr() - eback()) / sample_size
                            : page_current;

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
                                target = numof_samples - off - 1;
                                break;
                            default:
                                return pos_type(off_type(-1));
                        }

                        DBGLOG( "0 <= " << target << " < "
                                << numof_samples << "?");
                        if (target < 0 || numof_samples <= target) {
                            DBGLOG("out of range. failed to seekoff");
                            return pos_type(off_type(-1));
                        }
                        DBGLOG("OK");

                        if (gptr() && eback()) {
                            DBGLOG( page_current << " <= " << target
                                    << " < " << page_next << "?");
                            if (page_current <= target && target < page_next) {
                                DBGLOG("cache hit");
                                setg(   eback(),
                                        eback()
                                            + (target - page_current)
                                                * sample_size,
                                        egptr());
                            }
                            else {
                                DBGLOG("out of cache");
                                setg(NULL, NULL, NULL);
                                page_next = page_current = target;
                            }
                        }
                        else {
                            DBGLOG("before getting");
                            page_next = page_current = target;
                        }

                        return pos_type(off_type(target));
                    }
                    return pos_type(off_type(-1));
                }

                pos_type seekpos(
                        pos_type sp,
                        std::ios_base::openmode which =
                        std::ios_base::in | std::ios_base::out) {
                    DBGLOG( "avsaudiostreambuf::seekpos("
                            << sp << ", " << which << ")");
                    return seekoff(off_type(sp), std::ios_base::beg, which);
                }

                int_type underflow(void) {
                    DBGLOG("avsaudiostreambuf::underflow(void)");
                    if (numof_samples <= page_next) {
                        DBGLOG("reached to the end of avs audio stream");
                        return traits_type::eof();
                    }

                    uint64_t remainder = numof_samples - page_next;
                    get_audio_data(
                            (samples_at_a_time < remainder)
                            ? samples_at_a_time
                            : remainder);
                    page_current = page_next;
                    page_next += samples_at_a_time;

                    return traits_type::to_int_type(*gptr());
                }

                // utility function
                /*
                 * function GetAudio() returns audio data in the following
                 * format:
                 *
                 *  case mono
                 *      d0, d1, ... , dn
                 *  case stereo
                 *      l0, r0, l1, r1, ... , ln, rn
                 *  case 5.1 ch (maybe)
                 *      fl0, fr0, fc0, lf0, bl0, br0,
                 *          ... , fln, frn, fcn, lfn, bln, brn
                 *
                 *  notes
                 *      d:  data            (for mono)
                 *      l:  left            (for stereo)
                 *      r:  right           (for stereo)
                 *      fl: front left      (for 5.1ch)
                 *      fr: front right     (for 5.1ch)
                 *      fc: front center    (for 5.1ch)
                 *      lf: low frequency   (for 5.1ch)
                 *      bl: back left       (for 5.1ch)
                 *      br: back right      (for 5.1ch)
                 * */
                void get_audio_data(uint64_t samples) {
                    DBGLOG("get_audio_data(" << samples << ")");
                    clip->GetAudio(buf, page_next, samples, se);
                    setg(buf, buf, buf + sample_size * samples);
                }
        };

        class iavsaudiostream : public std::istream {
            private:
                const std::streambuf* internal_buf;

            public:
                // constructor
                iavsaudiostream(PClip clip, IScriptEnvironment* se)
                    : std::istream(new avsaudiostreambuf(clip, se)),
                    internal_buf(rdbuf()) {
                        DBGLOG( "iavsaudiostream::iavsaudiostream"
                                "(PClip, IScriptEnvironment*)");
                    }

                // destructor
                ~iavsaudiostream(void) {
                    DBGLOG( "iavsaudiostream::~iavsaudiostream(void)");
                    if (rdbuf() == internal_buf) {
                        DBGLOG( "delete the internal streambuf.");
                        delete rdbuf();
                    }
                }

            private:
                // copy constructor
                iavsaudiostream(const iavsaudiostream& rhs);
                // assignment operator
                iavsaudiostream& operator=(const iavsaudiostream& rhs);
        };

        std::istream& caudio_type::stream(void) {
            if (mv_stream == NULL) {
                mv_stream = new iavsaudiostream(mv_clip, mv_se);
            }
            return *mv_stream;
        }
    }
}

