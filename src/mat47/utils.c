/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENCE for license information.
 */

#include "utils.h"

_Bool MAT47_DEBUG;
FILE *MAT47_LOG_FILE = NULL;

#define _sum(n, arr) \
    intmax_t sum = 0; \
    while (n--) sum += arr[n]; \
    return sum;

intmax_t sum8(unsigned int n, int8_t arr[]) {_sum(n, arr)}
intmax_t sum16(unsigned int n, int16_t arr[]) {_sum(n, arr)}
intmax_t sum32(unsigned int n, int32_t arr[]) {_sum(n, arr)}
intmax_t sum64(unsigned int n, int64_t arr[]) {_sum(n, arr)}

uintmax_t usum8(unsigned int n, uint8_t arr[]) {_sum(n, arr)}
uintmax_t usum16(unsigned int n, uint16_t arr[]) {_sum(n, arr)}
uintmax_t usum32(unsigned int n, uint32_t arr[]) {_sum(n, arr)}
uintmax_t usum64(unsigned int n, uint64_t arr[]) {_sum(n, arr)}
