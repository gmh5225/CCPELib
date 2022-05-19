#include "CCPELib.h"
#include <linuxpe>

#if ((ULONG_MAX) == (UINT_MAX))
#    define CCPELibIS32BIT
#else
#    define CCPELibIS64BIT
#endif

#define PE_HEADER_SIZE 0x1000

namespace CCPELib {

int
PELib::GetNumberOfSections()
{
#ifdef CCPELibIS32BIT
    return mNtHeaderPtr32->file_header.num_sections;
#else
    return mNtHeaderPtr64->file_header.num_sections;
#endif
}

bool
PELib::SetNumberOfSections(int SectionCount)
{
    if (SectionCount < 0 || mNtHeaderPtr32 == NULL)
    {
        return false;
    }

#ifdef CCPELibIS32BIT
    mNtHeaderPtr32->file_header.num_sections = SectionCount;
#else
    mNtHeaderPtr64->file_header.num_sections = SectionCount;
#endif
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
#ifdef CCPELibIS32BIT
        auto SizeOfHeaders = mNtHeaderPtr32->optional_header.size_headers;
#else
        auto SizeOfHeaders = mNtHeaderPtr64->optional_header.size_headers;
#endif
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
#ifdef CCPELibIS32BIT
        auto SizeOfHeaders = mNtHeaderPtr32->optional_header.size_headers;
#else
        auto SizeOfHeaders = mNtHeaderPtr64->optional_header.size_headers;
#endif
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
PELib::GetSectionIndexByVA(size_t VA)
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
    auto SectionHeaderPtr = &mSectionHeaderPtr[NumberOfSections - 1];
    return SectionHeaderPtr->virtual_address;
}

size_t
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

int
PELib::AppendSection(const char *NewSectionName, size_t RawSize, size_t VirtualSize, unsigned int Characteristics)
{
    // RawSize can be set 0
    if (NewSectionName == nullptr || VirtualSize == 0 || mSectionHeaderPtr == nullptr)
    {
        return -1;
    }

    // SizeOfHeaders = 0x400
#ifdef CCPELibIS32BIT
    auto SizeOfHeaders = mNtHeaderPtr32->optional_header.size_headers;
#else
    auto SizeOfHeaders = mNtHeaderPtr64->optional_header.size_headers;
#endif
    auto NumberOfSections = GetNumberOfSections();

    // get new section header offset
    auto NewSectionHeaderOffset = unsigned int(
        (size_t)mSectionHeaderPtr - (size_t)mFileBufferMemory + sizeof(win::section_header_t) * NumberOfSections);

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
    auto SectionNameLen = strlen(NewSectionName);
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

} // namespace CCPELib
