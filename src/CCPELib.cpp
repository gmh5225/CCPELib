#include "CCPELib.h"
#include <linuxpe>

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

    if (IsPEFile64(FileName))
    {
        mIsX64 = true;
    }
    else if (IsPEFile32(FileName))
    {
        mIsX64 = false;
    }
    else
    {
        // not support
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

    if (mIsX64)
    {
        mSectionHeaderPtr =
            (win::section_header_t
                 *)((unsigned char *)&mNtHeaderPtr64->optional_header + mNtHeaderPtr64->file_header.size_optional_header);
    }
    else
    {
        mSectionHeaderPtr =
            (win::section_header_t
                 *)((unsigned char *)&mNtHeaderPtr32->optional_header + mNtHeaderPtr32->file_header.size_optional_header);
    }

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
PELib::Save(const char *FileName)
{
    if (mFileBufferMemory == nullptr || FileName == nullptr)
    {
        return false;
    }

    bool Success = false;

    do
    {
        // TODO save pe stub

        Success = true;
    } while (0);

    if (Success)
    {
        Success = WriteMemoryToFile(FileName, mFileBufferMemory, GetOutputFileSize());
    }

    return Success;
}

size_t
PELib::GetOutputFileSize()
{
    size_t OutputFileSize = mNewFileSize;

    // TODO pe stub size

    return OutputFileSize;
}

size_t
PELib::GetOldFileSize()
{
    return mOldFileSize;
}

size_t
PELib::GetNewFileSize()
{
    return mNewFileSize;
}

unsigned char *
PELib::GetFileBufferMemoryPtr()
{
    return mFileBufferMemory;
}

bool
PELib::ReallocFileBufferMemory(size_t NewSize, size_t OldSize)
{
    if (NewSize == 0 || NewSize < OldSize)
    {
        return false;
    }

    auto ReallocNewFileMemory = new unsigned char[NewSize];
    if (!ReallocNewFileMemory)
    {
        return false;
    }

    // copy old buffer
    memcpy(ReallocNewFileMemory, mFileBufferMemory, OldSize);
    // set 'nop' to new part
    memset(ReallocNewFileMemory + OldSize, 0x90, NewSize - OldSize);

    // realloc mFileBufferMemory
    delete[] mFileBufferMemory;
    mFileBufferMemory = ReallocNewFileMemory;

    // save dos_header_t
    mDosHeaderPtr = (win::dos_header_t *)mFileBufferMemory;
    // save nt_headers
    mNtHeaderPtr32 = (win::nt_headers_x86_t *)(mFileBufferMemory + mDosHeaderPtr->e_lfanew);
    mNtHeaderPtr64 = (win::nt_headers_x64_t *)(mFileBufferMemory + mDosHeaderPtr->e_lfanew);

    if (mIsX64)
    {
        mSectionHeaderPtr =
            (win::section_header_t
                 *)((unsigned char *)&mNtHeaderPtr64->optional_header + mNtHeaderPtr64->file_header.size_optional_header);
    }
    else
    {
        mSectionHeaderPtr =
            (win::section_header_t
                 *)((unsigned char *)&mNtHeaderPtr32->optional_header + mNtHeaderPtr32->file_header.size_optional_header);
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////
//// private function
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
