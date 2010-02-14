/*
 * audiowriter.cpp
 *  definitions of avsutil::AudioWriter class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil.h"

namespace avsutil {
    // for a file format that has headers, data and footer
    bool AudioWriter::write(const std::string& wavfile) {
#ifdef _MSC_VER
        errno_t result = fopen_s(&wavfp, wavfile.c_str(), "wb");
        if (errno != 0) {
            return false;
        }
#else
        wavfp = fopen(wavfile.c_str(), "wb");
        if (wavfp == NULL) {
            return false;
        }
#endif
        write_header();
        write_data();
        write_footer();
        fclose(wavfp);

        return true;
    };
}
