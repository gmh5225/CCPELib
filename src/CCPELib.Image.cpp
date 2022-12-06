#include "CCPELib.h"
#include <linuxpe>

namespace CCPELib {

uint64_t
PELib::GetImageBase()
{
    if (mNtHeaderPtr32 == nullptr)
    {
        return 0;
    }

    if (mNtHeaderPtr64 == nullptr)
    {
        return 0;
    }

    if (mIsX64)
    {
        return mNtHeaderPtr64->optional_header.image_base;
    }
    else
    {
        return mNtHeaderPtr32->optional_header.image_base;
    }
    return 0;
}

size_t
PELib::GetImageSize()
{
    if (mNtHeaderPtr32 == nullptr)
    {
        return 0;
    }

    if (mNtHeaderPtr64 == nullptr)
    {
        return 0;
    }

    if (mIsX64)
    {
        return mNtHeaderPtr64->optional_header.size_image;
    }
    else
    {
        return mNtHeaderPtr32->optional_header.size_image;
    }
    return 0;
}

bool
PELib::SetImageBase(uint64_t ImageBase)
{
    if (mNtHeaderPtr32 == nullptr)
    {
        return false;
    }

    if (mNtHeaderPtr64 == nullptr)
    {
        return false;
    }

    if (mIsX64)
    {
        mNtHeaderPtr64->optional_header.image_base = ImageBase;
    }
    else
    {
        mNtHeaderPtr32->optional_header.image_base = ImageBase;
    }

    return true;
}

bool
PELib::SetImageSize(size_t ImageSize)
{
    if (mNtHeaderPtr32 == nullptr)
    {
        return false;
    }

    if (mNtHeaderPtr64 == nullptr)
    {
        return false;
    }

    if (mIsX64)
    {
        mNtHeaderPtr64->optional_header.size_image = ImageSize;
    }
    else
    {
        mNtHeaderPtr32->optional_header.size_image = ImageSize;
    }

    return true;
}

} // namespace CCPELib
