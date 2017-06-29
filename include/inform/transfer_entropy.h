// Copyright 2016-2017 ELIFE. All rights reserved.
// Use of this source code is governed by a MIT
// license that can be found in the LICENSE file.
#pragma once

#include <inform/error.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Compute the transfer entropy from one time series to another
 *
 * @param[in] series_y the ensemble of the source node
 * @param[in] series_x the ensemble of the target node
 * @param[in] n        the number initial conditions
 * @param[in] m        the number of time steps in each time series
 * @param[in] b        the base or number of distinct states at each time step
 * @param[in] k        the history length used to calculate the transfer entropy
 * @param[out] err     an error structure
 * @return the transfer entropy of the ensemble
 */
EXPORT double inform_transfer_entropy(int const *series_y, int const *series_x,
    size_t n, size_t m, int b, size_t k, inform_error *err);

/**
 * Compute the local transfer entropy from one time series to another
 *
 * @param[in] series_y the ensemble of the source node
 * @param[in] series_x the ensemble of the target node
 * @param[in] n        the number initial conditions
 * @param[in] m        the number of time steps in each time series
 * @param[in] b        the base or number of distinct states at each time step
 * @param[in] k        the history length used to calculate the transfer entropy
 * @param[out] te      the transfer entropy
 * @param[out] err     an error structure
 * @return a pointer to the transfer entropy array
 */
EXPORT double *inform_local_transfer_entropy(int const *series_y, int const *series_x,
    size_t n, size_t m, int b, size_t k, double *te, inform_error *err);

/**
 * Compute the local transfer entropy from one time series to another
 *
 * @param[in] series_y the ensemble of the source node
 * @param[in] series_x the ensemble of the target node
 * @param[in] n        the number initial conditions
 * @param[in] m        the number of time steps in each time series
 * @param[in] b        the base or number of distinct states at each time step
 * @param[in] k        the history length used to calculate the transfer entropy
 * @param[out] te      the transfer entropy
 * @param[out] err     an error structure
 * @return a pointer to the transfer entropy array
 */
EXPORT double *inform_local_transfer_entropy2(int const *series_y, int const *series_x,
    size_t n, size_t m, int b, size_t k, double *te, inform_error *err);

#ifdef __cplusplus
}
#endif
