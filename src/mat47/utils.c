/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENSE for license information.
 */

#include <stdbool.h>
#include <stdio.h>
#include <time.h>

FILE *MAT47_LOG_FILE;

int mat47__return_zero(void)
{
    return 0;
}

char *mat47__get_timestamp(void)
{
    static _Thread_local struct timespec log_timespec;
    static _Thread_local struct tm log_tm;
    static _Thread_local time_t log_time;
    static _Thread_local char time_str[19] = {0};  // HH:MM:SS.NNNNNNNNN\0

    log_time = time(NULL);
    strftime(time_str, sizeof(time_str), "%T", localtime_r(&log_time, &log_tm));
    snprintf(
        time_str + 8, sizeof(time_str) - 8, ".%09ld",
        (timespec_get(&log_timespec, TIME_UTC) ? log_timespec.tv_nsec : 0)
    );

    return time_str;
}
