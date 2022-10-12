/* Main matrix definitions
 *
 * Copyright (c) 2022 AnonymouX47
 * See https://github.com/AnonymouX47/mat47/LICENCE for license information.
 */

/**
 * Thread-local variable used to signify errors that occur within library functions.
 *
 * The initial value is ``0`` and no function ever sets the value to ``0``.
 *
 * | When an error occurs within any function defined by
 *   this library, this variable is set to a positive value (one of the enumerators
 *   defined in :c:enum:`mat47_errors`) before the function returns.
 * | Across the documentation of this library, this is referred to as **"raising"**
 *   an error.
 *
 * Note:
 *     If the value will be examined after calling a function, it should be set to
 *     ``0`` before the call.
 */
extern _Thread_local int mat47_errno;

/**
 * Defines the errors *raised* by functions within this library.
 *
 * Note:
 *     Error codes (set in :c:data:`mat47_errno`) should only be compared with the
 *     enumerators defined in this enumeration and not directly with the integer
 *     values of the enumerators, as those might change in the future.
 */
enum mat47_errors {
    /** Raised when unable to allocate memory */
    MAT47_ERR_ALLOC = 1,

    /** Raised when arguments would give in an invalid empty/zero-sized result */
    MAT47_ERR_ZERO_SIZE,

    /** Raised when an argument is, or points to, a null pointer */
    MAT47_ERR_NULL_PTR,

    /** Raised for out-of-range index arguments */
    MAT47_ERR_INDEX_OUT_OF_RANGE
};
