#include "debuglogger.hpp"

#ifdef _DEBUG
namespace debuglogger {
    static const char* debuglogfile = "avsutil_debug.log";
    std::ofstream logs(debuglogfile);
}
#endif
