/*
 * avslint main.cpp
 *  AviSynth script validator
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "../../include/avsutil.hpp"
#include "avslint.hpp"

#include <iostream>
#include <string>
#include <locale>
#include <memory>   // for std::auto_ptr

using namespace std;
using namespace avsutil;

int main(const int argc, const char* const argv[]) {
    // set global locale to use non-ascii characters for filenames
    locale::global(locale(""));

    string inputfile;
    for (int i = 1; i < argc; ++i) {
        const string arg(argv[i]);
        if (arg == "-h" || arg == "--help") {
            usage(cout);
            return OK;
        }
        else if (arg == "-v" || arg == "--version") {
            version_license(cout);
            return OK;
        }
        else if (arg[0] == '-') {
            cerr << "Unknown option: \"" << arg << '"' << endl;
            usage(cerr);
            return BAD_ARG;
        }
        else {
            inputfile = arg;
            break;
        }
    }

    if (inputfile.empty()) {
        cerr << "Specify <inputfile>." << endl;
        usage(cerr);
        return BAD_ARG;
    }

    auto_ptr<IAvs> avs(CreateAvsObj(argv[1]));
    if (!avs->is_fine()) {
        cout << avs->errmsg() << endl;
        return BAD_AVS;
    }

    return OK;
}

