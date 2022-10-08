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

/* The matrix type */
typedef struct {
    unsigned int n_rows;
    unsigned int n_cols;
    double **data;
} mat47_t;

/* Creates a new zero matrix.
 *
 * PARAMETERS:
 * - n_rows: Number of rows; 1 <= n_rows <= UINT_MAX
 * - n_cols: Number of columns; 1 <= n_cols <= UINT_MAX
 *
 * RETURN VALUE:
 * - A null pointer, if either argument equals zero or a failure occurs during memory
 *   allocation.
 * - Otherwise, a pointer to a newly allocated matrix.
 */
mat47_t *mat47_new(unsigned int n_rows, unsigned int n_cols);

#endif
