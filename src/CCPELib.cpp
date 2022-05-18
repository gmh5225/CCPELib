#include "CCPELib.h"
#include <linuxpe>

#if ((ULONG_MAX) == (UINT_MAX))
#    define CCPELibIS32BIT
#else
#    define CCPELibIS64BIT
#endif

#define PE_HEADER_SIZE 0x1000

namespace CCPELib {

PELib::PELib() {}

PELib::~PELib() {}

/////////////////////////////////////////////////////////////////////////////////
//// public function
bool
PELib::Load(const char *FileName, bool IsClearDebugData /*= true*/)
{
    if (FileName == nullptr)
    {
        return false;
    }

    // mark ClearDebugData
    mIsClearDebugData = IsClearDebugData;

    if (!IsPEFile32(FileName) || !IsPEFile64(FileName))
    {
        return false;
    }

    // get file size
    auto FileSize = GetFileSizeByName(FileName);
    if (FileSize == 0)
    {
        return false;
    }

    // read file content to our buffer
    auto FileBufferMemory = new unsigned char[FileSize];
    if (FileBufferMemory == NULL)
    {
        return false;
    }

    if (!ReadFileToMemory(FileName, FileBufferMemory, FileSize))
    {
        delete[] FileBufferMemory;
        return false;
    }

    // save FileBufferMemory & FileSize
    mFileBufferMemory = FileBufferMemory;
    mOldFileSize = FileSize;
    mNewFileSize = FileSize;

    // save dos_header_t
    mDosHeaderPtr = (win::dos_header_t *)mFileBufferMemory;
    // save nt_headers
    mNtHeaderPtr32 = (win::nt_headers_x86_t *)(mFileBufferMemory + mDosHeaderPtr->e_lfanew);
    mNtHeaderPtr64 = (win::nt_headers_x64_t *)(mFileBufferMemory + mDosHeaderPtr->e_lfanew);
#ifdef CCPELibIS32BIT
    mSectionHeaderPtr =
        (win::section_header_t
             *)((unsigned char *)&mNtHeaderPtr32->optional_header + mNtHeaderPtr32->file_header.size_optional_header);
#else
    mSectionHeaderPtr =
        (win::section_header_t
             *)((unsigned char *)&mNtHeaderPtr64->optional_header + mNtHeaderPtr64->file_header.size_optional_header);
#endif

    // load pe struct
    if (!LoadPEStruct())
    {
        // remember to unlaod if failed
        UnLoadPEStruct();
        return false;
    }

    // save FileName
    mPEFileName = FileName;

    return true;
}

bool
PELib::IsPEFile32(const char *FileName)
{
    unsigned char PEHeaderBuffer[PE_HEADER_SIZE] = {0};
    do
    {
        if (!ReadFileToMemory(FileName, PEHeaderBuffer, sizeof(PEHeaderBuffer)))
        {
            break;
        }

        auto DosHeaderPtr = (win::dos_header_t *)PEHeaderBuffer;
        if (DosHeaderPtr->e_magic != win::DOS_HDR_MAGIC)
        {
            break;
        }

        auto NtHeadersPtr = (win::nt_headers_x86_t *)((unsigned char *)PEHeaderBuffer + DosHeaderPtr->e_lfanew);
        if (NtHeadersPtr->signature != win::NT_HDR_MAGIC)
        {
            break;
        }

        if (NtHeadersPtr->optional_header.magic == win::OPT_HDR32_MAGIC)
        {
            // X32
            return true;
        }

    } while (0);

    return false;
}

bool
PELib::IsPEFile64(const char *FileName)
{
    unsigned char PEHeaderBuffer[PE_HEADER_SIZE] = {0};
    do
    {
        if (!ReadFileToMemory(FileName, PEHeaderBuffer, sizeof(PEHeaderBuffer)))
        {
            break;
        }

        auto DosHeaderPtr = (win::dos_header_t *)PEHeaderBuffer;
        if (DosHeaderPtr->e_magic != win::DOS_HDR_MAGIC)
        {
            break;
        }

        auto NtHeadersPtr = (win::nt_headers_x64_t *)((unsigned char *)PEHeaderBuffer + DosHeaderPtr->e_lfanew);
        if (NtHeadersPtr->signature != win::NT_HDR_MAGIC)
        {
            break;
        }

        if (NtHeadersPtr->optional_header.magic == win::OPT_HDR64_MAGIC)
        {
            // X64
            return true;
        }

    } while (0);

    return false;
}

/////////////////////////////////////////////////////////////////////////////////
//// private function

//  file buffer
unsigned char *
PELib::GetFileBufferMemoryPtr()
{
    return mFileBufferMemory;
}

size_t
PELib::GetFileSizeByName(const char *FileName)
{
    if (FileName == nullptr)
    {
        return 0;
    }

    size_t FileSize = 0;

    auto PF = fopen(FileName, "rb");
    if (PF)
    {
        fseek(PF, 0, SEEK_END);

        FileSize = (size_t)_ftelli64(PF);

        fclose(PF);
    }

    return FileSize;
}

bool
PELib::ReadFileToMemory(const char *FileName, unsigned char *Buffer, size_t Size)
{
    if (FileName == nullptr || Buffer == nullptr || Size == 0)
    {
        return false;
    }

    bool Success = false;
    FILE *PF = nullptr;
    do
    {
        PF = fopen(FileName, "rb");
        if (!PF)
        {
            break;
        }

        size_t Read = fread(Buffer, Size, 1, PF);
        if (Read != Size)
        {
            break;
        }

        Success = true;
    } while (0);

    if (PF)
    {
        fclose(PF);
    }

    return Success;
}

bool
PELib::WriteMemoryToFile(const char *FileName, unsigned char *Buffer, size_t Size)
{
    if (FileName == nullptr || Buffer == nullptr || Size == 0)
    {
        return false;
    }

    bool Success = false;

    // first we remove the file
    remove(FileName);

    FILE *PF = nullptr;
    do
    {
        PF = fopen(FileName, "wb");
        if (!PF)
        {
            break;
        }

        size_t Write = fwrite(Buffer, Size, 1, PF);
        if (Write != Size)
        {
            break;
        }

        Success = true;
    } while (0);

    if (PF)
    {
        fclose(PF);
    }

    return Success;
}

// pe stub
bool
PELib::LoadPEStruct()
{
    // TODO
    return true;
}

void
PELib::UnLoadPEStruct()
{
}

} // namespace CCPELib
