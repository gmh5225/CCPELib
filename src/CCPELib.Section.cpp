#include "CCPELib.h"
#include <linuxpe>

namespace CCPELib {

int
PELib::GetNumberOfSections()
{
    if (mIsX64)
    {
        return mNtHeaderPtr64->file_header.num_sections;
    }
    else
    {
        return mNtHeaderPtr32->file_header.num_sections;
    }
}

bool
PELib::SetNumberOfSections(int SectionCount)
{
    if (SectionCount < 0 || mNtHeaderPtr32 == NULL)
    {
        return false;
    }

    if (mIsX64)
    {
        mNtHeaderPtr64->file_header.num_sections = SectionCount;
    }
    else
    {
        mNtHeaderPtr32->file_header.num_sections = SectionCount;
    }

    return true;
}

unsigned int
PELib::GetSectionMaxOffset()
{
    int NumberOfSection = GetNumberOfSections();
    if (NumberOfSection < 0 || mSectionHeaderPtr == NULL || mNtHeaderPtr32 == NULL)
    {
        return 0;
    }

    unsigned int MaxOffset = 0;

    // max offset = max.ptr_raw_data + max.size_raw_data
    for (int i = 0; i < NumberOfSection; i++)
    {
        auto SectionHeaderPtr = &mSectionHeaderPtr[i];
        if (MaxOffset < SectionHeaderPtr->ptr_raw_data + SectionHeaderPtr->size_raw_data)
        {
            MaxOffset = SectionHeaderPtr->ptr_raw_data + SectionHeaderPtr->size_raw_data;
        }
    }

    if (MaxOffset == 0)
    {
        // imposible actually
        unsigned int SizeOfHeaders = 0;
        if (mIsX64)
        {
            SizeOfHeaders = mNtHeaderPtr64->optional_header.size_headers;
        }
        else
        {
            SizeOfHeaders = mNtHeaderPtr32->optional_header.size_headers;
        }

        MaxOffset = SizeOfHeaders;
    }

    // align by file alignment
    MaxOffset = FileAlignValue(MaxOffset);

    return MaxOffset;
}

unsigned int
PELib::GetSectionMaxRVA()
{
    int NumberOfSection = GetNumberOfSections();
    if (NumberOfSection < 0 || mSectionHeaderPtr == NULL || mNtHeaderPtr32 == NULL)
    {
        return 0;
    }

    unsigned int MaxRVA = 0;

    // max offset = max.virtual_address + max.virtual_size
    for (int i = 0; i < NumberOfSection; i++)
    {
        auto SectionHeaderPtr = &mSectionHeaderPtr[i];
        if (MaxRVA < SectionHeaderPtr->virtual_address + SectionHeaderPtr->virtual_size)
        {
            MaxRVA = SectionHeaderPtr->virtual_address + SectionHeaderPtr->virtual_size;
        }
    }

    if (MaxRVA == 0)
    {
        // imposible actually
        unsigned int SizeOfHeaders = 0;
        if (mIsX64)
        {
            SizeOfHeaders = mNtHeaderPtr64->optional_header.size_headers;
        }
        else
        {
            SizeOfHeaders = mNtHeaderPtr32->optional_header.size_headers;
        }
        MaxRVA = SizeOfHeaders;
    }

    // align by section alignment
    MaxRVA = SectionAlignValue(MaxRVA);

    return MaxRVA;
}

int
PELib::GetSectionIndexByRVA(unsigned int RVA)
{
    int NumberOfSections = GetNumberOfSections();

    for (int i = 0; i < NumberOfSections; i++)
    {
        auto SectionHeaderPtr = &mSectionHeaderPtr[i];

        if (RVA >= SectionHeaderPtr->virtual_address &&
            RVA < SectionHeaderPtr->virtual_address + SectionHeaderPtr->virtual_size)
        {
            return i;
        }
    }

    return -1;
}

int
PELib::GetSectionIndexByVA(uint64_t VA)
{
    return GetSectionIndexByRVA(VA - GetImageBase());
}

int
PELib::GetSectionIndexByName(const char *SectionName)
{
    if (mSectionHeaderPtr == nullptr || SectionName == nullptr)
    {
        return -1;
    }

    int NumberOfSections = GetNumberOfSections();
    char TempName[LEN_SHORT_STR + 2] = {0};

    for (int i = 0; i < NumberOfSections; i++)
    {
        memset(TempName, 0, sizeof(TempName));
        memcpy(TempName, mSectionHeaderPtr[i].name.short_name, LEN_SHORT_STR);
        if (strncmp(TempName, SectionName, LEN_SHORT_STR) == 0)
        {
            return i;
        }
    }

    return -1;
}

int
PELib::GetSectionIndexByOffset(unsigned int Offset)
{
    int NumberOfSections = GetNumberOfSections();

    for (int i = 0; i < NumberOfSections; i++)
    {
        auto SectionHeaderPtr = &mSectionHeaderPtr[i];

        if (Offset >= SectionHeaderPtr->ptr_raw_data &&
            Offset < SectionHeaderPtr->ptr_raw_data + SectionHeaderPtr->size_raw_data)
        {
            return i;
        }
    }

    return -1;
}

unsigned int
PELib::GetLastSectionRVA()
{
    int NumberOfSections = GetNumberOfSections();
    if (NumberOfSections < 1)
    {
        return 0;
    }

    auto SectionHeaderPtr = &mSectionHeaderPtr[NumberOfSections - 1];
    return SectionHeaderPtr->virtual_address;
}

uint64_t
PELib::GetLastSectionVA()
{
    return GetImageBase() + GetLastSectionRVA();
}

int
PELib::AppendSection(const char *NewSectionName, size_t Size, unsigned int Characteristics)
{
    if (NewSectionName == nullptr || Size == 0 || mSectionHeaderPtr == nullptr)
    {
        return -1;
    }

    return AppendSection(NewSectionName, Size, Size, Characteristics);
}

bool
PELib::AppendSection(
    const char *NewSectionName,
    std::vector<unsigned char> &AppendBuffer,
    size_t Size,
    unsigned int Characteristics)
{
    if (NewSectionName == nullptr || Size == 0 || mSectionHeaderPtr == nullptr)
    {
        return false;
    }

    return AppendSection(NewSectionName, AppendBuffer, Size, Size, Characteristics);
}

int
PELib::AppendSection(const char *NewSectionName, size_t RawSize, size_t VirtualSize, unsigned int Characteristics)
{
    // RawSize can be set 0
    if (NewSectionName == nullptr || VirtualSize == 0 || mSectionHeaderPtr == nullptr)
    {
        return -1;
    }

    // SizeOfHeaders = 0x400
    uint32_t SizeOfHeaders = 0;
    if (mIsX64)
    {
        SizeOfHeaders = mNtHeaderPtr64->optional_header.size_headers;
    }
    else
    {
        SizeOfHeaders = mNtHeaderPtr32->optional_header.size_headers;
    }

    auto NumberOfSections = GetNumberOfSections();

    // get new section header offset
    auto NewSectionHeaderOffset =
        (unsigned int)((size_t)mSectionHeaderPtr - (size_t)mFileBufferMemory + sizeof(win::section_header_t) * NumberOfSections);

    if (NewSectionHeaderOffset + sizeof(win::section_header_t) > SizeOfHeaders)
    {
        // outside boundary
        return -1;
    }

    // get new section offset
    auto NewSectionOffset = GetSectionMaxOffset();
    if (NewSectionOffset == 0)
    {
        return -1;
    }

    // get new section rva
    auto NewSectionRVA = GetSectionMaxRVA();
    if (NewSectionRVA == 0)
    {
        return -1;
    }

    // set new section info
    auto NewSectionHeaderPtr = &mSectionHeaderPtr[NumberOfSections];
    memset(NewSectionHeaderPtr, 0, sizeof(win::section_header_t));

    // new virtual_address/virtual_size
    NewSectionHeaderPtr->virtual_address = NewSectionRVA;
    NewSectionHeaderPtr->virtual_size = VirtualSize;

    // new ptr_raw_data/size_raw_data
    if (RawSize == 0)
    {
        NewSectionHeaderPtr->ptr_raw_data = 0;
        NewSectionHeaderPtr->size_raw_data = 0;
    }
    else
    {
        NewSectionHeaderPtr->ptr_raw_data = NewSectionOffset;
        NewSectionHeaderPtr->size_raw_data = FileAlignValue(RawSize);
    }

    // new section name
    unsigned char SectionName[LEN_SHORT_STR] = {0};
    size_t SectionNameLen = strlen(NewSectionName);
    if (SectionNameLen > LEN_SHORT_STR)
    {
        memcpy(SectionName, NewSectionName, LEN_SHORT_STR);
    }
    else
    {
        memcpy(SectionName, NewSectionName, SectionNameLen);
    }
    memcpy(NewSectionHeaderPtr->name.short_name, SectionName, LEN_SHORT_STR);

    // new characteristics
    if (Characteristics == 0)
    {
        NewSectionHeaderPtr->characteristics.cnt_code = 1;
        NewSectionHeaderPtr->characteristics.mem_execute = 1;
        NewSectionHeaderPtr->characteristics.mem_read = 1;
        NewSectionHeaderPtr->characteristics.mem_write = 1;
    }
    else
    {
        NewSectionHeaderPtr->characteristics.flags = Characteristics;
    }

    // new section counts
    SetNumberOfSections(NumberOfSections + 1);

    // new image size
    auto NewImageSize = NewSectionHeaderPtr->virtual_address + NewSectionHeaderPtr->virtual_size;
    NewImageSize = SectionAlignValue(NewImageSize);
    SetImageSize(NewImageSize);

    // update NewFileSize
    if (RawSize)
    {
        auto NewFileSize = NewSectionHeaderPtr->ptr_raw_data + NewSectionHeaderPtr->size_raw_data;
        ReallocFileBufferMemory(NewFileSize, mNewFileSize);
        mNewFileSize = NewFileSize;
    }

    return NumberOfSections;
}

bool
PELib::AppendSection(
    const char *NewSectionName,
    std::vector<unsigned char> &AppendBuffer,
    size_t RawSize,
    size_t VirtualSize,
    unsigned int Characteristics)
{
    int NumberOfSections = AppendSection(NewSectionName, RawSize, VirtualSize, Characteristics);
    if (NumberOfSections > 0)
    {
        auto LastSectionVA = GetLastSectionVA();
        return PatchAddressByVA(LastSectionVA, AppendBuffer);
    }

    return false;
}

} // namespace CCPELib
