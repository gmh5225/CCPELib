
#include <stdlib.h>
#include <stdio.h>
#include "CCPELib.h"

void
test_add_section()
{
    CCPELib::PELib pelib;
    pelib.Load("E:\\peshell\\smc-testing\\Output\\testexe-ori.exe");
    pelib.AppendSection(".hehe", 0x1000, 0);
    pelib.Save("E:\\peshell\\smc-testing\\Output\\testexe-ori.out.exe");
}

int
main()
{
    test_add_section();
    return 0;
}
