#include "CCPELib.h"
#include <linuxpe>

#if ((ULONG_MAX) == (UINT_MAX))
#    define CCPELibIS32BIT
#else
#    define CCPELibIS64BIT
#endif

#define PE_HEADER_SIZE 0x1000

namespace CCPELib {

} // namespace CCPELib
