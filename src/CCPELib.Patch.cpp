#include "CCPELib.h"
#include <linuxpe>

namespace CCPELib {

bool
PELib::PatchAddressByVA(uint64_t VA, const std::vector<unsigned char> &PatchValueVector)
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
