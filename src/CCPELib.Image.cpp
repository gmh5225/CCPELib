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
PELib::SetImageBase(size_t ImageBase)
{
    if (mNtHeaderPtr32 == nullptr)
    {
        return false;
    }

#ifdef CCPELibIS32BIT
    mNtHeaderPtr32->optional_header.image_base = ImageBase;
#else
    mNtHeaderPtr64->optional_header.image_base = ImageBase;
#endif

    return true;
}

bool
PELib::SetImageSize(size_t ImageSize)
{
    if (mNtHeaderPtr32 == nullptr)
    {
        return false;
    }

#ifdef CCPELibIS32BIT
    mNtHeaderPtr32->optional_header.size_image = ImageSize;
#else
    mNtHeaderPtr64->optional_header.size_image = ImageSize;
#endif

    return true;
}

} // namespace CCPELib
