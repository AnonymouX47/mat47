/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENCE for license information.
 */

#include <stdio.h>

#ifndef MAT47
#define MAT47

/* Enables/Disables library-wide debugging (disabled by default). */
extern _Bool MAT47_DEBUG;

/* Stream to which libray logs should be written. If NULL, logs are written to `stderr`.
 *
 * NOTE: Only used when debugging is enabled via `MAT47_DEBUG`.
 */
extern FILE *MAT47_LOG_FILE;

#endif
