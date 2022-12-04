/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENSE for license information.
 */

#ifndef MAT47_MATRIX_H
#define MAT47_MATRIX_H

#include <stdint.h>
#include <stdio.h>

// Used only where necessary, to avoid long lines; Undefined later in this header
#define uint unsigned int

/** The default element format specifier */
#define MAT47_ELEM_FMT "%.4g"

/** Enables/Disables library-wide debug logs (disabled by default). */
extern _Bool MAT47_LOG_DEBUG;

/** Enables/Disables library-wide error logs (disabled by default). */
extern _Bool MAT47_LOG_ERROR;

/**
 * Stream to which library logs should be written.
 *
 * If null (default), logs are written to ``stderr``.
 *
 * Note:
 *     Only used when either or both debug and/or error logs are enabled via
 *     :c:var:`MAT47_LOG_DEBUG` and/or :c:var:`MAT47_LOG_ERROR`.
 *
 * Attention:
 *     Logging may or may not be thread-safe, depending on the file I/O implementation
 *     being used.
 */
extern FILE *MAT47_LOG_FILE;

/** The matrix type definition */
struct mat47 {

    /** Number of rows */
    unsigned int n_rows;

    /** Number of columns */
    unsigned int n_cols;

    double **data;
};

/** The matrix type (Alias of :c:struct:`struct mat47<mat47>`) */
typedef struct mat47 mat47_t;

/**
 * Copies a matrix.
 *
 * Args:
 *     m: The source matrix
 *
 * Returns:
 *     - A null pointer, if
 *
 *       - *m* is null, or
 *       - a failure occured during memory allocation.
 *
 *     - Otherwise, a pointer to a copy of *m*.
 *
 * ERRORS:
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_NULL_PTR`: *m* is null
 */
mat47_t *mat47_copy(const mat47_t *m);

/**
 * Deallocates memory used by a matrix.
 *
 * Args:
 *     m: The matrix to be deallocated
 *
 * Note:
 *     This must be called to deallocate memory internally used by a matrix.
 */
void mat47_del(mat47_t *m);

/**
 * Writes a string representation of a matrix to a stream.
 *
 * Args:
 *     m: The matrix whose representation should be written
 *     stream: A narrow-oriented stream to which the matrix representation should be
 *       written
 *     format: A valid format specifier for a ``double``, to be applied to every element
 *
 * Returns:
 *     - ``-1``, if
 *
 *       - any argument is null, or
 *       - a failure occurs during memory allocation.
 *
 *     - Otherwise, the amount of characters written to the stream.
 *
 * ERRORS:
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_NULL_PTR`: At least one of the
 *       arguments is null
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_ZERO_SIZE`: The element format string
 *       (*format*) is empty
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_ALLOC`: Unable to allocate memory
 *
 * Note:
 *     - The string representation of each element has a length limit of 24 characters.
 *       Beyond that, an element's string representation is simply truncated.
 *     - If *format* is invalid for a ``double`` or not null-terminated, the behaviour
 *       is undefined.
 *     - If the number of bytes written is greater than INTMAX_MAX, the return value is
 *       undefined.
 *     - *stream* is not explicitly flushed.
 */
intmax_t mat47_fprintf(
    const mat47_t *m, FILE *restrict stream, const char *restrict format
);

/** Like :c:func:`mat47_fprintf` but with *format* set to :c:macro:`MAT47_ELEM_FMT` */
#define mat47_fprint(m, stream) mat47_fprintf(m, stream, MAT47_ELEM_FMT)

/**
 * Retrieves a matrix element.
 *
 * Args:
 *     m: The matrix from which to retrieve an element
 *     row: The **1-based** index of the element's row
 *     col: The **1-based** index of the element's column
 *
 * Returns:
 *     - ``NAN``, if either index is out of range.
 *     - Otherwise, the element at ``m[row, col]``.
 *
 * ERRORS:
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_NULL_PTR`: *m* is null
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_INDEX_OUT_OF_RANGE`: Either index is
 *       out of range
 */
double mat47_get_elem(const mat47_t *m, unsigned int row, unsigned int col);

/**
 * Retrieves a sub-matrix.
 *
 * Args:
 *     m: The matrix from which to retrieve a sub-matrix
 *     top: The **1-based** index of the sub-matrix's top row
 *     left: The **1-based** index of the sub-matrix's leftmost column
 *     bottom: The **1-based** index of the sub-matrix's bottom row
 *     right: The **1-based** index of the sub-matrix's rightmost column
 *
 * Returns:
 *     - A null pointer, if any of the error conditions below occur.
 *     - Otherwise, a pointer to a new matrix equal to ``m[top:bottom , left:right]``.
 *
 * ERRORS:
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_NULL_PTR`: *m* is null
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_INDEX_OUT_OF_RANGE`: Any index is
 *       out of range
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_ZERO_SIZE`: Either dimension of the
 *       sub-matrix is less than or equal to zero
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_ALLOC`: Unable to allocate memory
 */
mat47_t *
mat47_get_submat
(const mat47_t *m, unsigned top, unsigned left, unsigned bottom, unsigned right);

/**
 * Creates a new matrix and initializes it from the given array.
 *
 * Args:
 *     n_rows (``unsigned int``): Number of rows in the array
 *     n_cols (``unsigned int``): Number of columns in the array
 *     array (``T **restrict``): 2D array from which the matrix should be initialized
 *       (See the description of ``T`` below)
 *
 * Returns:
 *     :c:type:`mat47_t *<mat47_t>`:
 *     - A null pointer, if
 *
 *       - either dimension equals zero, or
 *       - *array* (or any of the pointers it points to) is null, or
 *       - a failure occurs during memory allocation.
 *
 *     - Otherwise, a pointer to a new initialized matrix.
 *
 * ERRORS:
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_NULL_PTR`: *array*, or any of the
 *       pointers it points to, is null
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_ZERO_SIZE`: Either dimension equals zero
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_ALLOC`: Unable to allocate memory
 *
 * This is a generic interface which accepts an array of (or pointer to) pointers to
 * various arithmetic types. Hence, ``T`` can be any of the following types (or any
 * type compatible with any of these):
 *
 * - Signed integer: ``int8_t``, ``int16_t``, ``int32_t``, ``int64_t``
 * - Unsigned integer: ``uint8_t``, ``uint16_t``, ``uint32_t``, ``uint64_t``
 * - Floating-point: ``float``, ``double``
 *
 * Note:
 *     The behaviour is undefined if either dimension is greater than the respective
 *     dimension of the given array.
 */
#define mat47_init(n_rows, n_cols, array) \
    _Generic( \
        (array), \
        int8_t **: mat47_init_int8, int16_t **: mat47_init_int16, \
        int32_t **: mat47_init_int32, int64_t **: mat47_init_int64, \
        uint8_t **: mat47_init_uint8, uint16_t **: mat47_init_uint16, \
        uint32_t **: mat47_init_uint32, uint64_t **: mat47_init_uint64, \
        float **: mat47_init_float, double **: mat47_init_double \
    )(n_rows, n_cols, array)

// See ``mat47_init``
mat47_t *mat47_init_int8(uint n_rows, uint n_cols, int8_t **restrict array);
mat47_t *mat47_init_int16(uint n_rows, uint n_cols, int16_t **restrict array);
mat47_t *mat47_init_int32(uint n_rows, uint n_cols, int32_t **restrict array);
mat47_t *mat47_init_int64(uint n_rows, uint n_cols, int64_t **restrict array);
mat47_t *mat47_init_uint8(uint n_rows, uint n_cols, uint8_t **restrict array);
mat47_t *mat47_init_uint16(uint n_rows, uint n_cols, uint16_t **restrict array);
mat47_t *mat47_init_uint32(uint n_rows, uint n_cols, uint32_t **restrict array);
mat47_t *mat47_init_uint64(uint n_rows, uint n_cols, uint64_t **restrict array);
mat47_t *mat47_init_float(uint n_rows, uint n_cols, float **restrict array);
mat47_t *mat47_init_double(uint n_rows, uint n_cols, double **restrict array);

/** Like :c:func:`mat47_fprintf` but with *stream* set to ``stdout`` */
#define mat47_printf(m, format) mat47_fprintf(m, stdout, format)

/** Like :c:func:`mat47_printf` but with *format* set to :c:macro:`MAT47_ELEM_FMT` */
#define mat47_print(m) mat47_printf(m, MAT47_ELEM_FMT)

/**
 * Modifies a matrix element.
 *
 * Args:
 *     m: The matrix from which to retrieve an element
 *     row: The **1-based** index of the element's row
 *     col: The **1-based** index of the element's column
 *     value: The new value of the element
 *
 * ERRORS:
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_NULL_PTR`: *m* is null
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_INDEX_OUT_OF_RANGE`: Either index is
 *       out of range.
 */
void mat47_set_elem(mat47_t *m, unsigned int row, unsigned int col, double value);

/**
 * Creates a new zero matrix.
 *
 * Args:
 *     n_rows: Number of rows
 *     n_cols: Number of columns
 *
 * Returns:
 *     - A null pointer, if either dimension equals zero or a failure occurs during
 *       memory allocation.
 *     - Otherwise, a pointer to a new zero matrix.
 *
 * ERRORS:
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_ZERO_SIZE`: Either dimension equals zero
 *     - :c:enumerator:`~mat47_errors.MAT47_ERR_ALLOC`: Unable to allocate memory
 */
mat47_t *mat47_zero(unsigned int n_rows, unsigned int n_cols);

#undef uint

#endif  // MAT47_MATRIX_H
