#pragma once
#include <string>
#include <vector>

namespace win {
struct dos_header_t;

template <bool x64>
struct nt_headers_t;

} // namespace win

namespace coff {
struct section_header_t;

} // namespace coff

namespace CCPELib {

class PELib
{
public:
    PELib();
    virtual ~PELib();

public:
    // static method

    // misc
    static bool IsPEFile32(const char *FileName);
    static bool IsPEFile64(const char *FileName);

    //  file buffer
    static size_t GetFileSizeByName(const char *FileName);
    static bool ReadFileToMemory(const char *FileName, unsigned char *Buffer, size_t Size);
    static bool WriteMemoryToFile(const char *FileName, unsigned char *Buffer, size_t Size);

public:
    // file input/output
    bool Load(const char *FileName, bool IsClearDebugData = true);
    bool Save(const char *FileName);
    size_t GetOutputFileSize();
    unsigned char *GetFileBufferMemoryPtr();
    bool ReallocFileBufferMemory(unsigned int NewSize, unsigned int OldSize);

public:
    // align
    unsigned int AlignValue(unsigned int Value, unsigned int AlignNumber);
    unsigned int FileAlignValue(unsigned int Value);
    unsigned int SectionAlignValue(unsigned int Value);

public:
    // transform
    unsigned int RVA2Offset(unsigned int RVA);

public:
    // section
    int GetNumberOfSections();
    bool SetNumberOfSections(int SectionCount);
    unsigned int GetSectionMaxOffset(); // align by file alignment
    unsigned int GetSectionMaxRVA();    // align by section alignment
    int GetSectionIndexByRVA(unsigned int RVA);
    int GetSectionIndexByVA(size_t VA);
    int GetSectionIndexByName(const char *SectionName);
    int GetSectionIndexByOffset(unsigned int Offset);
    unsigned int GetLastSectionRVA();
    size_t GetLastSectionVA();
    int AppendSection(const char *NewSectionName, size_t Size, unsigned int Characteristics);
    int AppendSection(const char *NewSectionName, size_t RawSize, size_t VirtualSize, unsigned int Characteristics);

public:
    // image
    size_t GetImageBase();
    size_t GetImageSize();
    bool SetImageBase(size_t ImageBase);
    bool SetImageSize(size_t ImageSize);

public:
    // patch
    bool PatchAddressByVA(size_t VA, const std::vector<unsigned char> &PatchValueVector);
    bool PatchAddressByRVA(size_t RVA, const std::vector<unsigned char> &PatchValueVector);

private:
    // pe stub
    bool LoadPEStruct();
    void UnLoadPEStruct();

private:
    // misc
    bool mIsClearDebugData = true;

    // file name
    std::string mPEFileName = "";

    //  file buffer
    unsigned char *mFileBufferMemory = nullptr;
    size_t mOldFileSize = 0;
    size_t mNewFileSize = 0;

    // pe stub
    win::dos_header_t *mDosHeaderPtr = nullptr;
    win::nt_headers_t<false> *mNtHeaderPtr32 = nullptr;
    win::nt_headers_t<true> *mNtHeaderPtr64 = nullptr;
    coff::section_header_t *mSectionHeaderPtr = nullptr;
};

} // namespace CCPELib
