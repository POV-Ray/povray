/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * Copyright (c) 1996-1999 by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef _SYS_TYPES_H
#define	_SYS_TYPES_H

#pragma ident	"@(#)types.h	1.65	99/11/15 SMI"

#include <sys/isa_defs.h>
#include <sys/feature_tests.h>

/*
 * Machine dependent definitions moved to <sys/machtypes.h>.
 */
#include <sys/machtypes.h>

/*
 * Include fixed width type declarations proposed by the ISO/JTC1/SC22/WG14 C
 * committee's working draft for the revision of the current ISO C standard,
 * ISO/IEC 9899:1990 Programming language - C.  These are not currently
 * required by any standard but constitute a useful, general purpose set
 * of type definitions which is namespace clean with respect to all standards.
 */
#ifdef SOLARIS_NP
#ifdef __STDC__
#undef __STDC__
#define __STDC__NP
#endif
#define __STDC__ 0
#include "int_types.h"
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 32
#endif
#else
#ifdef	_KERNEL
#include <sys/inttypes.h>
#else /* /* _KERNEL */ */
#include <sys/int_types.h>
#endif /* /* _KERNEL */ */
#endif

#if defined(_KERNEL) || defined(_SYSCALL32)
#include <sys/types32.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * The following protects users who use other than Sun compilers
 * (eg, GNU C) that don't support long long, and need to include
 * this header file.
 */
#if __STDC__ - 0 == 0 && !defined(_NO_LONGLONG)
typedef	long long		longlong_t;
typedef	unsigned long long	u_longlong_t;
#else
/* used to reserve space and generate alignment */
typedef union {
	double	_d;
	int32_t	_l[2];
} longlong_t;
typedef union {
	double		_d;
	uint32_t	_l[2];
} u_longlong_t;
#endif /* /* __STDC__ - 0 == 0 && !defined(_NO_LONGLONG) */ */

/*
 * These types (t_{u}scalar_t) exist because the XTI/TPI/DLPI standards had
 * to use them instead of int32_t and uint32_t because DEC had
 * shipped 64-bit wide.
 */
#if defined(_LP64) || defined(_I32LPx)
typedef int32_t		t_scalar_t;
typedef uint32_t	t_uscalar_t;
#else
typedef long		t_scalar_t;	/* historical versions */
typedef unsigned long	t_uscalar_t;
#endif /* /* defined(_LP64) || defined(_I32LPx) */ */

/*
 * POSIX Extensions
 */
typedef	unsigned char	uchar_t;
typedef	unsigned short	ushort_t;
typedef	unsigned int	uint_t;
typedef	unsigned long	ulong_t;

typedef	char		*caddr_t;	/* ?<core address> type */
typedef	long		daddr_t;	/* <disk address> type */
typedef	short		cnt_t;		/* ?<count> type */

#if defined(_ILP32)	/* only used in i386 code */
typedef	ulong_t		paddr_t;	/* <physical address> type */
#elif defined(__ia64)	/* XXX Fix me */
typedef	uint_t		paddr_t;
#endif

#ifndef	_PTRDIFF_T
#define	_PTRDIFF_T
#if defined(_LP64) || defined(_I32LPx)
typedef	long	ptrdiff_t;		/* pointer difference */
#else
typedef	int	ptrdiff_t;		/* (historical version) */
#endif
#endif

/*
 * VM-related types
 */
typedef	ulong_t		pfn_t;		/* page frame number */
typedef	ulong_t		pgcnt_t;	/* number of pages */
typedef	long		spgcnt_t;	/* signed number of pages */

typedef	uchar_t		use_t;		/* use count for swap.  */
typedef	short		sysid_t;
typedef	short		index_t;
typedef void		*timeout_id_t;	/* opaque handle from timeout(9F) */
typedef void		*bufcall_id_t;	/* opaque handle from bufcall(9F) */

/*
 * The size of off_t and related types depends on the setting of
 * _FILE_OFFSET_BITS.  (Note that other system headers define other types
 * related to those defined here.)
 *
 * If _LARGEFILE64_SOURCE is defined, variants of these types that are
 * explicitly 64 bits wide become available.
 */
#ifndef _OFF_T
#define	_OFF_T

#if defined(_LP64) || _FILE_OFFSET_BITS == 32
typedef long		off_t;		/* offsets within files */
#elif _FILE_OFFSET_BITS == 64
typedef longlong_t	off_t;		/* offsets within files */
#endif

#if defined(_LARGEFILE64_SOURCE)
#ifdef _LP64
typedef	off_t		off64_t;	/* offsets within files */
#else
typedef longlong_t	off64_t;	/* offsets within files */
#endif
#endif /* /* _LARGEFILE64_SOURCE */ */

#endif /* /* _OFF_T */ */

#if defined(_LP64) || _FILE_OFFSET_BITS == 32
typedef ulong_t		ino_t;		/* expanded inode type	*/
typedef long		blkcnt_t;	/* count of file blocks */
typedef ulong_t		fsblkcnt_t;	/* count of file system blocks */
typedef ulong_t		fsfilcnt_t;	/* count of files */
#elif _FILE_OFFSET_BITS == 64
typedef u_longlong_t	ino_t;		/* expanded inode type	*/
typedef longlong_t	blkcnt_t;	/* count of file blocks */
typedef u_longlong_t	fsblkcnt_t;	/* count of file system blocks */
typedef u_longlong_t	fsfilcnt_t;	/* count of files */
#endif

#if defined(_LARGEFILE64_SOURCE)
#ifdef _LP64
typedef	ino_t		ino64_t;	/* expanded inode type */
typedef	blkcnt_t	blkcnt64_t;	/* count of file blocks */
typedef	fsblkcnt_t	fsblkcnt64_t;	/* count of file system blocks */
typedef	fsfilcnt_t	fsfilcnt64_t;	/* count of files */
#else
typedef u_longlong_t	ino64_t;	/* expanded inode type	*/
typedef longlong_t	blkcnt64_t;	/* count of file blocks */
typedef u_longlong_t	fsblkcnt64_t;	/* count of file system blocks */
typedef u_longlong_t	fsfilcnt64_t;	/* count of files */
#endif
#endif /* /* _LARGEFILE64_SOURCE */ */

#ifdef _LP64
typedef	int		blksize_t;	/* used for block sizes */
#else
typedef	long		blksize_t;	/* used for block sizes */
#endif

#if defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)
typedef enum { _B_FALSE, _B_TRUE } boolean_t;
#else
typedef enum { B_FALSE, B_TRUE } boolean_t;
#endif /* /* defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE) */ */

/*
 * The [u]pad64_t is to be used in structures such that those structures
 * may be accessed by code produced by compilation environments which don't
 * support a 64 bit integral datatype.  This intention is not to allow
 * use of these fields in such environments, but to maintain the alignment
 * and offsets of the structure.
 */
#if __STDC__ - 0 == 0 && !defined(_NO_LONGLONG)
typedef int64_t		pad64_t;
typedef	uint64_t	upad64_t;
#else
typedef union {
	double   _d;
	int32_t  _l[2];
} pad64_t;
typedef union {
	double   _d;
	uint32_t _l[2];
} upad64_t;
#endif

typedef	longlong_t	offset_t;
typedef	u_longlong_t	u_offset_t;
typedef u_longlong_t	len_t;
typedef	longlong_t	diskaddr_t;

/*
 * Definitions remaining from previous partial support for 64-bit file
 * offsets.  This partial support for devices greater than 2gb requires
 * compiler support for long long.
 */
#ifdef _LONG_LONG_LTOH
typedef union {
	offset_t	_f;	/* Full 64 bit offset value */
	struct {
		int32_t	_l;	/* lower 32 bits of offset value */
		int32_t	_u;	/* upper 32 bits of offset value */
	} _p;
} lloff_t;
#endif

#ifdef _LONG_LONG_HTOL
typedef union {
	offset_t	_f;	/* Full 64 bit offset value */
	struct {
		int32_t	_u;	/* upper 32 bits of offset value */
		int32_t	_l;	/* lower 32 bits of offset value */
	} _p;
} lloff_t;
#endif

#ifdef _LONG_LONG_LTOH
typedef union {
	diskaddr_t	_f;	/* Full 64 bit disk address value */
	struct {
		int32_t	_l;	/* lower 32 bits of disk address value */
		int32_t	_u;	/* upper 32 bits of disk address value */
	} _p;
} lldaddr_t;
#endif

#ifdef _LONG_LONG_HTOL
typedef union {
	diskaddr_t	_f;	/* Full 64 bit disk address value */
	struct {
		int32_t	_u;	/* upper 32 bits of disk address value */
		int32_t	_l;	/* lower 32 bits of disk address value */
	} _p;
} lldaddr_t;
#endif

typedef uint_t k_fltset_t;	/* kernel fault set type */

/*
 * The following type is for various kinds of identifiers.  The
 * actual type must be the same for all since some system calls
 * (such as sigsend) take arguments that may be any of these
 * types.  The enumeration type idtype_t defined in sys/procset.h
 * is used to indicate what type of id is being specified.
 */
#if defined(_LP64) || defined(_I32LPx)
typedef int		id_t;		/* A process id,	*/
					/* process group id,	*/
					/* session id, 		*/
					/* scheduling class id,	*/
					/* user id, or group id */
#else
typedef	long		id_t;		/* (historical version) */
#endif

/*
 * Type useconds_t is an unsigned integral type capable of storing
 * values at least in the range of zero to 1,000,000.
 */
typedef uint_t		useconds_t;	/* Time, in microseconds */

#ifndef	_SUSECONDS_T
#define	_SUSECONDS_T
typedef long	suseconds_t;	/* signed # of microseconds */
#endif /* /* _SUSECONDS_T */ */

/*
 * Typedefs for dev_t components.
 */
#if defined(_LP64) || defined(_I32LPx)
typedef uint_t	major_t;	/* major part of device number */
typedef uint_t	minor_t;	/* minor part of device number */
#else
typedef ulong_t	major_t;	/* (historical version) */
typedef ulong_t	minor_t;	/* (historical version) */
#endif

/*
 * The data type of a thread priority.
 */
typedef short	pri_t;

/*
 * For compatibility reasons the following typedefs (prefixed o_)
 * can't grow regardless of the EFT definition. Although,
 * applications should not explicitly use these typedefs
 * they may be included via a system header definition.
 * WARNING: These typedefs may be removed in a future
 * release.
 *		ex. the definitions in s5inode.h remain small
 *			to preserve compatibility in the S5
 *			file system type.
 */
typedef	ushort_t o_mode_t;		/* old file attribute type */
typedef short	o_dev_t;		/* old device type	*/
typedef	ushort_t o_uid_t;		/* old UID type		*/
typedef	o_uid_t	o_gid_t;		/* old GID type		*/
typedef	short	o_nlink_t;		/* old file link type	*/
typedef short	o_pid_t;		/* old process id type	*/
typedef ushort_t o_ino_t;		/* old inode type	*/


/*
 * POSIX and XOPEN Declarations
 */
typedef	int	key_t;			/* IPC key type		*/
#if defined(_LP64) || defined(_I32LPx)
typedef	uint_t	mode_t;			/* file attribute type	*/
#else
typedef	ulong_t	mode_t;			/* (historical version) */
#endif

#ifndef	_UID_T
#define	_UID_T
#if defined(_LP64) || defined(_I32LPx)
typedef	int	uid_t;			/* UID type		*/
#else
typedef	long	uid_t;			/* (historical version) */
#endif
#endif /* /* _UID_T */ */

typedef	uid_t	gid_t;			/* GID type		*/

/*
 * POSIX definitions are same as defined in thread.h and synch.h.
 * Any changes made to here should be reflected in corresponding
 * files as described in comments.
 */
#ifndef SOLARIS_NP
typedef	unsigned int	pthread_t;	/* = thread_t in thread.h */
typedef	unsigned int	pthread_key_t;	/* = thread_key_t in thread.h */

typedef	struct	_pthread_mutex {		/* = mutex_t in synch.h */
	struct {
		uint16_t	__pthread_mutex_flag1;
		uint8_t		__pthread_mutex_flag2;
		uint8_t		__pthread_mutex_ceiling;
		uint32_t 	__pthread_mutex_type;
	} __pthread_mutex_flags;
	union {
		struct {
			uint8_t	__pthread_mutex_pad[8];
		} __pthread_mutex_lock64;
		upad64_t __pthread_mutex_owner64;
	} __pthread_mutex_lock;
	upad64_t __pthread_mutex_data;
} pthread_mutex_t;

typedef	struct	_pthread_cond {		/* = cond_t in synch.h */
	struct {
		uint8_t		__pthread_cond_flag[4];
		uint32_t 	__pthread_cond_type;
	} __pthread_cond_flags;
	upad64_t __pthread_cond_data;
} pthread_cond_t;

/*
 * UNIX 98 Extension
 */
typedef	struct _pthread_rwlock {	/* = rwlock_t in synch.h */
	int32_t		__pthread_rwlock_readers;
	uint16_t	__pthread_rwlock_type;
	uint16_t	__pthread_rwlock_magic;
	upad64_t	__pthread_rwlock_pad1[3];
	upad64_t	__pthread_rwlock_pad2[2];
	upad64_t	__pthread_rwlock_pad3[2];
} pthread_rwlock_t;

/*
 * attributes for threads, dynamically allocated by library
 */
typedef struct _pthread_attr {
	void	*__pthread_attrp;
} pthread_attr_t;


/*
 * attributes for mutex, dynamically allocated by library
 */
typedef struct _pthread_mutexattr {
	void	*__pthread_mutexattrp;
} pthread_mutexattr_t;


/*
 * attributes for cond, dynamically allocated by library
 */
typedef struct _pthread_condattr {
	void	*__pthread_condattrp;
} pthread_condattr_t;

/*
 * pthread_once
 */
typedef	struct	_once {
	upad64_t	__pthread_once_pad[4];
} pthread_once_t;

/*
 * UNIX 98 Extensions
 * attributes for rwlock, dynamically allocated by library
 */
typedef struct _pthread_rwlockattr {
	void	*__pthread_rwlockattrp;
} pthread_rwlockattr_t;
#endif

typedef ulong_t	dev_t;			/* expanded device type */

#if defined(_LP64) || defined(_I32LPx)
typedef	uint_t nlink_t;			/* file link type	*/
typedef int	pid_t;			/* process id type	*/
#else
typedef	ulong_t	nlink_t;		/* (historical version) */
typedef	long	pid_t;			/* (historical version) */
#endif

#ifndef _SIZE_T
#define	_SIZE_T
#if defined(_LP64) || defined(_I32LPx)
typedef	ulong_t	size_t;		/* size of something in bytes */
#else
typedef	uint_t	size_t;		/* (historical version) */
#endif
#endif /* /* _SIZE_T */ */

#ifndef _SSIZE_T
#define	_SSIZE_T
#if defined(_LP64) || defined(_I32LPx)
typedef long	ssize_t;	/* size of something in bytes or -1 */
#else
typedef int	ssize_t;	/* (historical version) */
#endif
#endif /* /* _SSIZE_T */ */

#ifndef _TIME_T
#define	_TIME_T
typedef	long		time_t;	/* time of day in seconds */
#endif /* /* _TIME_T */ */

#ifndef _CLOCK_T
#define	_CLOCK_T
typedef	long		clock_t; /* relative time in a specified resolution */
#endif /* /* ifndef _CLOCK_T */ */

#ifndef _CLOCKID_T
#define	_CLOCKID_T
typedef	int	clockid_t;	/* clock identifier type */
#endif /* /* ifndef _CLOCKID_T */ */

#ifndef _TIMER_T
#define	_TIMER_T
typedef	int	timer_t;	/* timer identifier type */
#endif /* /* ifndef _TIMER_T */ */

#if defined(__EXTENSIONS__) || \
	(!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE))

/* BEGIN CSTYLED */
typedef	unsigned char	unchar;
typedef	unsigned short	ushort;
typedef	unsigned int	uint;
typedef	unsigned long	ulong;
/* END CSTYLED */

#if defined(_KERNEL)

#define	SHRT_MIN	(-32768)	/* min value of a "short int" */
#define	SHRT_MAX	32767		/* max value of a "short int" */
#define	USHRT_MAX	65535		/* max of "unsigned short int" */
#define	INT_MIN		(-2147483647-1) /* min value of an "int" */
#define	INT_MAX		2147483647	/* max value of an "int" */
#define	UINT_MAX	4294967295U	/* max value of an "unsigned int" */
#if defined(_LP64)
#define	LONG_MIN	(-9223372036854775807L-1L)
					/* min value of a "long int" */
#define	LONG_MAX	9223372036854775807L
					/* max value of a "long int" */
#define	ULONG_MAX	18446744073709551615UL
					/* max of "unsigned long int" */
#else /* /* _ILP32 */ */
#define	LONG_MIN	(-2147483647L-1L)
					/* min value of a "long int" */
#define	LONG_MAX	2147483647L	/* max value of a "long int" */
#define	ULONG_MAX	4294967295UL	/* max of "unsigned long int" */
#endif

#endif /* /* defined(_KERNEL) */ */

#define	P_MYPID	((pid_t)0)

/*
 * The following is the value of type id_t to use to indicate the
 * caller's current id.  See procset.h for the type idtype_t
 * which defines which kind of id is being specified.
 */
#define	P_MYID	(-1)
#define	NOPID (pid_t)(-1)

#ifndef NODEV
#define	NODEV	(dev_t)(-1l)
#ifdef _SYSCALL32
#define	NODEV32	(dev32_t)(-1)
#endif /* /* _SYSCALL32 */ */
#endif /* /* NODEV */ */

/*
 * The following value of type pfn_t is used to indicate
 * invalid page frame number.
 */
#define	PFN_INVALID	((pfn_t)-1)

/* BEGIN CSTYLED */
typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;
typedef unsigned long	u_long;
typedef struct _quad { int val[2]; } quad_t;	/* used by UFS */
typedef quad_t		quad;			/* used by UFS */
/* END CSTYLED */

/*
 * Nested include for BSD/sockets source compatibility.
 * (The select macros used to be defined here).
 */
#include <sys/select.h>

#endif /* /* defined(__EXTENSIONS__) || (!defined(_POSIX_C_SOURCE) && ... */ */

/*
 * _VOID was defined to be either void or char but this is not
 * required because previous SunOS compilers have accepted the void
 * type. However, because many system header and source files use the
 * void keyword, the volatile keyword, and ANSI C function prototypes,
 * non-ANSI compilers cannot compile the system anyway. The _VOID macro
 * should therefore not be used and remains for source compatibility
 * only.
 */
/* CSTYLED */
#define	_VOID	void

#ifdef	__cplusplus
}
#endif

#endif /* /* _SYS_TYPES_H */ */
