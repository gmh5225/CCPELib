#include "CCPELib.h"
#include <linuxpe>

#if ((ULONG_MAX) == (UINT_MAX))
#    define CCPELibIS32BIT
#else
#    define CCPELibIS64BIT
#endif

#define PE_HEADER_SIZE 0x1000

namespace CCPELib {

// misc
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

//  file buffer
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

        size_t Read = fread(Buffer, 1, Size, PF);
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

        size_t Write = fwrite(Buffer, 1, Size, PF);
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

} // namespace CCPELib
