#include "CCPELib.h"
#include <linuxpe>

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

    uint32_t FileAlignment = 0x200;
    if (mIsX64)
    {
        FileAlignment = mNtHeaderPtr64->optional_header.file_alignment;
    }
    else
    {
        FileAlignment = mNtHeaderPtr32->optional_header.file_alignment;
    }

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

    uint32_t SectionAlignment = 0x1000;
    if (mIsX64)
    {
        SectionAlignment = mNtHeaderPtr64->optional_header.section_alignment;
    }
    else
    {
        SectionAlignment = mNtHeaderPtr32->optional_header.section_alignment;
    }

    return AlignValue(Value, SectionAlignment);
}

} // namespace CCPELib
