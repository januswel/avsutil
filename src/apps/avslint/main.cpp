/*
 * avslint main.cpp
 *  AviSynth script validator
 *
 *  written by janus_wel<janus.wel.3@gmail.com>
 *  This source code is in public domain, and has NO WARRANTY.
 * */

#include "../../include/avsutil.hpp"

#include "avslint.hpp"

#include <iostream>
#include <locale>
#include <memory>       // for std::auto_ptr
#include <stdexcept>

using namespace std;
using namespace avsutil;

int Main::start(void) {
    // Handling multi-nonopt-parameters
    if (!current_has_behind_parameters.empty()) {
        cerr
            << "Don't specify parameters behind <inputfile>: "
                << current_has_behind_parameters << "\n"
            << endl;
        usage(cerr);
        return BAD_ARGUMENT;
    }

    // Treating unknown options
    if (!unknown_opt_parameters.empty()) {
        cerr
            << "Unknown options: "
            << tconv().join(
                    unknown_opt_parameters.begin(),
                    unknown_opt_parameters.end(), ", ") << "\n"
            << endl;
        usage(cerr);
        return BAD_ARGUMENT;
    }

    // Option handling
    if (priority != UNSPECIFIED) {
        switch (priority) {
            case VERSION:   version_license(cout);  break;
            case HELP:      usage(cout);            break;
            default:        throw std::logic_error("unknown error");
        }
        return OK;
    }

    // Error handling for specifying an input file
    if (nonopt_parameter.empty()) {
        cerr << "Specify <inputfile>." << endl;
        usage(cerr);
        return BAD_ARGUMENT;
    }
    const string_type& inputfile = nonopt_parameter;

    // Do it
    auto_ptr<Avs> avs(CreateAvsObj(inputfile.c_str()));
    if (!avs->is_fine()) {
        cerr << avs->errmsg() << endl;
        return BAD_AVS;
    }

    return OK;
}

int main(const int argc, const char* const argv[]) {
    try {
        locale::global(locale(""));
        Main main;
        main.do_parameters(argc, argv);
        return main.start();
    }
    catch (const exception& ex) {
        cerr << "error: " << ex.what() << endl;
        return Main::UNKNOWN;
    }
}

