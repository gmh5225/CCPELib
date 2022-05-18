#pragma once

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
    size_t GetFileSizeByName(const char *FileName);
    bool ReadFileToMemory(const char *FileName, unsigned char *Buffer, size_t Size);
    bool WriteMemoryToFile(const char *FileName, unsigned char *Buffer, size_t Size);

private:
    bool mIsClearDebugData = true;
};

} // namespace CCPELib
