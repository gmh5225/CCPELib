#include "CCPELib.h"
#include <linuxpe>

#if ((ULONG_MAX) == (UINT_MAX))
#    define CCPELibIS32BIT
#else
#    define CCPELibIS64BIT
#endif

#define PE_HEADER_SIZE 0x1000

namespace CCPELib {

unsigned int
PELib::RVA2Offset(unsigned int RVA)
{
    int SectionIndex = GetSectionIndexByRVA(RVA);
    if (SectionIndex < 0)
    {
        // Note: if the first section offset=0x400,RVA=0x1000, the address to be converted is 0x401
        // it should return an error because RVA=0x401, which does not exist in the file.
        // so it should be compared with PointerToRawData here.
        if (RVA < mSectionHeaderPtr[0].ptr_raw_data)
        {
            return RVA;
        }
    }
    else
    {
        // PointerToRawData + (RVA - VirtualAddress)
        auto SectionHeaderPtr = &mSectionHeaderPtr[SectionIndex];
        unsigned int Offset = RVA - SectionHeaderPtr->virtual_address;

        if (Offset < SectionHeaderPtr->size_raw_data)
        {
            return SectionHeaderPtr->ptr_raw_data + Offset;
        }
        else
        {
            // error
            return 0;
        }
    }

    return 0;
}

} // namespace CCPELib
