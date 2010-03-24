#include "debuglogger.hpp"

#ifdef _DEBUG

#include <fstream>

namespace debuglogger {
    // a file name to be outputed
    static const char* debuglogfile = "avsutil_debug.log";

    std::ofstream logs(debuglogfile, std::ios::trunc);
}

#endif // _DEBUG

