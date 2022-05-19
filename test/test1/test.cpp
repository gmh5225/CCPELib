
#include <stdlib.h>
#include <stdio.h>
#include "CCPELib.h"

void
test_add_section()
{
    CCPELib::PELib pelib;

    pelib.Load(CCPELIB_SRC_DIR "/sample/testexe.exe");
    pelib.AppendSection(".hehe", 0x1000, 0);
    std::vector<unsigned char> PatchValue = {0x90, 0x90};
    auto LastSectionVA = pelib.GetLastSectionVA();
    printf("GetLastSectionVA=%p\n", LastSectionVA);
    pelib.PatchAddressByVA(LastSectionVA, PatchValue);
    pelib.Save(CCPELIB_SRC_DIR "/sample/testexe.out.exe");
}

int
main()
{
    test_add_section();
    return 0;
}
