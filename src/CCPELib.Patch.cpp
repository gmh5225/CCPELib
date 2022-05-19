#include "CCPELib.h"
#include <linuxpe>

#if ((ULONG_MAX) == (UINT_MAX))
#    define CCPELibIS32BIT
#else
#    define CCPELibIS64BIT
#endif

#define PE_HEADER_SIZE 0x1000

namespace CCPELib {

bool
PELib::PatchAddressByVA(size_t VA, const std::vector<unsigned char> &PatchValueVector)
{
    if (!VA)
    {
        return false;
    }

    if (PatchValueVector.empty())
    {
        return false;
    }

    auto Rva = VA - GetImageBase();
    auto Offset = RVA2Offset(Rva);
    auto Target = mFileBufferMemory + Offset;
    std::copy(PatchValueVector.begin(), PatchValueVector.end(), Target);

    return true;
}

bool
PELib::PatchAddressByRVA(size_t RVA, const std::vector<unsigned char> &PatchValueVector)
{
    return PatchAddressByVA(RVA + GetImageBase(), PatchValueVector);
}

} // namespace CCPELib
