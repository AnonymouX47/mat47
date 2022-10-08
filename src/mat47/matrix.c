/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENCE for license information.
 */

#include <stdlib.h>

#include "matrix.h"
#include "utils.h"


mat47_t *mat47_new(unsigned int n_rows, unsigned int n_cols)
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
        if (!(data[i] = calloc(sizeof(double), n_cols))) {
            mat47_del(m);
            mat47_log("Failed to allocate rows");
            return NULL;
        }

    mat47_log("Allocated rows");
    mat47_log(
        "[1, 1] = %f, [%d, %d] = %f",
        data[0][0], n_rows, n_cols, data[n_rows - 1][n_cols - 1]
    );

    return m;
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
