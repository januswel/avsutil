/*
 * avsutil.cpp
 *  definitions of avsutil::AvsUtil class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil.h"

namespace avsutil {
    // * definitions for Avs2File
    void AvsUtil::open(const std::string& avsfile) {
        try {
            se = CreateScriptEnvironment();

            // pack the filename as the argument of AviSynth filter
            AVSValue filename = avsfile.c_str();
            AVSValue args = AVSValue(&filename, 1);

            // load AviSynth script
            AVSValue imported = se->Invoke("Import", args, 0);

            // get the clip
            clip = imported.AsClip();

            // get video informations and pack audio informations
            vi = clip->GetVideoInfo();
            pack_audio_info();
        }
        catch (AvisynthError& avserr) {
            throw Avs2FileError(std::string("Specified file is not valid for AviSynth.\n") + avserr.msg);
        }
    }
}
