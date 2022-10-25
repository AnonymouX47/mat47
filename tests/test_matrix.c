#include <criterion/criterion.h>

#include "../src/mat47/matrix.c"

/* new */

Test(new, zero_size)
{
    mat47_errno = 0;
    cr_assert_null(mat47_new(0, 1, false), "Zero `n_rows` is invalid");
    cr_assert_eq(
        mat47_errno, MAT47_ERR_ZERO_SIZE,
        "%u (%s) was raised", mat47_errno, mat47_strerror(mat47_errno)
    );

    mat47_errno = 0;
    cr_assert_null(mat47_new(1, 0, false), "Zero `n_cols` is invalid");
    cr_assert_eq(
        mat47_errno, MAT47_ERR_ZERO_SIZE,
        "%u (%s) was raised", mat47_errno, mat47_strerror(mat47_errno)
    );

    mat47_errno = 0;
    cr_assert_null(mat47_new(1, 0, false), "Zero `n_cols` and `n_rows` is invalid");
    cr_assert_eq(
        mat47_errno, MAT47_ERR_ZERO_SIZE,
        "%u (%s) was raised", mat47_errno, mat47_strerror(mat47_errno)
    );
}

Test(new, uninitialized)
{
    unsigned int r, c, i;
    mat47_t *m;

    for (r = 1; r <= 10; r++)
        for (c = 1; c <= 10; c++) {
            mat47_errno = 0;
            m = mat47_new(r, c, false);

            cr_assert_eq(
                mat47_errno, 0,
                "Error creating matrix (%s): r=%u, c=%u",
                mat47_strerror(mat47_errno), r, c
            );
            cr_assert_not_null(m, "`m` is null: r=%u, c=%u", r, c);

            cr_assert_eq(m->n_rows, r, "n_rows=%u, r=%u, c=%u", m->n_rows, r, c);
            cr_assert_eq(m->n_cols, c, "n_cols=%u, r=%u, c=%u", m->n_cols, r, c);

            cr_assert_not_null(m->data, "`m->data` is null: r=%u, c=%u", r, c);
            for (i = 0; i < m->n_rows; i++)
                cr_assert_not_null(
                    m->data[i], "`m->data[%u]` is null: r=%u, c=%u", i, r, c
                );

            mat47_del(m);
        }
}

Test(new, zeroed)
{
    unsigned int r, c, i, j;
    mat47_t *m;

    for (r = 1; r <= 10; r++)
        for (c = 1; c <= 10; c++) {
            mat47_errno = 0;
            m = mat47_new(r, c, true);

            cr_assert_eq(
                mat47_errno, 0,
                "Error creating matrix (%s): r=%u, c=%u",
                mat47_strerror(mat47_errno), r, c
            );
            cr_assert_not_null(m, "`m` is null: r=%u, c=%u", r, c);

            cr_assert_eq(m->n_rows, r, "n_rows=%u, r=%u, c=%u", m->n_rows, r, c);
            cr_assert_eq(m->n_cols, c, "n_cols=%u, r=%u, c=%u", m->n_cols, r, c);

            cr_assert_not_null(m->data, "`m->data` is null: r=%u, c=%u", r, c);
            for (i = 0; i < m->n_rows; i++) {
                cr_assert_not_null(
                    m->data[i], "`m->data[%u]` is null: r=%u, c=%u", i, r, c
                );
                for (j = 0; j < m->n_cols; j++)
                    cr_assert_eq(
                        m->data[i][j], 0,
                        "`m->data[%u][%u]` is not zero: r=%u, c=%u", i, j, r, c
                    );
            }

            mat47_del(m);
        }
}

/* init */

Test(init, null_array)
{
    mat47_errno = 0;
    cr_assert_null(mat47_init(2, 2, (double **)NULL), "Null `array` is invalid");
    cr_assert_eq(mat47_errno, MAT47_ERR_NULL_PTR);

    mat47_errno = 0;
    cr_assert_null(
        mat47_init(2, 2, ((int *[]){((int []){1, 1}), NULL})),
        "Null `array[i]` is invalid"
    );
    cr_assert_eq(mat47_errno, MAT47_ERR_NULL_PTR);
}

#define TestInit(T) \
Test(init, T) \
{ \
    unsigned int i, j; \
    mat47_t *m; \
    T a[3][2] = {{-128, -1}, {0, 1}, {2, 127}}; \
\
    mat47_errno = 0; \
    m = mat47_init(sizeof_arr(a), sizeof_arr(a[0]), ((T *[3]){a[0], a[1], a[2]})); \
\
    cr_assert_eq( \
        mat47_errno, 0, "Error creating matrix: (%s)", mat47_strerror(mat47_errno) \
    ); \
    for (i = 0; i < sizeof_arr(a); i++) \
        for (j = 0; j < sizeof_arr(a[0]); j++) \
            cr_assert_eq( \
                m->data[i][j], (double)a[i][j], \
                "`m = %f`, `a = %f`; i=%u, j=%u", \
                m->data[i][j], (double)a[i][j], i, j \
            ); \
\
    mat47_del(m); \
}

TestInit(int8_t)
TestInit(int16_t)
TestInit(int32_t)
TestInit(int64_t)

TestInit(uint8_t)
TestInit(uint16_t)
TestInit(uint32_t)
TestInit(uint64_t)

TestInit(float)
TestInit(double)

#undef TestInit

/* copy */

Test(copy, null_matrix_ptr)
{
    mat47_errno = 0;
    cr_assert_null(mat47_copy(NULL), "Null `m` is invalid");
    cr_assert_eq(
        mat47_errno, MAT47_ERR_NULL_PTR,
        "%u (%s) was raised", mat47_errno, mat47_strerror(mat47_errno)
    );
}

Test(copy, copy)
{
    unsigned int i, j;
    mat47_t *m1, *m2;
    int a[3][2] = {{-128, -1}, {0, 1}, {2, 127}};

    mat47_errno = 0;
    m1 = mat47_init(sizeof_arr(a), sizeof_arr(a[0]), ((int *[3]){a[0], a[1], a[2]}));

    cr_assert_eq(
        mat47_errno, 0, "Error creating matrix: (%s)", mat47_strerror(mat47_errno)
    );

    m2 = mat47_copy(m1);

    cr_assert_eq(
        mat47_errno, 0, "Error creating matrix: (%s)", mat47_strerror(mat47_errno)
    );
    cr_assert_not_null(m2, "`m2` is null");

    for (i = 0; i < sizeof_arr(a); i++)
        for (j = 0; j < sizeof_arr(a[0]); j++)
            cr_assert_eq(
                (double)m2->data[i][j], m1->data[i][j],
                "`m2 = %f`, `m1 = %f`: i=%u, j=%u",
                m2->data[i][j], m1->data[i][j], i, j
            );

    mat47_del(m1); mat47_del(m2);
}

/* elem */

Test(elem, get)
{
    unsigned int i, j;
    double a[3][2] = {{-128, -1}, {0, 1}, {2, 127}};
    unsigned char indexes[][2] = {
        {0, 1},
        {sizeof_arr(a) + 1, 1},
        {1, 0},
        {1, sizeof_arr(a[0]) + 1},
        {0, 0},
        {sizeof_arr(a) + 1, sizeof_arr(a[0]) + 1}
    };
    mat47_t *m;

    mat47_errno = 0;
    m = mat47_init(sizeof_arr(a), sizeof_arr(a[0]), ((double *[3]){a[0], a[1], a[2]}));

    cr_assert_eq(
        mat47_errno, 0, "Error creating matrix: (%s)", mat47_strerror(mat47_errno)
    );

    for (i = 0; i < sizeof_arr(indexes); i++) {
        mat47_errno = 0;
        mat47_get_elem(m, indexes[i][0], indexes[i][1]);

        cr_assert_eq(
            mat47_errno, MAT47_ERR_INDEX_OUT_OF_RANGE,
            "[%u,%u] should be out of range", indexes[i][0], indexes[i][1]
        );
    };

    mat47_errno = 0;
    for (i = 1; i <= sizeof_arr(a); i++) {
        for (j = 1; j <= sizeof_arr(a[0]); j++) {
            cr_assert_eq(
                mat47_get_elem(m, i, j), m->data[i-1][j-1],
                "`get_elem() = %f`, `m[][] = %f`; i=%u, j=%u; (error: %s)",
                mat47_get_elem(m, i, j), m->data[i-1][j-1], i, j,
                mat47_strerror(mat47_errno)
            );
        }
    }

    mat47_del(m);
}
