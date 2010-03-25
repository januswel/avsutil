/*
 * usage.cpp
 *  Usage definitions for avslint.
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "avsinfo.hpp"
#include <iostream>

void usage(std::ostream& out) {
    out << "Usage: avslint [options] <inputfile>\n"
        << "General options:\n"
        << "    -h, --help      Shows this help.\n"
        << "    -v, --version   Shows version and license informations.\n"
        << "\n"
        << "    -a, --all       Shows all of informations about the <inputfile>.\n"
        << "                    This is default value if you don't specify any options.\n"
        << "    -i, --video     Shows all of video stream informations\n"
        << "                    that has no one about audio stream.\n"
        << "    -u, --audio     Shows all of audio stream informations.\n"
        << "    -n, --noheader  Doesn't show each header of informations.\n"
        << "\n"
        << "Options for video stream:\n"
        << "    --size          Shows a size of video.\n"
        << "    --fps           Shows a FPS - frames per second of video.\n"
        << "    --videotime     Shows a time length of video stream.\n"
        << "    --frames        Shows a number of frames.\n"
        << "    --colorspace    Shows a color space of frames.\n"
        << "    --bpp           Shows bits per pixel of frame.\n"
        << "    --interlacetype Shows whether video is field-based or frame-based.\n"
        << "                    This doesn't always mean video is interlaced,\n"
        << "                    because the information whether video is frame-based\n"
        << "                    or progressive is contained by AviSynth.\n"
        << "                    See AviSynth's help for more detail.\n"
        << "    --tff           Shows whether video is TFF - top field first"
        << "                    or BFF - bottom field first.\n"
        << "                    This has a point only when \"--interlacetype\" is field-based.\n"
        << "\n"
        << "Options for audio stream:\n"
        << "    --channels      Shows a number of channels.\n"
        << "    --bitdepth      Shows a bit depth of audio samples.\n"
        << "    --audiotime     Shows a time length of audio stream.\n"
        << "    --samplingrate  Shows a sampling rate of audio samples.\n"
        << "    --samples       Shows a number of audio samples.\n"
        << "    --blocksize     Shows bytes per sample.\n"
        << std::endl;
}

