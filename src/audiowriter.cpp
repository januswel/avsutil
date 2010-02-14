/*
 * audiowriter.cpp
 *  definitions of avsutil::AudioWriter class
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsutil.h"

namespace avsutil {
    // for a file format that has headers, data and footer
    bool AudioWriter::write(
            const std::string& wavfile,
            void (*progress)(const unsigned __int64, const unsigned __int64)) {
#ifdef _MSC_VER
        errno_t result = fopen_s(&audiofp, wavfile.c_str(), "wb");
        if (errno != 0) {
            return false;
        }
#else
        audiofp = fopen(wavfile.c_str(), "wb");
        if (audiofp == NULL) {
            return false;
        }
#endif
        write_header();
        write_data(progress);
        write_footer();
        fclose(audiofp);

        return true;
    };
}
