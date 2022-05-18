#pragma once
#include <linuxpe>

namespace CCPELib {

class PELib
{
public:
    PELib();
    virtual ~PELib();

public:
    bool Load(const char *FileName, bool IsClearDebugData = true);

private:
    bool mIsClearDebugData;
};

} // namespace CCPELib
