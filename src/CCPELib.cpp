#include "CCPELib.h"

#if ((ULONG_MAX) == (UINT_MAX))
#    define CCPELibIS32BIT
#else
#    define CCPELibIS64BIT
#endif

namespace CCPELib {

PELib::PELib()
{
    mIsClearDebugData = true;
}

PELib::~PELib() {}

bool
PELib::Load(const char *FileName, bool IsClearDebugData /*= true*/)
{
    if (FileName == nullptr)
    {
        return false;
    }

    mIsClearDebugData = IsClearDebugData;
    return true;
}

} // namespace CCPELib