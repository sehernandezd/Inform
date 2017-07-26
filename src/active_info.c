// Copyright 2016-2017 ELIFE. All rights reserved.
// Use of this source code is governed by a MIT
// license that can be found in the LICENSE file.
#include <inform/active_info.h>
#include <inform/shannon.h>
#include <string.h>

static void accumulate_observations(int const* series, size_t n, size_t m,
    int b, size_t k, inform_dist *states, inform_dist *histories,
    inform_dist *futures)
{
    for (size_t i = 0; i < n; ++i, series += m)
    {
        int history = 0, q = 1, state, future;
        for (size_t j = 0; j < k; ++j)
        {
            q *= b;
            history *= b;
            history += series[j];
        }
        for (size_t j = k; j < m; ++j)
        {
            future = series[j];
            state  = history * b + future;

            states->histogram[state]++;
            histories->histogram[history]++;
            futures->histogram[future]++;

            history = state - series[j - k]*q;
        }
    }
}

static void accumulate_local_observations(int const* series, size_t n, size_t m,
    int b, size_t k, inform_dist *states, inform_dist *histories,
    inform_dist *futures, int *state, int *history, int *future)
{
    for (size_t i = 0; i < n; ++i)
    {
        history[0] = 0;
        int q = 1;
        for (size_t j = 0; j < k; ++j)
        {
            q *= b;
            history[0] *= b;
            history[0] += series[j];
        }
        for (size_t j = k; j < m; ++j)
        {
            size_t l = j - k;
            future[l] = series[j];
            state[l] = history[l] * b + future[l];

            states->histogram[state[l]]++;
            histories->histogram[history[l]]++;
            futures->histogram[future[l]]++;

            if (j + 1 != m)
                history[l + 1] = state[l] - series[l]*q;
        }
        series += m;
        state += (m - k);
        history += (m - k);
        future += (m - k);
    }
}

static bool check_arguments(int const *series, size_t n, size_t m, int b, size_t k, inform_error *err)
{
    if (series == NULL)
    {
        INFORM_ERROR_RETURN(err, INFORM_ETIMESERIES, true);
    }
    else if (n < 1)
    {
        INFORM_ERROR_RETURN(err, INFORM_ENOINITS, true);
    }
    else if (m < 2)
    {
        INFORM_ERROR_RETURN(err, INFORM_ESHORTSERIES, true);
    }
    else if (b < 2)
    {
        INFORM_ERROR_RETURN(err, INFORM_EBASE, true);
    }
    else if (k == 0)
    {
        INFORM_ERROR_RETURN(err, INFORM_EKZERO, true);
    }
    else if (m <= k)
    {
        INFORM_ERROR_RETURN(err, INFORM_EKLONG, true);
    }
    for (size_t i = 0; i < n * m; ++i)
    {
        if (series[i] < 0)
        {
            INFORM_ERROR_RETURN(err, INFORM_ENEGSTATE, true);
        }
        else if (b <= series[i])
        {
            INFORM_ERROR_RETURN(err, INFORM_EBADSTATE, true);
        }
    }
    return false;
}

double inform_active_info(int const *series, size_t n, size_t m, int b, size_t k, inform_error *err)
{
    if (check_arguments(series, n, m, b, k, err)) return NAN;

    size_t const N = n * (m - k);

    size_t const states_size = (size_t) (b * pow((double) b,(double) k));
    size_t const histories_size = states_size / b;
    size_t const futures_size = b;
    size_t const total_size = states_size + histories_size + futures_size;

    uint32_t *data = calloc(total_size, sizeof(uint32_t));
    if (data == NULL)
    {
        INFORM_ERROR_RETURN(err, INFORM_ENOMEM, NAN);
    }

    inform_dist states    = { data, states_size, N };
    inform_dist histories = { data + states_size, histories_size, N };
    inform_dist futures   = { data + states_size + histories_size, futures_size, N };

    accumulate_observations(series, n, m, b, k, &states, &histories, &futures);

    double ai = inform_shannon_mi(&states, &histories, &futures, 2.0);

    free(data);

    return ai;
}

double *inform_local_active_info(int const *series, size_t n, size_t m, int b,
    size_t k, double *ai, inform_error *err)
{
    if (check_arguments(series, n, m, b, k, err)) return NULL;

    size_t const N = n * (m - k);

    bool allocate_ai = (ai == NULL);
    if (allocate_ai)
    {
        ai = malloc(N * sizeof(double));
        if (ai == NULL)
        {
            INFORM_ERROR_RETURN(err, INFORM_ENOMEM, NULL);
        }
    }

    size_t const states_size = (size_t) (b*pow((double) b,(double) k));
    size_t const histories_size = states_size / b;
    size_t const futures_size = b;
    size_t const total_size = states_size + histories_size + futures_size;

    uint32_t *histogram_data = calloc(total_size, sizeof(uint32_t));
    if (histogram_data == NULL)
    {
        if (allocate_ai) free(ai);
        INFORM_ERROR_RETURN(err, INFORM_ENOMEM, NULL);
    }
    inform_dist states    = { histogram_data, states_size, N };
    inform_dist histories = { histogram_data + states_size, histories_size, N };
    inform_dist futures   = { histogram_data + states_size + histories_size, futures_size, N };

    int *state_data = malloc(3 * N * sizeof(int));
    if (state_data == NULL)
    {
        if (allocate_ai) free(ai);
        free(histogram_data);
        INFORM_ERROR_RETURN(err, INFORM_ENOMEM, NULL);
    }
    int *state   = state_data;
    int *history = state + N;
    int *future  = history + N;

    accumulate_local_observations(series, n, m, b, k, &states, &histories,
        &futures, state, history, future);

    for (size_t i = 0; i < N; ++i)
    {
        ai[i] = inform_shannon_pmi(&states, &histories, &futures, state[i],
            history[i], future[i], 2.0);
    }

    free(state_data);
    free(histogram_data);

    return ai;
}

static void accumulate_local_observations2(int const* series, size_t n,
    size_t m, size_t t, int b, size_t k, inform_dist *states,
    inform_dist *histories, inform_dist *futures, int *state, int *history,
    int *future)
{
    // n, number of timeseries
    // m, number of timesteps in a timeseries
    // t, timestep of history (i)
  
    size_t history_idx = t;
    size_t future_idx = t + k;
    int q = 1;    
    for (size_t i = 0; i <n; ++i)
    {      
        // Compute history
        history[i] = 0;
        q = 1;
        for (size_t j = history_idx; j < history_idx + k; ++j)
        {
            q *= b;
            history[i] *= b;
            history[i] += series[j];
        }
        future[i] = series[future_idx];
        state[i] = history[i] * b + future[i];

        // Add observation
        states->histogram[state[i]]++;
        histories->histogram[history[i]]++;
        futures->histogram[future[i]]++;

        // Update indexes
        history_idx += m;
        future_idx  += m;	
    }
}

double *inform_local_active_info2(int const *series, size_t n, size_t m,
    int b, size_t k, double *ai, inform_error *err)
{
    
    if (check_arguments(series, n, m, b, k, err)) return NULL;

    size_t const N = n * (m - k);

    bool allocate_ai = (ai == NULL);
    if (allocate_ai)
    {
        ai = malloc(N * sizeof(double));
        if (ai == NULL)
        {
            INFORM_ERROR_RETURN(err, INFORM_ENOMEM, NULL);
        }
    }

    size_t const states_size = (size_t) (b*pow((double) b,(double) k));
    size_t const histories_size = states_size / b;
    size_t const futures_size = b;
    size_t const total_size = states_size + histories_size + futures_size;

    uint32_t *histogram_data = calloc(total_size, sizeof(uint32_t));
    if (histogram_data == NULL)
    {
        if (allocate_ai) free(ai);
        INFORM_ERROR_RETURN(err, INFORM_ENOMEM, NULL);
    }

    inform_dist states    = { histogram_data, states_size, n };
    inform_dist histories = { histogram_data + states_size, histories_size, n };
    inform_dist futures   = { histogram_data + states_size + histories_size, futures_size, n };

    int *state_data = malloc(3 * n * sizeof(int));
    if (state_data == NULL)
    {
        if (allocate_ai) free(ai);
        free(histogram_data);
        INFORM_ERROR_RETURN(err, INFORM_ENOMEM, NULL);
    }
    int *state   = state_data;
    int *history = state + n;
    int *future  = history + n;

    for (size_t i = 0; i < m-k; ++i)
    {
        accumulate_local_observations2(series, n, m, i, b, k, &states, &histories,
            &futures, state, history, future);

        for (size_t h = 0; h < n; ++h)
        {
            ai[i+h*(m-k)] = inform_shannon_pmi(&states, &histories, &futures, state[h],
                    history[h], future[h], 2.0);
        }
        
        memset(histogram_data, 0, total_size * sizeof(uint32_t));
    }

    free(state_data);
    free(histogram_data);

    return ai;
}
