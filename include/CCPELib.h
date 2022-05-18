#pragma once
#include <string>

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
    bool Load(const char *FileName, bool IsClearDebugData = true);
    bool IsPEFile32(const char *FileName);
    bool IsPEFile64(const char *FileName);

private:
    //  file buffer
    unsigned char *GetFileBufferMemoryPtr();
    size_t GetFileSizeByName(const char *FileName);
    bool ReadFileToMemory(const char *FileName, unsigned char *Buffer, size_t Size);
    bool WriteMemoryToFile(const char *FileName, unsigned char *Buffer, size_t Size);
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
