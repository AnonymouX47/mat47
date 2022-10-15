#include <criterion/criterion.h>

#include "../src/mat47/matrix.c"

Test(new, zero_size)
{
    mat47_errno = 0;
    cr_assert_null(mat47_new(0, 1, false), "Zero `n_rows` is invalid");
    cr_assert_eq(mat47_errno, MAT47_ERR_ZERO_SIZE);

    mat47_errno = 0;
    cr_assert_null(mat47_new(1, 0, false), "Zero `n_cols` is invalid");
    cr_assert_eq(mat47_errno, MAT47_ERR_ZERO_SIZE);

    mat47_errno = 0;
    cr_assert_null(mat47_new(1, 0, false), "Zero `n_cols` and `n_rows` is invalid");
    cr_assert_eq(mat47_errno, MAT47_ERR_ZERO_SIZE);
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
    m = mat47_init(3, 2, ((T *[3]){a[0], a[1], a[2]})); \
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
