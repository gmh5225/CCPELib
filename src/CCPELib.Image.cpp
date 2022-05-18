#include "CCPELib.h"
#include <linuxpe>

#if ((ULONG_MAX) == (UINT_MAX))
#    define CCPELibIS32BIT
#else
#    define CCPELibIS64BIT
#endif

#define PE_HEADER_SIZE 0x1000

namespace CCPELib {

size_t
PELib::GetImageBase()
{
    if (mNtHeaderPtr32 == nullptr)
    {
        return 0;
    }

#ifdef CCPELibIS32BIT
    return mNtHeaderPtr32->optional_header.image_base;
#else
    return mNtHeaderPtr64->optional_header.image_base;
#endif
}

size_t
PELib::GetImageSize()
{
    if (mNtHeaderPtr32 == nullptr)
    {
        return 0;
    }

#ifdef CCPELibIS32BIT
    return mNtHeaderPtr32->optional_header.size_image;
#else
    return mNtHeaderPtr64->optional_header.size_image;
#endif
}

bool
PELib::SetImageBase(size_t dwImageBase)
{
    if (mNtHeaderPtr32 == nullptr)
    {
        return false;
    }

#ifdef CCPELibIS32BIT
    mNtHeaderPtr32->optional_header.image_base = dwImageBase;
#else
    mNtHeaderPtr64->optional_header.image_base = dwImageBase;
#endif

    return true;
}

bool
PELib::SetImageSize(size_t dwImageSize)
{
    if (mNtHeaderPtr32 == nullptr)
    {
        return false;
    }

#ifdef CCPELibIS32BIT
    mNtHeaderPtr32->optional_header.size_image = dwImageSize;
#else
    mNtHeaderPtr64->optional_header.size_image = dwImageSize;
#endif

    return true;
}

} // namespace CCPELib
