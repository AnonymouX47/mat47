/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENCE for license information.
 */

#include <stdint.h>
#include <stdio.h>

#ifndef MAT47_UTILS_H
#define MAT47_UTILS_H

#define mat47_log(msg, ...) (MAT47_DEBUG && \
    fprintf( \
        (MAT47_LOG_FILE ? MAT47_LOG_FILE : stderr), \
        "[DEBUG] " __FILE__ ":%d: %s: " msg "\n", \
        __LINE__, __func__, ##__VA_ARGS__ \
    )\
)

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define imax(a, b) (a = max(a, b))

#define sum(n, arr) \
    _Generic( \
        (arr), \
        int8_t *: sum8, int16_t *: sum16, int32_t *: sum32, int64_t *: sum64, \
        uint8_t *: usum8, uint16_t *: usum16, uint32_t *: usum32, uint64_t *: usum64 \
    )(n, arr)

extern _Bool MAT47_DEBUG;
extern FILE *MAT47_LOG_FILE;

#define _sum(n, arr) \
    intmax_t sum = 0; \
    while (n--) sum += arr[n]; \
    return sum;

static intmax_t sum8(unsigned int n, int8_t arr[]) {_sum(n, arr)}
static intmax_t sum16(unsigned int n, int16_t arr[]) {_sum(n, arr)}
static intmax_t sum32(unsigned int n, int32_t arr[]) {_sum(n, arr)}
static intmax_t sum64(unsigned int n, int64_t arr[]) {_sum(n, arr)}

static uintmax_t usum8(unsigned int n, uint8_t arr[]) {_sum(n, arr)}
static uintmax_t usum16(unsigned int n, uint16_t arr[]) {_sum(n, arr)}
static uintmax_t usum32(unsigned int n, uint32_t arr[]) {_sum(n, arr)}
static uintmax_t usum64(unsigned int n, uint64_t arr[]) {_sum(n, arr)}

#endif  // MAT47_UTILS_H
