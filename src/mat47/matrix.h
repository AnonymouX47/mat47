/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENCE for license information.
 */

#include <stdint.h>
#include <stdio.h>

#ifndef MAT47_H
#define MAT47_H

/* The default element format specifier */
#define MAT47_ELEM_FMT "%.4g"

/* Like `mat47_fprintf()` but with *format* set to `MAT47_ELEM_FMT` */
#define mat47_fprint(m, stream) mat47_fprintf(m, stream, MAT47_ELEM_FMT)

/* Like `mat47_fprintf()` but with *stream* set to `stdout` */
#define mat47_printf(m, format) mat47_fprintf(m, stdout, format)

/* Like `mat47_printf()` but with *format* set to `MAT47_ELEM_FMT` */
#define mat47_print(m) mat47_printf(m, MAT47_ELEM_FMT)

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

/* Deallocates a matrix.
 *
 * PARAMETERS:
 * - m: The matrix to be deallocated
 *
 * RETURN VALUE:
 * - A null pointer, if either argument equals zero or a failure occurs during memory
 *   allocation.
 * - Otherwise, a pointer to a newly allocated matrix.
 *
 * NOTE: This must be called to deallocate the memory internally used by a matrix.
 */
void *mat47_del(mat47_t *m);

/* Writes a string representation of a matrix to a stream.
 *
 * PARAMETERS:
 * - m: The matrix whose representation should be written
 * - stream: A narrow-oriented stream to which the matrix representation should be
 *   written
 * - format: A valid format specifier for a `double`, to be applied to every element
 *
 * RETURN VALUE:
 * - `-1`, if any argument is null.
 * - Otherwise, the amount of characters written to the stream.
 *
 * NOTE:
 * - The string representation of every element has a length limit of 24 characters.
 * - If *format* is invalid for a `double` or not null-terminated, the behaviour is
 *   undefined.
 * - If the number of bytes written is greater than INTMAX_MAX, the return value is
 *   undefined.
 * - *stream* is not explicitly flushed.
 */
intmax_t mat47_fprintf(mat47_t *m, FILE *restrict stream, const char *restrict format);

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
