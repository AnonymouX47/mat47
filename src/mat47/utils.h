/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENCE for license information.
 */

#include <stdio.h>

#ifndef MAT47_UTILS
#define MAT47_UTILS

#define mat47_log(msg, ...) (MAT47_DEBUG && \
    fprintf( \
        (MAT47_LOG_FILE ? MAT47_LOG_FILE : stderr), \
        "[DEBUG] " __FILE__ ":%d: %s: " msg "\n", \
        __LINE__, __func__, ##__VA_ARGS__ \
    )\
)

extern _Bool MAT47_DEBUG;
extern FILE *MAT47_LOG_FILE;

#endif
