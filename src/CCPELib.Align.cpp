#include "CCPELib.h"
#include <linuxpe>

#if ((ULONG_MAX) == (UINT_MAX))
#    define CCPELibIS32BIT
#else
#    define CCPELibIS64BIT
#endif

#define PE_HEADER_SIZE 0x1000

namespace CCPELib {

// align
unsigned int
PELib::AlignValue(unsigned int Value, unsigned int AlignNumber)
{
    if (AlignNumber == 0)
    {
        return 0;
    }

    if (Value % AlignNumber == 0)
    {
        return Value;
    }

    return AlignNumber * (Value / AlignNumber) + AlignNumber;
}

unsigned int
PELib::FileAlignValue(unsigned int Value)
{
    if (mNtHeaderPtr32 == NULL)
    {
        return 0;
    }

    if (mNtHeaderPtr64 == NULL)
    {
        return 0;
    }

#ifdef CCPELibIS32BIT
    auto FileAlignment = mNtHeaderPtr32->optional_header.file_alignment;
#else
    auto FileAlignment = mNtHeaderPtr64->optional_header.file_alignment;
#endif
    return AlignValue(Value, FileAlignment);
}

unsigned int
PELib::SectionAlignValue(unsigned int Value)
{
    if (mNtHeaderPtr32 == NULL)
    {
        return 0;
    }

    if (mNtHeaderPtr64 == NULL)
    {
        return 0;
    }

#ifdef CCPELibIS32BIT
    auto SectionAlignment = mNtHeaderPtr32->optional_header.section_alignment;
#else
    auto SectionAlignment = mNtHeaderPtr64->optional_header.section_alignment;
#endif
    return AlignValue(Value, SectionAlignment);
}

} // namespace CCPELib
