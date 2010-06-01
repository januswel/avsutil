/*
 * avs_impl.cpp
 *  Definitions for cavs_type class
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil_impl.hpp"

#include "../../helper/dlogger.hpp"

namespace avsutil {
    // implementations for functions
    avs_type* create_avs(void) {
        DBGLOG("avsutil::create_avs(void)");
        return new impl::cavs_type();
    }
    avs_type* create_avs(const char* filename) {
        DBGLOG("avsutil::create_avs(\"" << filename << "\")");
        avs_type* avs = new impl::cavs_type();
        avs->open(filename);
        return avs;
    }

    namespace impl {
        void cavs_type::open(const char* avsfile) {
            DBGLOG("avsutil::impl::cavs_type::open(\"" << avsfile << "\")");

            try {
                // pack the filename as the argument of AviSynth filter
                AVSValue filename = avsfile;
                AVSValue args = AVSValue(&filename, 1);

                // load AviSynth script
                AVSValue imported = mv_se->Invoke("Import", args, 0);

                // get the clip and video informations
                mv_clip = imported.AsClip();

                // store filename
                mv_filename.assign(avsfile);
            }
            catch (AvisynthError& avserr) {
                mv_is_fine = false;
                mv_errmsg = avserr.msg;
            }
            catch (std::exception& ex) {
                mv_is_fine = false;
                mv_errmsg = ex.what();
            }
        }
    }
}

