#ifndef DEBUGLOGGER_HPP
#define DEBUGLOGGER_HPP

#ifdef _DEBUG
#   define DBGLOG(str) (debuglogger::logs << str << std::endl)
#   ifdef _MSC_VER
#       define FUNCNAME __FUNCTION__
#   else
#       define FUNCNAME __func__
#   endif
#else
#   define DBGLOG(str)
#endif

#ifdef _DEBUG

#include <fstream>

namespace debuglogger {
    extern std::ofstream logs;
}

#endif // _DEBUG

#endif // DEBUGLOGGER_HPP

