/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENSE for license information.
 */

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "matrix.h"
#include "utils.h"

#define uint unsigned int  // Used only where necessary, to avoid long lines

_Thread_local unsigned int mat47_errno;


/**
 * Allocates memory for a new matrix.
 *
 * Args:
 *     n_rows: Number of rows
 *     n_cols: Number of columns
 *     zero: If true, all the matrix' elements are initialized to 0.0.
 *       Otherwise, the elements are uninitialized.
 *
 * Returns:
 *     - A null pointer, if either dimension equals zero or a failure occurs during
 *       memory allocation.
 *     - Otherwise, a pointer to a newly allocated matrix.
 *
 * Raises:
 *     MAT47_ERR_ZERO_SIZE: Either dimension equals zero.
 *     MAT47_ERR_ALLOC: Unable to allocate memory.
 *
 * Note:
 *     Allocation of zeroed memory takes longer (tested).
 */
static mat47_t *mat47_new(unsigned int n_rows, unsigned int n_cols, bool zero)
{
    double **restrict data;

    if (!(n_rows && n_cols)) {
        mat47_errno = MAT47_ERR_ZERO_SIZE;
        error(": %u x %u", n_rows, n_cols);
        return NULL;
    }

    mat47_t *m;
    if (!(m = malloc(sizeof(mat47_t)))) {
        mat47_errno = MAT47_ERR_ALLOC;
        error(" for matrix object");
        return NULL;
    }

    debug("Allocated matrix @ %p", (void *)m);

    m->n_rows = n_rows;
    m->n_cols = n_cols;
    // Using `calloc()` to ensure all pointer are NULL, in case row allocation fails
    if (!(m->data = data = calloc(sizeof(double *), n_rows))) {
        free(m);
        mat47_errno = MAT47_ERR_ALLOC;
        error(" for row pointers");
        return NULL;
    }

    debug("Allocated row pointers");

    for (unsigned int i = 0; i < n_rows; i++)
        if (!(data[i] = (
            zero ? calloc(sizeof(double), n_cols) : malloc(sizeof(double) * n_cols)
        ))) {
            mat47_del(m);
            mat47_errno = MAT47_ERR_ALLOC;
            error(" for rows");
            return NULL;
        }

    debug("Allocated rows, zero=%u", zero);
    debug(
        "[1, 1] = %f, [%d, %d] = %f",
        data[0][0], n_rows, n_cols, data[n_rows - 1][n_cols - 1]
    );

    return m;
}


mat47_t *mat47_zero(unsigned int n_rows, unsigned int n_cols)
{
    return mat47_new(n_rows, n_cols, true);
}


#define init \
    mat47_t *m; \
    double **restrict data, *restrict data_row; \
    restrict typeof(*array) row; \
\
    if (check_ptr(array)) return NULL; \
    if (!(m = mat47_new(n_rows, n_cols, false))) return NULL; \
\
    data = m->data; \
    while (n_rows--) { \
        if (!(row = array[n_rows])) { \
            mat47_del(m); \
            mat47_errno = MAT47_ERR_NULL_PTR; \
            error(": `array[%u]`", n_rows); \
            return NULL; \
        } \
        data_row = data[n_rows]; \
        for (unsigned int j = 0; j < n_cols; j++) \
            data_row[j] = row[j]; \
    } \
\
    return m;

mat47_t *mat47_init_int8(uint n_rows, uint n_cols, int8_t **restrict array)
{
    init
}

mat47_t *mat47_init_int16(uint n_rows, uint n_cols, int16_t **restrict array)
{
    init
}

mat47_t *mat47_init_int32(uint n_rows, uint n_cols, int32_t **restrict array)
{
    init
}

mat47_t *mat47_init_int64(uint n_rows, uint n_cols, int64_t **restrict array)
{
    init
}

mat47_t *mat47_init_uint8(uint n_rows, uint n_cols, uint8_t **restrict array)
{
    init
}

mat47_t *mat47_init_uint16(uint n_rows, uint n_cols, uint16_t **restrict array)
{
    init
}

mat47_t *mat47_init_uint32(uint n_rows, uint n_cols, uint32_t **restrict array)
{
    init
}

mat47_t *mat47_init_uint64(uint n_rows, uint n_cols, uint64_t **restrict array)
{
    init
}

mat47_t *mat47_init_float(uint n_rows, uint n_cols, float **restrict array)
{
    init
}

#undef init

mat47_t *mat47_init_double(uint n_rows, uint n_cols, double **restrict array)
{
    mat47_t *m;
    double **restrict data, *restrict row;

    if (check_ptr(array)) return NULL;
    if (!(m = mat47_new(n_rows, n_cols, false))) return NULL;

    data = m->data;
    while (n_rows--) {
        if (!(row = array[n_rows])) {
            mat47_del(m);
            mat47_errno = MAT47_ERR_NULL_PTR;
            error(": `array[%u]`", n_rows);
            return NULL;
        }
        memcpy(data[n_rows], row, sizeof(double) * n_cols);
    }

    return m;
}


mat47_t *mat47_copy(const mat47_t *m)
{
    if (check_ptr(m) || check_ptr(m->data)) return NULL;
    return mat47_init_double(m->n_rows, m->n_cols, m->data);
}


void mat47_del(mat47_t *m)
{
    if (m) {
        if (m->data) {
            double **restrict data = m->data;
            unsigned int n_rows = m->n_rows;

            for (unsigned int i = 0; i < n_rows; i++) free(data[i]);
            free(m->data);
        }
        debug("Deallocated matrix @ %p", (void *)m);
        free(m);
    }
}


double mat47_get_elem(const mat47_t *m, unsigned int row, unsigned int col)
{
    if (check_ptr(m) || check_ptr(m->data)) return NAN;  // Null pointers
    if (check_row(m, row) || check_col(m, col)) return NAN;  // Out-of-range indexes
    // Null pointer (but the index must be validated first)
    if (check_ptr(m->data[row - 1])) return NAN;

    return m->data[row - 1][col - 1];
}


void mat47_set_elem(mat47_t *m, unsigned int row, unsigned int col, double value)
{
    if (check_ptr(m) || check_ptr(m->data)) return;  // Null pointers
    if (check_row(m, row) || check_col(m, col)) return;  // Out-of-range indexes
    // Null pointer (but the index must be validated first)
    if (check_ptr(m->data[row - 1])) return;

    m->data[row - 1][col - 1] = value;
}


mat47_t *
mat47_get_submat
(const mat47_t *m, unsigned top, unsigned left, unsigned bottom, unsigned right)
{
    long n_rows, n_cols;
    double **restrict data, **restrict sub_data, *restrict row;
    mat47_t *sub;

    // Null pointers
    if (check_ptr(m) || check_ptr(m->data)) return NULL;

    // Out-of-range indexes
    if (
        check_row(m, top)
        || check_col(m, left)
        || check_row(m, bottom)
        || check_col(m, right)
    ) return NULL;

    // Empty sub-matrix
    if ((n_rows = (long)bottom - top + 1) < 1) {
        mat47_errno = MAT47_ERR_ZERO_SIZE;
        error(": top=%u, bottom=%u", top, bottom);
        return NULL;
    }
    if ((n_cols = (long)right - left + 1) < 1) {
        mat47_errno = MAT47_ERR_ZERO_SIZE;
        error(": left=%u, right=%u", left, right);
        return NULL;
    }

    if (!(sub = mat47_new(n_rows, n_cols, false))) return NULL;

    data = m->data;
    sub_data = sub->data;
    --top; --left;  // Change to zero-based
    for (unsigned int i = 0; i < n_rows; i++) {
        if (!(row = data[top + i])) {
            mat47_del(sub);
            mat47_errno = MAT47_ERR_NULL_PTR;
            error(": `m->data[%u]`", top + i);
            return NULL;
        }
        memcpy(sub_data[i], row + left, sizeof(double) * n_cols);
    }

    return sub;
}


#define ELEM_MAX_LEN 24
#define ELEM_MAX_LEN_NDIGITS 2

intmax_t mat47_fprintf(
    const mat47_t *m, FILE *restrict stream, const char *restrict format
) {
    if (check_ptr(m) || check_ptr(stream) || check_ptr(format)) return -1;
    if (!*format) {
        mat47_errno = MAT47_ERR_ZERO_SIZE;
        error(": empty element format string");
        return -1;
    }

    double *restrict row, **restrict data = m->data;
    unsigned int i, j, n_rows = m->n_rows, n_cols = m->n_cols;
    intmax_t n_bytes = 0;

    debug("Printing matrix @ %p; n_rows=%u, n_cols=%u", (void *)m, n_rows, n_cols);

    char *mat_str[n_rows],
         (*row_str)[ELEM_MAX_LEN + 1],
         // 6 = '|' + ' ' + '%' + 's' + ' ' + '\0'
         col_fmt[n_cols][ELEM_MAX_LEN_NDIGITS + 6];
    unsigned char col_widths[n_cols];

    // Allocating rows separately to avoid failures for large matrices.
    // Also, **not** allocating per element reduces the number of allocations and
    // helps to avoid an extra level of indirection.
    for (i = n_rows; i--;) {
        if (!(mat_str[i] = malloc((ELEM_MAX_LEN + 1) * sizeof(char) * n_cols))) {
            while (++i < n_rows) free(mat_str[i]);  // Deallocate allocated rows
            mat47_errno = MAT47_ERR_ALLOC;
            error(" for element strings");
            return -1;
        }
    }

    debug("Allocated memory to print matrix");

    memset(col_widths, 0, n_cols * sizeof(*col_widths));
    for (i = 0; i < n_rows; i++) {
        row = data[i];
        row_str = (char (*)[ELEM_MAX_LEN + 1])mat_str[i];
        for (j = 0; j < n_cols; j++)
            imax(
                col_widths[j],
                min(
                    ELEM_MAX_LEN,
                    snprintf(row_str[j], ELEM_MAX_LEN + 1, format, row[j])
                )
            );
    }

    debug(
        "Formatted elements: "
        "[1, 1] = \"%s\" (%f), [1, 2] = \"%s\" (%f), [%d, %d] = \"%s\" (%f)",
        mat_str[0], data[0][0],
        (m->n_rows > 1 ? mat_str[0] + (ELEM_MAX_LEN + 1) : "Out of range"),
        (m->n_rows > 1 ? data[0][1] : NAN),
        n_rows, n_cols,
        mat_str[n_rows - 1] + ((n_cols - 1) * (ELEM_MAX_LEN + 1)),
        data[n_rows - 1][n_cols - 1]
    );

    for (j = 0; j < n_cols; j++)
        sprintf(col_fmt[j], "| %%%ds ", col_widths[j]);

    debug(
        "Created column format strings: [1] = %s, [%d] = %s",
        col_fmt[0], n_cols, col_fmt[n_cols - 1]
    );

    // First 3 = '|'/'+' + 2 * ' ' (per column)
    // Second 3 = '|'/'+' + "\n\0" (at the end of every line)
    unsigned int line_length = sum(n_cols, col_widths) + 3 * n_cols + 3;
    char bar[line_length], mid_bar[line_length], *line;

    line = bar;
    *line++ = '+';
    // 4 = 2 * '+' + "\n\0"
    memset(line, '-', line_length - 4); line += line_length - 4;
    *line++ = '+';
    strcpy(line, "\n\0");

    line = mid_bar;
    *line++ = '|';
    for (j = 0; j < n_cols; j++) {
        // 2 = 2 * ' ' (surrounding every **formatted** element)
        memset(line, '-', col_widths[j] + 2); line += col_widths[j] + 2;
        *line++ = '+';
    }
    strcpy(line - 1, "|\n\0");  // Overwrites the last '+' (line - 1)

    debug("Writing to file");

    n_bytes += fputs(bar, stream);
    for (i = 0; i < n_rows; i++) {
        row_str = (char (*)[ELEM_MAX_LEN + 1])mat_str[i];
        for (j = 0; j < n_cols; j++)
            n_bytes += fprintf(stream, col_fmt[j], row_str[j]);
        n_bytes += fprintf(stream, "|\n");
        if (i < n_rows - 1) n_bytes += fputs(mid_bar, stream);
    }
    n_bytes += fputs(bar, stream);

    for (i = n_rows; i--;) free(mat_str[i]);
    debug("Deallocated memory for element strings");

    return n_bytes;
}


char *mat47_strerror(unsigned int errnum)
{
    static char *error_str[] = {
        [1] = "Unable to allocate memory",
        "Invalid zero-sized/empty result",
        "Null pointer argument(s)",
        "Index out of range",
    };

    if (errnum >= sizeof_arr(error_str)) errnum = 0;
    return error_str[errnum];
}
