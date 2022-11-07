/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENSE for license information.
 */

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

bool MAT47_LOG_DEBUG;
bool MAT47_LOG_ERROR;
FILE *MAT47_LOG_FILE;
_Thread_local struct timespec log_timespec;
_Thread_local char time_str[9];
