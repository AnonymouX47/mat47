/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENCE for license information.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"
#include "utils.h"


/* Allocates memory for a new matrix.
 *
 * Args:
 *     n_rows: Number of rows; 1 <= n_rows <= UINT_MAX
 *     n_cols: Number of columns; 1 <= n_cols <= UINT_MAX
 *     zero: If true, all the matrix' elements are initialized to 0.0.
 *       Otherwise, the elements are uninitialized.
 *
 * Returns:
 *     - A null pointer, if either argument equals zero or a failure occurs during
 *       memory allocation.
 *     - Otherwise, a pointer to a newly allocated matrix.
 *
 * Note:
 *     Allocation of zeroed memory takes longer (tested).
 */
static mat47_t *mat47_new(unsigned int n_rows, unsigned int n_cols, bool zero)
{
    double **data;

    if (!(n_rows && n_cols)) {
        mat47_log("Zero size: %u x %u", n_rows, n_cols);
        return NULL;
    }

    mat47_t *m;
    if (!(m = malloc(sizeof(mat47_t)))) {
        mat47_log("Failed to allocate matrix");
        return NULL;
    }

    mat47_log("Allocated matrix @ %p", m);

    m->n_rows = n_rows;
    m->n_cols = n_cols;
    // Using `calloc()` to ensure all pointer are NULL, in case row allocation fails
    if (!(m->data = calloc(sizeof(double *), n_rows))) {
        free(m);
        mat47_log("Failed to allocate row pointers");
        return NULL;
    }

    mat47_log("Allocated row pointers");

    data = m->data;
    for (unsigned int i = 0; i < n_rows; i++)
        if (!(data[i] = (
            zero ? calloc(sizeof(double), n_cols) : malloc(sizeof(double) * n_cols)
        ))) {
            mat47_del(m);
            mat47_log("Failed to allocate rows");
            return NULL;
        }

    mat47_log("Allocated rows, zero=%u", zero);
    mat47_log(
        "[1, 1] = %f, [%d, %d] = %f",
        data[0][0], n_rows, n_cols, data[n_rows - 1][n_cols - 1]
    );

    return m;
}


mat47_t *mat47_zero(unsigned int n_rows, unsigned int n_cols)
{
    return mat47_new(n_rows, n_cols, true);
}


void *mat47_del(mat47_t *m)
{
    if (m) {
        if (m->data) {
            double **data = m->data;
            unsigned int n_rows = m->n_rows;

            for (unsigned int i = 0; i < n_rows; i++) free(data[i]);
            free(m->data);
        }
        free(m);
        mat47_log("Deallocated matrix @ %p", m);
    }
}


#define ELEM_MAX_LEN 24
#define ELEM_MAX_LEN_NDIGITS 2

intmax_t mat47_fprintf(mat47_t *m, FILE *restrict stream, const char *restrict format)
{
    if (!(m && stream && format)) {
        mat47_log("Null pointer(s): m=%p, stream=%p, format=%p", m, stream, format);
        return -1;
    }

    double *restrict row, **restrict data = m->data;
    unsigned int i, j, n_rows = m->n_rows, n_cols = m->n_cols;
    char (*row_str)[ELEM_MAX_LEN + 1],  // 1 = '\0'
         mat_str[n_rows][n_cols][ELEM_MAX_LEN + 1],  // 1 = '\0'
         // 6 = '|' + ' ' + '%' + 's' + ' ' + '\0'
         col_fmt[n_cols][ELEM_MAX_LEN_NDIGITS + 6];
    unsigned char col_widths[n_cols];
    intmax_t n_bytes = 0;

    mat47_log("Printing matrix @ %p", m);

    memset(col_widths, 0, n_cols * sizeof(*col_widths));
    for (i = 0; i < n_rows; i++) {
        row = data[i];
        row_str = mat_str[i];
        for (j = 0; j < n_cols; j++)
            imax(
                col_widths[j],
                min(
                    ELEM_MAX_LEN,
                    snprintf(row_str[j], ELEM_MAX_LEN + 1, format, row[j])
                )
            );
    }

    mat47_log("Formatted elements");
    mat47_log(
        "[1, 1] = %s, [%d, %d] = %s",
        mat_str[0][0], n_rows, n_cols, mat_str[n_rows - 1][n_cols - 1]
    );

    for (j = 0; j < n_cols; j++)
        sprintf(col_fmt[j], "| %%%ds ", col_widths[j]);

    mat47_log("Created column format strings");
    mat47_log(
        "[1] = %s, [%d] = %s",
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

    n_bytes += fprintf(stream, bar);
    for (i = 0; i < n_rows; i++) {
        row_str = mat_str[i];
        for (j = 0; j < n_cols; j++)
            n_bytes += fprintf(stream, col_fmt[j], row_str[j]);
        n_bytes += fprintf(stream, "|\n");
        if (i < n_rows - 1) n_bytes += fprintf(stream, mid_bar);
    }
    n_bytes += fprintf(stream, bar);

    return n_bytes;
}
