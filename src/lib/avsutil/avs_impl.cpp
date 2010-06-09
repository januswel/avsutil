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
    manager_type& manager(void) {
        static impl::cmanager_type manager;
        return manager;
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
                mv_filepath = avsfile;
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

