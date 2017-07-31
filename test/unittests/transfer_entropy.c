// Copyright 2016-2017 ELIFE. All rights reserved.
// Use of this source code is governed by a MIT
// license that can be found in the LICENSE file.
#include "util.h"
#include <inform/transfer_entropy.h>
#include <math.h>
#include <unit.h>

UNIT(TransferEntropyNULLSeries)
{
    int const series[] = {1,1,0,0,1,0,0,1};

    inform_error err = INFORM_SUCCESS;
    ASSERT_NAN(inform_transfer_entropy(NULL, series, 1, 3, 2, 2, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_ETIMESERIES, err);

    err = INFORM_SUCCESS;
    ASSERT_NAN(inform_transfer_entropy(series, NULL, 1, 3, 2, 2, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_ETIMESERIES, err);
}

UNIT(TransferEntropyNoInits)
{
    int const series[] = {1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;
    ASSERT_NAN(inform_transfer_entropy(series, series, 0, 3, 2, 2, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_ENOINITS, err);
}

UNIT(TransferEntropySeriesTooShort)
{
    int const series[] = {1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;

    for (size_t i = 0; i < 2; ++i)
    {
        err = INFORM_SUCCESS;
        ASSERT_NAN(inform_transfer_entropy(series, series, 1, i, 2, 2, &err));
        ASSERT_TRUE(inform_failed(&err));
        ASSERT_EQUAL(INFORM_ESHORTSERIES, err);
    }
}

UNIT(TransferEntropyHistoryTooLong)
{
    int const series[] = {1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;
    for (size_t i = 2; i < 4; ++i)
    {
        err = INFORM_SUCCESS;
        ASSERT_NAN(inform_transfer_entropy(series, series, 1, 2, 2, i, &err));
        ASSERT_TRUE(inform_failed(&err));
        ASSERT_EQUAL(INFORM_EKLONG, err);
    }
}

UNIT(TransferEntropyZeroHistory)
{
    int const series[] = {1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;
    ASSERT_NAN(inform_transfer_entropy(series, series, 1, 2, 2, 0, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_EKZERO, err);
}

UNIT(TransferEntropyInvalidBase)
{
    int const series[] = {1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;
    for (int i = 0; i < 2; ++i)
    {
        err = INFORM_SUCCESS;
        ASSERT_NAN(inform_transfer_entropy(series, series, 1, 2, i, 2, &err));
        ASSERT_TRUE(inform_failed(&err));
        ASSERT_EQUAL(INFORM_EBASE, err);
    }
}

UNIT(TransferEntropyNegativeState)
{
    int const seriesA[] = { 1,1,0,0,1,0,0,1};
    int const seriesB[] = {-1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;

    ASSERT_NAN(inform_transfer_entropy(seriesA, seriesB, 1, 8, 2, 2, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_ENEGSTATE, err);

    err = INFORM_SUCCESS;
    ASSERT_NAN(inform_transfer_entropy(seriesB, seriesA, 1, 8, 2, 2, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_ENEGSTATE, err);
}

UNIT(TransferEntropyBadState)
{
    int const seriesA[] = {1,1,0,0,1,0,0,1};
    int const seriesB[] = {2,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;

    ASSERT_NAN(inform_transfer_entropy(seriesA, seriesB, 1, 8, 2, 2, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_EBADSTATE, err);

    err = INFORM_SUCCESS;
    ASSERT_NAN(inform_transfer_entropy(seriesB, seriesA, 1, 8, 2, 2, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_EBADSTATE, err);
}

UNIT(TransferEntropySingleSeries_Base2)
{
    {
        int series[10] = {
                1,1,1,0,0,
                1,1,0,0,1
        };
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(series,   series,   1, 5, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.666666,
                inform_transfer_entropy(series+5, series,   1, 5, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(series,   series+5, 1, 5, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(series+5, series+5, 1, 5, 2, 2, NULL), 1e-6);
    }

    {
        int series[20] = {
                0,0,1,1,1,0,0,0,0,1,
                1,1,0,0,0,0,0,0,1,1
        };
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(series,    series,    1, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.500000,
                inform_transfer_entropy(series+10, series,    1, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.106844,
                inform_transfer_entropy(series,    series+10, 1, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(series+10, series+10, 1, 10, 2, 2, NULL), 1e-6);
    }

    {
        int series[20] = {
                0,1,0,1,0,0,1,1,0,0,
                0,0,1,0,1,1,1,0,1,1
        };
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(series,    series,    1, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.344361,
                inform_transfer_entropy(series+10, series,    1, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.250000,
                inform_transfer_entropy(series,    series+10, 1, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(series+10, series+10, 1, 10, 2, 2, NULL), 1e-6);
    }
}

UNIT(TransferEntropyEnsemble_Base2)
{
    {
        int xseries[50] = {
            1, 1, 1, 0, 0, 1, 1, 0, 1, 0,
            0, 1, 0, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
            0, 0, 1, 0, 0, 0, 1, 0, 0, 1,
            0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        };
        int yseries[50] = {
            0, 1, 0, 0, 0, 1, 0, 1, 1, 0,
            0, 0, 0, 1, 1, 1, 0, 1, 0, 0,
            1, 0, 1, 0, 1, 0, 0, 0, 1, 0,
            0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
            0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
        };

        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(xseries, xseries, 5, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.091141,
                inform_transfer_entropy(yseries, xseries, 5, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.107630,
                inform_transfer_entropy(xseries, yseries, 5, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(yseries, yseries, 5, 10, 2, 2, NULL), 1e-6);

        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(xseries, xseries, 4, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.134536,
                inform_transfer_entropy(yseries, xseries, 4, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.089517,
                inform_transfer_entropy(xseries, yseries, 4, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(yseries, yseries, 4, 10, 2, 2, NULL), 1e-6);
    }
    {
        int xseries[50] = {
            0, 1, 0, 1, 0, 0, 1, 1, 1, 1,
            0, 1, 0, 1, 1, 1, 0, 0, 1, 0,
            1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
            0, 1, 1, 1, 1, 1, 0, 1, 1, 1,
        };
        int yseries[50] = {
            1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
            0, 1, 1, 0, 0, 1, 1, 0, 0, 0,
            0, 1, 1, 1, 0, 1, 0, 0, 0, 0,
            0, 1, 0, 0, 1, 1, 0, 1, 0, 0,
            0, 1, 1, 1, 1, 0, 1, 1, 1, 1,
        };

        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(xseries, xseries, 5, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.031471,
                inform_transfer_entropy(yseries, xseries, 5, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.152561,
                inform_transfer_entropy(xseries, yseries, 5, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(yseries, yseries, 5, 10, 2, 2, NULL), 1e-6);

        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(xseries, xseries, 4, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.172618,
                inform_transfer_entropy(yseries, xseries, 4, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.206156,
                inform_transfer_entropy(xseries, yseries, 4, 10, 2, 2, NULL), 1e-6);
        ASSERT_DBL_NEAR_TOL(0.000000,
                inform_transfer_entropy(yseries, yseries, 4, 10, 2, 2, NULL), 1e-6);
    }
}

UNIT(LocalTransferEntropyNULLSeries)
{
    double te[8];
    int const series[] = {1,1,0,0,1,0,0,1};

    inform_error err = INFORM_SUCCESS;
    ASSERT_NULL(inform_local_transfer_entropy(NULL, series, 1, 3, 2, 2, te, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_ETIMESERIES, err);

    err = INFORM_SUCCESS;
    ASSERT_NULL(inform_local_transfer_entropy(series, NULL, 1, 3, 2, 2, te, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_ETIMESERIES, err);
}

UNIT(LocalTransferEntropyNoInits)
{
    double te[8];
    int const series[] = {1,1,0,0,1,0,0,1};

    inform_error err = INFORM_SUCCESS;
    ASSERT_NULL(inform_local_transfer_entropy(series, series, 0, 3, 2, 2, te, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_ENOINITS, err);
}

UNIT(LocalTransferEntropySeriesTooShort)
{
    double te[8];
    int const series[] = {1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;

    for (size_t i = 0; i < 2; ++i)
    {
        err = INFORM_SUCCESS;
        ASSERT_NULL(inform_local_transfer_entropy(series, series, 1, i, 2, 2, te, &err));
        ASSERT_TRUE(inform_failed(&err));
        ASSERT_EQUAL(INFORM_ESHORTSERIES, err);
    }
}

UNIT(LocalTransferEntropyHistoryTooLong)
{
    double te[8];
    int const series[] = {1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;
    for (size_t i = 2; i < 4; ++i)
    {
        err = INFORM_SUCCESS;
        ASSERT_NULL(inform_local_transfer_entropy(series, series, 1, 2, 2, i, te, &err));
        ASSERT_TRUE(inform_failed(&err));
        ASSERT_EQUAL(INFORM_EKLONG, err);
    }
}

UNIT(LocalTransferEntropyZeroHistory)
{
    double te[8];
    int const series[] = {1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;
    ASSERT_NULL(inform_local_transfer_entropy(series, series, 1, 2, 2, 0, te, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_EKZERO, err);
}

UNIT(LocalTransferEntropyInvalidBase)
{
    double te[8];
    int const series[] = {1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;
    for (int i = 0; i < 2; ++i)
    {
        err = INFORM_SUCCESS;
        ASSERT_NULL(inform_local_transfer_entropy(series, series, 1, 2, i, 2, te, &err));
        ASSERT_TRUE(inform_failed(&err));
        ASSERT_EQUAL(INFORM_EBASE, err);
    }
}

UNIT(LocalTransferEntropyNegativeState)
{
    double te[8];
    int const seriesA[] = { 1,1,0,0,1,0,0,1};
    int const seriesB[] = {-1,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;

    ASSERT_NULL(inform_local_transfer_entropy(seriesA, seriesB, 1, 8, 2, 2, te, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_ENEGSTATE, err);

    err = INFORM_SUCCESS;
    ASSERT_NULL(inform_local_transfer_entropy(seriesB, seriesA, 1, 8, 2, 2, te, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_ENEGSTATE, err);
}

UNIT(LocalTransferEntropyBadState)
{
    double te[8];
    int const seriesA[] = {1,1,0,0,1,0,0,1};
    int const seriesB[] = {2,1,0,0,1,0,0,1};
    inform_error err = INFORM_SUCCESS;

    ASSERT_NULL(inform_local_transfer_entropy(seriesA, seriesB, 1, 8, 2, 2, te, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_EBADSTATE, err);

    err = INFORM_SUCCESS;
    ASSERT_NULL(inform_local_transfer_entropy(seriesB, seriesA, 1, 8, 2, 2, te, &err));
    ASSERT_TRUE(inform_failed(&err));
    ASSERT_EQUAL(INFORM_EBADSTATE, err);
}

UNIT(LocalTransferEntropyAllocatesOutput)
{
    inform_error err = INFORM_SUCCESS;
    int source[] = {0,0,1,0,0,1,0,1,0};
    int target[] = {0,1,0,0,1,0,0,1,0};
    double *te = inform_local_transfer_entropy(source, target, 1, 9, 2, 2, NULL, &err);
    ASSERT_NOT_NULL(te);
    ASSERT_EQUAL(INFORM_SUCCESS, err);
    free(te);
}

UNIT(LocalTransferEntropySingleSeries_Base2)
{
    {
        double te[3];
        int series[10] = {
            1,1,1,0,0,
            1,1,0,0,1
        };
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series, series, 1, 5, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series+5, series, 1, 5, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.666666, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series, series+5, 1, 5, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series+5, series+5, 1, 5, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
    }

    {
        double te[8];
        int series[20] = {
            0,0,1,1,1,0,0,0,0,1,
            1,1,0,0,0,0,0,0,1,1
        };
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series, series, 1, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series+10, series, 1, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.500000, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series, series+10, 1, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.106844, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series+10, series+10, 1, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
    }

    {
        double te[8];
        int series[20] = {
            0,1,0,1,0,0,1,1,0,0,
            0,0,1,0,1,1,1,0,1,1
        };
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series, series, 1, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series+10, series, 1, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.344361, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series, series+10, 1, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.250000, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(series+10, series+10, 1, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
    }
}

UNIT(LocalTransferEntropyEnsemble_Base2)
{
    {
        double te[40];
        int xseries[50] = {
            1, 1, 1, 0, 0, 1, 1, 0, 1, 0,
            0, 1, 0, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
            0, 0, 1, 0, 0, 0, 1, 0, 0, 1,
            0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        };
        int yseries[50] = {
            0, 1, 0, 0, 0, 1, 0, 1, 1, 0,
            0, 0, 0, 1, 1, 1, 0, 1, 0, 0,
            1, 0, 1, 0, 1, 0, 0, 0, 1, 0,
            0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
            0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
        };

        ASSERT_NOT_NULL(inform_local_transfer_entropy(xseries, xseries, 5, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(yseries, xseries, 5, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.091141, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(xseries, yseries, 5, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.107630, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(yseries, yseries, 5, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
    }
    {
        double te[32];
        int xseries[40] = {
            1, 1, 1, 0, 0, 1, 1, 0, 1, 0,
            0, 1, 0, 1, 1, 1, 0, 0, 0, 0,
            0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
            0, 0, 1, 0, 0, 0, 1, 0, 0, 1,
        };
        int yseries[40] = {
            0, 1, 0, 0, 0, 1, 0, 1, 1, 0,
            0, 0, 0, 1, 1, 1, 0, 1, 0, 0,
            1, 0, 1, 0, 1, 0, 0, 0, 1, 0,
            0, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        };

        ASSERT_NOT_NULL(inform_local_transfer_entropy(xseries, xseries, 4, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(yseries, xseries, 4, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.134536, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(xseries, yseries, 4, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.089517, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(yseries, yseries, 4, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
    }
    {
        double te[40];
        int xseries[50] = {
            0, 1, 0, 1, 0, 0, 1, 1, 1, 1,
            0, 1, 0, 1, 1, 1, 0, 0, 1, 0,
            1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
            0, 1, 1, 1, 1, 1, 0, 1, 1, 1,
        };
        int yseries[50] = {
            1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
            0, 1, 1, 0, 0, 1, 1, 0, 0, 0,
            0, 1, 1, 1, 0, 1, 0, 0, 0, 0,
            0, 1, 0, 0, 1, 1, 0, 1, 0, 0,
            0, 1, 1, 1, 1, 0, 1, 1, 1, 1,
        };

        ASSERT_NOT_NULL(inform_local_transfer_entropy(xseries, xseries, 5, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(yseries, xseries, 5, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.031471, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(xseries, yseries, 5, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.152561, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(yseries, yseries, 5, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
    }
    {
        double te[32];
        int xseries[50] = {
            0, 1, 0, 1, 0, 0, 1, 1, 1, 1,
            0, 1, 0, 1, 1, 1, 0, 0, 1, 0,
            1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
            0, 1, 1, 1, 1, 1, 0, 1, 1, 1,
        };
        int yseries[50] = {
            1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
            0, 1, 1, 0, 0, 1, 1, 0, 0, 0,
            0, 1, 1, 1, 0, 1, 0, 0, 0, 0,
            0, 1, 0, 0, 1, 1, 0, 1, 0, 0,
            0, 1, 1, 1, 1, 0, 1, 1, 1, 1,
        };

        ASSERT_NOT_NULL(inform_local_transfer_entropy(xseries, xseries, 4, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(yseries, xseries, 4, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.172618, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(xseries, yseries, 4, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.206156, AVERAGE(te), 1e-6);
        ASSERT_NOT_NULL(inform_local_transfer_entropy(yseries, yseries, 4, 10, 2, 2, te, NULL));
        ASSERT_DBL_NEAR_TOL(0.000000, AVERAGE(te), 1e-6);
    }
}

BEGIN_SUITE(TransferEntropy)
    ADD_UNIT(TransferEntropyNULLSeries)
    ADD_UNIT(TransferEntropyNoInits)
    ADD_UNIT(TransferEntropySeriesTooShort)
    ADD_UNIT(TransferEntropyHistoryTooLong)
    ADD_UNIT(TransferEntropyZeroHistory)
    ADD_UNIT(TransferEntropyInvalidBase)
    ADD_UNIT(TransferEntropyNegativeState)
    ADD_UNIT(TransferEntropyBadState)
    ADD_UNIT(TransferEntropySingleSeries_Base2)
    ADD_UNIT(TransferEntropyEnsemble_Base2)
    ADD_UNIT(LocalTransferEntropyNULLSeries)
    ADD_UNIT(LocalTransferEntropyNoInits)
    ADD_UNIT(LocalTransferEntropySeriesTooShort)
    ADD_UNIT(LocalTransferEntropyHistoryTooLong)
    ADD_UNIT(LocalTransferEntropyZeroHistory)
    ADD_UNIT(LocalTransferEntropyInvalidBase)
    ADD_UNIT(LocalTransferEntropyNegativeState)
    ADD_UNIT(LocalTransferEntropyBadState)
    ADD_UNIT(LocalTransferEntropyAllocatesOutput)
    ADD_UNIT(LocalTransferEntropySingleSeries_Base2)
    ADD_UNIT(LocalTransferEntropyEnsemble_Base2)
END_SUITE
