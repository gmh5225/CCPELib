
#include <stdlib.h>
#include <stdio.h>
#include <CCPELib.h>
#include <gtest/gtest.h>

TEST(test1, test_add_section)
{
    CCPELib::PELib pelib;

    pelib.Load(CCPELIB_SRC_DIR "/sample/testexe.exe");
    pelib.AppendSection(".hehe", 0x1000, 0);
    std::vector<unsigned char> PatchValue = {0x90, 0x90};
    auto LastSectionVA = pelib.GetLastSectionVA();
    printf("New section VA=%p\n", LastSectionVA);
    pelib.PatchAddressByVA(LastSectionVA, PatchValue);
    pelib.Save(CCPELIB_SRC_DIR "/sample/testexe.out.exe");
}

int
main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, const_cast<char **>(argv));
    return RUN_ALL_TESTS();
}
