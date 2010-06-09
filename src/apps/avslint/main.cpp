/*
 * avslint main.cpp
 *  AviSynth script validator
 *
 *  Copyright (C) 2010 janus_wel<janus.wel.3@gmail.com>
 *  see LICENSE for redistributing, modifying, and so on.
 * */

#include "../../include/avsutil.hpp"

#include "avslint.hpp"
#include "main.hpp"

#include <iostream>
#include <locale>
#include <memory>       // for std::auto_ptr
#include <stdexcept>

using namespace std;
using namespace avsutil;

// global objects
util::string::typeconverter tconv;

int Main::main(void) {
    // Error handling for specifying an input file
    if (inputfile.empty()) {
        throw avslint_error(BAD_ARGUMENT, "Specify <inputfile>.\n");
    }

    // Do it
    avs_type& avs = manager().load(inputfile.c_str());
    if (!avs.is_fine()) {
        cerr << avs.errmsg() << endl;
        return BAD_AVS;
    }

    return OK;
}

int main(const int argc, const char* const argv[]) {
    try {
        locale::global(locale(""));
        Main main;
        main.analyze_option(argc, argv);
        main.preparation();
        return main.start();
    }
    catch (const avslint_error& ex) {
        cerr << ex.what() << endl;
        if (ex.return_value() == BAD_ARGUMENT) usage(cerr);
        return ex.return_value();
    }
    catch (const exception& ex) {
        cerr << "error: " << ex.what() << endl;
        return UNKNOWN;
    }
}

