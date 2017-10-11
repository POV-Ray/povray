# Copyright (C) 1992-2000 the Florida State University
#  Distributed by the Florida State University under the terms of the
#  GNU Library General Public License.
#
#This file is part of Pthreads.
#
#Pthreads is free software; you can redistribute it and/or
#modify it under the terms of the GNU Library General Public
#License as published by the Free Software Foundation (version 2).
#
#Pthreads is distributed "AS IS" in the hope that it will be
#useful, but WITHOUT ANY WARRANTY; without even the implied
#warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#See the GNU Library General Public License for more details.
#
#You should have received a copy of the GNU Library General Public
#License along with Pthreads; see the file COPYING.  If not, write
#to the Free Software Foundation, 675 Mass Ave, Cambridge,
#MA 02139, USA.
#
#Report problems and direct all questions to:
#
#  pthreads-bugs@ada.cs.fsu.edu
#
#  @(#)Makefile.SCO	3.14 11/8/00
#

#Compile flags:
#__FreeBSD__:		Compile for FreeBSD target
#__linux__:		Compile for Linux target
#_M_UNIX:		Compile for SCO-UNIX target
#ASM_SETJMP:		setjmp/longjmp reimplemented (with thread-specific
#			signal mask) in assembly. If this flag is not set,
#			the regular C library routines will be used. This
#			increases the portability but violates Pthreads
#			since the thread-specific signal mask may or may not
#			be saved. Thus, it should only be omitted when
#			absolutely necessary (for quick & dirty portability).
#C_CONTEXT_SWITCH:	for context switch written in C. Notice that the
#			assembly version for the SPARC is faster but by far
#			less portable (see portability notes in disp.c
#			and assembly file *.S).
#			Internal signal handling (pthread_kill) is also much
#			slower with the context switch in C, about a factor 2.
#			speed(C context switch) =~ 1.2*speed(as context switch)
#C_INTERFACE:		for C-language interface; if this option is omitted,
#			a language-independent interface will be generated
#			which has been tested for Verdix/SunAda. It seems that
#			this flag should only be omitted for Verdix/SunAda.
#			All other compilers follow C interface specs.
#CLEANUP_HEAP:		allocate cleanup handlers on heap (default: stack).
#DEBUG:			dump out trace information (mainly for signals).
#DEF_RR:		supports round-robin time-sliced scheduling.
#			Only works properly when MALLOC is also defined.
#IO:			I/O operations only block current thread, not process.
#			(SunOS 4.1.x kernel configuration must include
#			light-weight processes [LWP] and aynchronous I/O [AIO];
#			under Solaris 2.x, link with -laio -ldl.)
#MALLOC:		thread-safe malloc, in conjuction with Gnu malloc lib.
#			See README for more information.
#MUT_SWITCH:		forces context switch upon successful mutex_lock()
#			with regard to priorities (do not define NOERR_CHECK)
#NOERR_CHECK:		fast mutex operations without error checking.
#NO_INLINE:		no inlines for macros (internally CLEAR_KERNEL_FLAG).
#RAND_SWITCH:		forces context switch on kernel exit at random intervals
#			and to random threads without regard to priorities
#			(do not define NOERR_CHECK)
#REAL_TIME:		real-time scheduling support (implementation defined)
#RR_SWITCH:		forces context switch on kernel exit (round-robin queue)
#			without regard to priorities (do not define NOERR_CHECK)
#SIGNAL_STACK:		Allows user to execute handlers in case of a stack
#			overflow. STACK_CHECK has to be on for this.
#SIM_KERNEL:		simulate kernel implementation by using dummy UNIX
#			system call in selected routines.
#SOLARIS:		support for Solaris 2.x. Need to define SVR4 as well.
#			If you want to compile for Solaris, change this
#			Makefile by deleting/inserting
#			comments wherever the string "Solaris" occurs.
#SRP:			support of mutex ceiling under stack resource
#			policy (SRP)
#			if not defined, _POSIX_THREADS_PRIO_PROTECT has to be
#			undefined in unistd.h!
#STACK_CHECK:		lock page to cause bus error / illegal instruction
#			on stack overflow
#STAND_ALONE:		micro-kernel for VME SPARC Engine 1E; works only in
#			conjunction with SRP and no other compile options
#sun4e:			Sun SPARC 1E VME board (STAND_ALONE MythOS version)
#sun4m:			Force SPARC 3CE VME board (STAND_ALONE MythOS version)
#SVR4:			System V Release 4 UNIX system. For portability.
#			Currently supports Solaris only. If not defined,
#			BSD (Berkley) UNIX is assumed (currently SunOS 4.1.x).
#TIMER_DEBUG:		dump out trace information for SIGALRM signals and timer
#VME_ICACHE_OFF:	turn I-cache off (only w/ STAND_ALONE and sun4m)
#VME_DCACHE_OFF:	turn D-cache off (only w/ STAND_ALONE and sun4m)

#include directory location
INCS = ../include
PINC = $(INCS)/pthread
INC1 = -I$(INCS)
INC2 = 

#for cross-compilation
#INC2 = -I/home/midasusr/include
#INC2 = -I/sim/usr/include

#install directory locations
INSTALL_INCLUDE = /usr/local/include
INSTALL_LIBS = /usr/local/lib
#for Logware
#INSTALL_INCLUDE = /usr/contrib/include
#INSTALL_LIBS = /usr/contrib/lib

FBSD_ASM_INC =

#SPARC port flags
#for shipping
#CFLAGS = -DASM_SETJMP -DNOERR_CHECK -DSTACK_CHECK -DSIGNAL_STACK
#for maximum portability (minimum assembly required)
#CFLAGS = -DCLEANUP_HEAP -DC_CONTEXT_SWITCH
#for thread-safe malloc and round-robin scheduling option
#CFLAGS = -DASM_SETJMP -DNOERR_CHECK -DSTACK_CHECK -DSIGNAL_STACK \
#	-DMALLOC -DNO_FIX_MALLOC -DDEF_RR
#for GNARL project
#CFLAGS = -DASM_SETJMP -DSTACK_CHECK -DSIGNAL_STACK -DMALLOC -DNO_FIX_MALLOC
#for VME SPARC Engine 1E SunOS 4.0.3e (sun4e) / Force 3CE SunOS 4.1.3u (sun4m)
#CFLAGS = -DASM_SETJMP -DNOERR_CHECK -DSTAND_ALONE \
#	 -DSTACK_CHECK -DMALLOC -DNO_FIX_MALLOC \
#	 -Dsun4m 
#	 -DVME_ICACHE_OFF
#	 -DVME_DCACHE_OFF
#	 -DSTACK_CHECK -DSIGNAL_STACK
#	 -Dsun4e -DUARTB
#for testing
#CFLAGS = -DDEBUG -DTIMER_DEBUG -DASM_SETJMP -DSTACK_CHECK -DSIGNAL_STACK

#Linux / SCO port flags
CFLAGS =  -DC_CONTEXT_SWITCH -DCLEANUP_HEAP -D_POSIX -DAUTO_INIT

#MS-DOS under DJGPP2 flags
#CFLAGS = -DC_CONTEXT_SWITCH -DCLEANUP_HEAP -D_POSIX -D__dos__

#FreeBSD port flags
#CFLAGS =  -DC_CONTEXT_SWITCH -DCLEANUP_HEAP -D_POSIX -DSTACK_CHECK -DAUTO_INIT
#FBSD_ASM_INC = -I$(INCS)/../src/freebsd

#general flags
CFLAGS += -DSRP -DC_INTERFACE

#for non-process blocking (only thread-blocking) I/O:
CFLAGS += -DIO -DUSE_POLL -DMALLOC -DNO_FIX_MALLOC

#for shipping
CCFLAGS = -O2 $(CFLAGS)
CCFLAGS1 = -O1 $(CFLAGS)
CCFLAGS2 = -O2 $(CFLAGS)
CCFLAGS3 = -O0 $(CFLAGS)
#for testing
#CCFLAGS = -g $(CFLAGS)
#CCFLAGS1 = $(CCFLAGS)
#CCFLAGS2 = $(CCFLAGS)
#CCFLAGS3 = $(CCFLAGS)

#compile flags, always the same
CCFLAGS0 = $(CCFLAGS)

#for Gnu C compiler
CC = gcc
COMP_ASM = $(CC) $(INC1) $(INC2) $(FBSD_ASM_INC) $(CFLAGS) -c $*.S
#for Sun C
#COMP_ASM = $(AS) $(INC1) $(INC2) -P $(CFLAGS) $*.S -o $*.o
#for Sun C under Solaris 2.x
#delete comment designator "#" in the line below if you use /usr/lang/cc
#CC = /usr/lang/cc
#CC = /vol/rudolf-vol2/SUNWspro/bin/CC

#for Solaris 2.x
#delete comment designator "#" in the lines below for Solaris 2.x/SunOS 5.x
#CFLAGS += -DSOLARIS -DSVR4
#Only for gcc under SunOS 4/5.x, add "-traditional" in COMP_ASM.
#COMP_ASM += -traditional

COMP_C   = $(CC) $(INC1) $(INC2) $(CCFLAGS) -c $*.c
COMP_C0  = $(CC) $(INC1) $(INC2) $(CCFLAGS0) -c $*.c
COMP_C1  = $(CC) $(INC1) $(INC2) $(CCFLAGS1) -c $*.c
COMP_C2  = $(CC) $(INC1) $(INC2) $(CCFLAGS2) -c $*.c
COMP_C3  = $(CC) $(INC1) $(INC2) $(CCFLAGS3) -c $*.c

AR     = ar
RANLIB = ranlib

LINTFLAGS = -u $(CFLAGS)

LIB1 = ../lib/libgthreads.a
LIB2 = ../lib/libmalloc.a
LIBS = $(LIB1) $(LIB2)

CSRC = pthread.c stack.c mutex.c init.c signal.c signal_func.c \
	cond.c queue.c io.c disp.c

SCRS = $(CSRC) sched.S p_aux.S opt.S

OBJS = pthread.o stack.o mutex.o cond.o init.o \
	signal.o signal_func.o sched.o p_aux.o opt.o \
	queue.o io.o disp.o

HDR0 = $(PINC)/config.h config_internals.h


HDR1 = internals.h $(INCS)/pthread.h $(PINC)/unistd.h $(PINC)/limits.h \
	$(PINC)/errno.h $(PINC)/signal.h $(PINC)/asm.h \
	$(HDR0)

DEP1 = $(HDR1) Makefile

HDRS = $(HDR1) offsets.h mutex.h signal_internals.h

all: $(LIBS)

$(LIB2)::
	cd ../malloc; $(MAKE)

$(LIB1):: $(OBJS) ../malloc/gmalloc.o
	$(AR) ru $(LIB1) $(OBJS) ../malloc/gmalloc.o
	$(RANLIB) $(LIB1)

cond.o: cond.c $(DEP1) mutex.h
	$(COMP_C3)

mutex.o: mutex.c $(DEP1) mutex.h
	$(COMP_C)

pthread.o: pthread.c $(DEP1)
	$(COMP_C)

init.o: init.c $(DEP1) setjmp.h
	$(COMP_C)

signal.o: signal.c $(DEP1) mutex.h signal_internals.h offsets.h setjmp.h
	$(COMP_C)

signal_func.o: signal_func.c $(DEP1) mutex.h internals.h offsets.h setjmp.h
	$(COMP_C3)

stack.o: stack.c $(DEP1)
	$(COMP_C)

queue.o: queue.c $(DEP1)
	$(COMP_C)

io.o: io.c $(DEP1)
	$(COMP_C0)

disp.o: disp.c $(DEP1) setjmp.h
	$(COMP_C1)

sched.o: sched.S $(PINC)/asm.h \
		offsets.h signal_internals.h $(HDR0) Makefile
	$(COMP_ASM)
#for SunOS 4.1.x and Solaris < 2.4
#	$(COMP_ASM) -DEMPTY

p_aux.o: p_aux.S $(PINC)/asm.h \
		offsets.h $(HDR0) Makefile
	$(COMP_ASM)

opt.o: opt.S $(PINC)/asm.h \
		offsets.h $(HDR0) Makefile
	$(COMP_ASM)

offsets.h: get_offsets
	./get_offsets > offsets.h

get_offsets: get_offsets.c $(DEP1) setjmp.h
	$(CC) $(INC1) $(INC2) $(CCFLAGS) -o $@ $@.c

$(PINC)/config.h: config_header Makefile const.sh
	./config_header -D`uname -r | sed "s/\.//g"` $(CCFLAGS) > $@
	sh const.sh >> $@

config_internals.h: config_header2 Makefile
	./config_header2 -l -D`uname -r | sed "s/\.//"` $(CCFLAGS) > $@

config_header: config_header.c Makefile
	$(CC) $(INC1) $(INC2) $(CCFLAGS) $(CCIFLAGS) -o $@ $@.c

config_header2: config_header.c Makefile
	touch config_internals.h
	$(CC) $(INC1) $(INC2) $(CCFLAGS) -DINTERNALS -o $@ $<
	rm -f config_internals.h

ifneq ($(MAKECMDGOALS),clean)
-include .dep
endif

.dep: $(HDR0) offsets.h Makefile
	$(CC) -M $(INC1) $(INC2) $(CFLAGS) *.c *.S > .dep


install:
	cp $(INCS)/*.h $(INSTALL_INCLUDE)
	if ( test ! -d $(INSTALL_INCLUDE)/pthread ) ; then \
	  mkdir $(INSTALL_INCLUDE)/pthread ; \
	fi ;
	cp $(INCS)/pthread/*.h $(INSTALL_INCLUDE)/pthread
	cp $(LIBS) $(INSTALL_LIBS)

lint:
	lint $(LINTFLAGS) $(CSRC)

llib:
	lint $(LINTFLAGS) -Cthreads $(SRCS)

clean:
	-rm -f $(LIBS) a.out core errs *.o *.EXE *.BAK *.CKP config_internals.h $(PINC)/config.h config_header config_header2 offsets.h get_offsets *~ #* .dep
	cd ../malloc; $(MAKE) clean
