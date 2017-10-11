/*
 * Copyright (c) 1996 by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef _SYS_INT_TYPES_H
#define	_SYS_INT_TYPES_H

#pragma ident	"@(#)int_types.h	1.4	96/09/25 SMI"

/*
 * This file, <sys/int_types.h>, is part of the Sun Microsystems implementation
 * of <inttypes.h> as proposed in the ISO/JTC1/SC22/WG14 C committee's working
 * draft for the revision of the current ISO C standard, ISO/IEC 9899:1990
 * Programming language - C.
 *
 * Programs/Modules should not directly include this file.  Access to the
 * types defined in this file should be through the inclusion of one of the
 * following files:
 *
 *	<sys/types.h>		Provides only the "_t" types defined in this
 *				file which is a subset of the contents of
 *				<inttypes.h>.  (This can be appropriate for
 *				all programs/modules except those claiming
 *				ANSI-C conformance.)
 *
 *	<sys/inttypes.h>	Provides the Kernel and Driver appropriate
 *				components of <inttypes.h>.
 *
 *	<inttypes.h>		For use by applications.
 *
 * See these files for more details.
 *
 * Use at your own risk.  As of February 1996, the committee is squarely
 * behind the fixed sized types; the "least" and "fast" types are still being
 * discussed.  The probability that the "fast" types may be removed before
 * the standard is finalized is high enough that they are not currently
 * implemented.  The unimplemented "fast" types are of the form
 * [u]int_fast[0-9]*_t and [u]intfast_t.
 */

#include <sys/isa_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Basic / Extended integer types
 *
 * The following defines the basic fixed-size integer types.
 *
 * Implementations are free to typedef them to Standard C integer types or
 * extensions that they support. If an implementation does not support one
 * of the particular integer data types below, then it should not define the
 * typedefs and macros corresponding to that data type.  Note that int8_t
 * is not defined in -Xs mode on ISAs for which the ABI specifies "char"
 * as an unsigned entity because there is not way to defined an eight bit
 * signed integral.
 */
#if defined(_CHAR_IS_SIGNED)
typedef char			int8_t;
#else
#if defined(__STDC__)
typedef signed char		int8_t;
#endif
#endif
typedef short			int16_t;
typedef int			int32_t;
#ifdef	_LP64
typedef long			int64_t;
#else /* /* _ILP32 */ */
#if __STDC__ - 0 == 0 && !defined(_NO_LONGLONG)
typedef	long long		int64_t;
#endif
#endif

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
#ifdef	_LP64
typedef unsigned long		uint64_t;
#else /* /* _ILP32 */ */
#if __STDC__ - 0 == 0 && !defined(_NO_LONGLONG)
typedef unsigned long long	uint64_t;
#endif
#endif

/*
 * intmax_t and uintmax_t are to be the longest (in number of bits) signed
 * and unsigned integer types supported by the implementation.
 */
#if defined(_LP64) || (__STDC__ - 0 == 0 && !defined(_NO_LONGLONG))
typedef int64_t			intmax_t;
typedef uint64_t		uintmax_t;
#else
typedef int32_t			intmax_t;
typedef uint32_t		uintmax_t;
#endif

/*
 * intptr_t and uintptr_t are signed and unsigned integer types large enough
 * to hold any data pointer; that is, data pointers can be assigned into or
 * from these integer types without losing precision.
 */
#ifdef	_LP64
typedef long			intptr_t;
typedef unsigned long		uintptr_t;
#else
typedef int			intptr_t;
typedef unsigned int		uintptr_t;
#endif /* /* _LP64 */ */

/*
 * The following define the smallest integer types that can hold the
 * specified number of bits.
 */
#if defined(_CHAR_IS_SIGNED)
typedef char			int_least8_t;
#else
#if defined(__STDC__)
typedef signed char		int_least8_t;
#endif
#endif
typedef short			int_least16_t;
typedef int			int_least32_t;
#ifdef	_LP64
typedef long			int_least64_t;
#else /* /* _ILP32 */ */
#if __STDC__ - 0 == 0 && !defined(_NO_LONGLONG)
typedef long long		int_least64_t;
#endif
#endif

typedef unsigned char		uint_least8_t;
typedef unsigned short		uint_least16_t;
typedef unsigned int		uint_least32_t;
#ifdef	_LP64
typedef unsigned long		uint_least64_t;
#else /* /* _ILP32 */ */
#if __STDC__ - 0 == 0 && !defined(_NO_LONGLONG)
typedef unsigned long long	uint_least64_t;
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* /* _SYS_INT_TYPES_H */ */
