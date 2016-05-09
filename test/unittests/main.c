// Copyright 2016 ELIFE. All rights reserved.
// Use of this source code is governed by a MIT
// license that can be found in the LICENSE file.
#define CTEST_MAIN

#include <ctest.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char const** argv)
{
    srand(time(NULL));
    return ctest_main(argc, argv);
}
