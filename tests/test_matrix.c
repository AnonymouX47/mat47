#include <limits.h>

#include <criterion/criterion.h>

#include "../src/mat47/matrix.c"
#include "../src/mat47/utils.c"


#define create_matrix(m, mat47_f, ...) \
    mat47_errno = 0; \
    m = mat47_f(__VA_ARGS__); \
\
    cr_assert_eq( \
        mat47_errno, 0, "Error creating matrix: (%s)", mat47_strerror(mat47_errno) \
    ); \
    cr_assert_not_null(m, "`" #m "` is null")

#define assert_null_ret_yes(arg, mat47_f, ...) \
    cr_assert_null(mat47_f(__VA_ARGS__), "`" #arg "` is invalid")

#define assert_null_ret_no(_, mat47_f, ...) \
    mat47_f(__VA_ARGS__)

#define assert_null_ptr(ptr, chk_ret, mat47_f, ...) \
    mat47_errno = 0; \
    assert_null_ret_##chk_ret(ptr = NULL, mat47_f, ##__VA_ARGS__); \
\
    cr_assert_eq( \
        mat47_errno, MAT47_ERR_NULL_PTR, \
        #ptr " = NULL: %u (%s) was raised", mat47_errno, mat47_strerror(mat47_errno) \
    )

#define assert_null_martix_ptr(chk_ret, mat47_f, ...) \
    assert_null_ptr(m, chk_ret, mat47_f, NULL, ##__VA_ARGS__)


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
    T a[3][2] = {{-128, -1}, {0, 1}, {2, 127}}; \
    mat47_t *m; \
\
    create_matrix( \
        m, mat47_init, sizeof_arr(a), sizeof_arr(a[0]), ((T *[3]){a[0], a[1], a[2]}) \
    ); \
\
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
    assert_null_martix_ptr(yes, mat47_copy);
}

Test(copy, copy)
{
    unsigned int i, j;
    int a[3][2] = {{-128, -1}, {0, 1}, {2, 127}};
    mat47_t *m1, *m2;

    create_matrix(
        m1, mat47_init, sizeof_arr(a), sizeof_arr(a[0]), ((int *[3]){a[0], a[1], a[2]})
    );
    create_matrix(m2, mat47_copy, m1);

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

Test(elem, get_null_matrix_ptr)
{
    assert_null_martix_ptr(no, mat47_get_elem, 1, 1);
}

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

    create_matrix(
        m, mat47_init,
        sizeof_arr(a), sizeof_arr(a[0]), ((double *[3]){a[0], a[1], a[2]})
    );

    for (i = 0; i < sizeof_arr(indexes); i++) {
        mat47_errno = 0;
        mat47_get_elem(m, indexes[i][0], indexes[i][1]);

        cr_assert_eq(
            mat47_errno, MAT47_ERR_INDEX_OUT_OF_RANGE,
            "[%u,%u] should be out of range", indexes[i][0], indexes[i][1]
        );
    };

    for (i = 1; i <= sizeof_arr(a); i++) {
        for (j = 1; j <= sizeof_arr(a[0]); j++) {
            mat47_errno = 0;

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

Test(elem, set_null_matrix_ptr)
{
    assert_null_martix_ptr(no, mat47_set_elem, 1, 1, 0.0);
}

Test(elem, set)
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

    create_matrix(m, mat47_zero, sizeof_arr(a), sizeof_arr(a[0]));

    for (i = 0; i < sizeof_arr(indexes); i++) {
        mat47_errno = 0;
        mat47_set_elem(m, indexes[i][0], indexes[i][1], 0.0);

        cr_assert_eq(
            mat47_errno, MAT47_ERR_INDEX_OUT_OF_RANGE,
            "[%u,%u] should be out of range", indexes[i][0], indexes[i][1]
        );
    };

    for (i = 1; i <= sizeof_arr(a); i++) {
        for (j = 1; j <= sizeof_arr(a[0]); j++) {
            mat47_errno = 0;
            mat47_set_elem(m, i, j, a[i-1][j-1]);

            cr_assert_eq(
                m->data[i-1][j-1], a[i-1][j-1],
                "`m[][] = %f`, `a[][] = %f`; i=%u, j=%u; (error: %s)",
                m->data[i-1][j-1], a[i-1][j-1], i, j,
                mat47_strerror(mat47_errno)
            );
        }
    }

    mat47_del(m);
}

/* submat */

#define assert_get_submat_null_ret assert_null_ret_yes
#define assert_set_submat_null_ret assert_null_ret_no

#define Test_submat_index_out_of_range(get_set, ...) \
Test(submat, get_set##_index_out_of_range) \
{ \
    unsigned int i, j, indexes[] = {0, 5, UINT_MAX}; \
    char *index_names[4] = {"top", "left", "bottom", "right"}; \
    mat47_t *m; \
\
    create_matrix(m, mat47_zero, 4, 4); \
\
    for (j = 0; j < sizeof_arr(indexes); j++) { \
        for (i = 0; i < 4; i++) { \
            unsigned int args[4] = {1, 1, 1, 1}; \
\
            args[i] = indexes[j]; \
            mat47_errno = 0; \
            assert_##get_set##_submat_null_ret( \
                out-of-range index, \
                mat47_##get_set##_submat, \
                m, args[0], args[1], args[2], args[3], ##__VA_ARGS__ \
            ); \
\
            cr_assert_eq( \
                mat47_errno, MAT47_ERR_INDEX_OUT_OF_RANGE, \
                "%s = %u; error: %u (%s)", \
                index_names[i], indexes[j], mat47_errno, mat47_strerror(mat47_errno) \
            ); \
        } \
    } \
\
    mat47_del(m); \
}

#define Test_submat_zero_size(get_set, ...) \
Test(submat, get_set##_zero_size) \
{ \
    unsigned int i, indexes[][4] = { \
        /* top, left, bottom, right */ \
        {2, 1, 1, 1}, \
        {1, 2, 1, 1}, \
        {3, 1, 1, 1}, \
        {1, 3, 1, 1}, \
        {4, 1, 2, 1}, \
        {1, 4, 1, 3}, \
    }; \
    mat47_t *m; \
\
    create_matrix(m, mat47_zero, 4, 4); \
\
    for (i = 0; i < sizeof_arr(indexes); i++) { \
        mat47_errno = 0; \
        assert_##get_set##_submat_null_ret( \
            zero size, \
            mat47_##get_set##_submat, \
            m, indexes[i][0], indexes[i][1], indexes[i][2], indexes[i][3], \
            ##__VA_ARGS__ \
        ); \
\
        cr_assert_eq( \
            mat47_errno, MAT47_ERR_ZERO_SIZE, \
            "[%u:%u , %u:%u]; %u (%s) was raised", \
            /* top:bottom, left:right */ \
            indexes[i][0], indexes[i][2], indexes[i][1], indexes[i][3], \
            mat47_errno, mat47_strerror(mat47_errno) \
        ); \
    }; \
\
    mat47_del(m); \
}


Test(submat, get_null_matrix_ptr)
{
    assert_null_martix_ptr(yes, mat47_get_submat, 1, 1, 1, 1);
}

Test_submat_index_out_of_range(get)

Test_submat_zero_size(get)

Test(submat, get)
{
    unsigned int n, i, j, u, v, top, left, bottom, right, n_rows, n_cols;
    double a[4][4] = {
        {-128, -64, -32, -1},
        {0, 1, 2, 3},
        {4, 5, 6, 7},
        {16, 32, 64, 127}
    };
    unsigned int indexes[][3][2] = {
        // {top, left}, {bottom, right}, {n_rows, n_cols}
        {{1, 1}, {1, 1}, {1, 1}},
        {{4, 4}, {4, 4}, {1, 1}},
        {{1, 1}, {4, 4}, {4, 4}},
        {{1, 1}, {3, 2}, {3, 2}},
        {{3, 2}, {4, 4}, {2, 3}},
        {{2, 2}, {3, 3}, {2, 2}},
        {{2, 2}, {2, 2}, {1, 1}}
    };
    mat47_t *m, *sub;

    create_matrix(m, mat47_init, 4, 4, ((double *[4]){a[0], a[1], a[2], a[3]}));

    for (n = 0; n < sizeof_arr(indexes); n++) {
        top = indexes[n][0][0]; left = indexes[n][0][1];
        bottom = indexes[n][1][0]; right = indexes[n][1][1];
        n_rows = indexes[n][2][0]; n_cols = indexes[n][2][1];

        mat47_errno = 0;
        sub = mat47_get_submat(m, top, left, bottom, right);

        cr_assert_eq(
            mat47_errno, 0,
            "[%u:%u , %u:%u]: %s",
            top, bottom, left, right, mat47_strerror(mat47_errno)
        );

        cr_assert_eq(
            sub->n_rows, n_rows,
            "[%u:%u , %u:%u]: n_rows=%u", top, bottom, left, right, sub->n_rows
        );

        cr_assert_eq(
            sub->n_cols, n_cols,
            "[%u:%u , %u:%u]: n_cols=%u", top, bottom, left, right, sub->n_cols
        );

        // Zero-based indexing
        --top; --left;
        for (u = (i = 0) + top; i < n_rows; i++, u++) {
            for (v = (j = 0) + left; j < n_cols; j++, v++) {
                cr_assert_eq(
                    sub->data[i][j], m->data[u][v],
                    "[%u:%u , %u:%u]: sub[%u,%u] = %f, m[%u,%u] = %f",
                    top + 1, bottom, left + 1, right,
                    i + 1, j + 1, sub->data[i][j], u + 1, v + 1, m->data[u][v]
                );
            }
        }
        mat47_del(sub);
    }
    mat47_del(m);
}

Test(submat, set_null_matrix_ptr)
{
    mat47_t *m_sub;

    create_matrix(m_sub, mat47_zero, 1, 1);
    // Test `m`
    assert_null_martix_ptr(no, mat47_set_submat, 1, 1, 1, 1, m_sub);
    // Test `sub`
    assert_null_ptr(sub, no, mat47_set_submat, m_sub, 1, 1, 1, 1, NULL);
    mat47_del(m_sub);
}

Test_submat_index_out_of_range(set, m)

Test_submat_zero_size(set, m)

Test(submat, set)
{
    unsigned int n, i, j, u, v, top, left, bottom, right, n_rows, n_cols;
    double a[4][4] = {
        {-128, -64, -32, -1},
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {16, 32, 64, 127}
    };
    unsigned int indexes[][3][2] = {
        // {top, left}, {bottom, right}, {n_rows, n_cols}
        {{1, 1}, {1, 1}, {1, 1}},
        {{4, 4}, {4, 4}, {1, 1}},
        {{1, 1}, {4, 4}, {4, 4}},
        {{1, 1}, {3, 2}, {3, 2}},
        {{3, 2}, {4, 4}, {2, 3}},
        {{2, 2}, {3, 3}, {2, 2}},
        {{2, 2}, {2, 2}, {1, 1}}
    };
    mat47_t *m, *sub;

    for (n = 0; n < sizeof_arr(indexes); n++) {
        top = indexes[n][0][0]; left = indexes[n][0][1];
        bottom = indexes[n][1][0]; right = indexes[n][1][1];
        n_rows = indexes[n][2][0]; n_cols = indexes[n][2][1];

        create_matrix(m, mat47_init, 4, 4, ((double *[4]){a[0], a[1], a[2], a[3]}));
        create_matrix(sub, mat47_zero, n_rows, n_cols);

        mat47_errno = 0;
        mat47_set_submat(m, top, left, bottom, right, sub);

        cr_assert_eq(
            mat47_errno, 0,
            "[%u:%u , %u:%u]: %s",
            top, bottom, left, right, mat47_strerror(mat47_errno)
        );

        // Zero-based indexing
        --top; --left;
        for (u = (i = 0) + top; i < n_rows; i++, u++) {
            for (v = (j = 0) + left; j < n_cols; j++, v++) {
                cr_assert_eq(
                    sub->data[i][j], m->data[u][v],
                    "[%u:%u , %u:%u]: sub[%u,%u] = %f, m[%u,%u] = %f",
                    top + 1, bottom, left + 1, right,
                    i + 1, j + 1, sub->data[i][j], u + 1, v + 1, m->data[u][v]
                );
            }
        }
        mat47_del(sub);
        mat47_del(m);
    }
}
