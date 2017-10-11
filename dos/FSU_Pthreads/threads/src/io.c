/* Copyright (C) 1992-2000 the Florida State University
   Distributed by the Florida State University under the terms of the
   GNU Library General Public License.

This file is part of Pthreads.

Pthreads is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation (version 2).

Pthreads is distributed "AS IS" in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with Pthreads; see the file COPYING.  If not, write
to the Free Software Foundation, 675 Mass Ave, Cambridge,
MA 02139, USA.

Report problems and direct all questions to:

  pthreads-bugs@ada.cs.fsu.edu

  @(#)io.c	3.14 11/8/00
  
*/

#ifdef IO

#include <pthread/config.h>

#if defined(__FreeBSD__) || defined(_M_UNIX) || defined(__linux__) || defined(__dos__)

#include "signal_internals.h"
#include "internals.h"
#include "fd.h"
#include <fcntl.h>
#include <sys/uio.h>
#ifdef _M_UNIX
#include <sys/socket.h>
#endif /* _M_UNIX */
#include <sys/stat.h>
#ifdef SCO5
#include <sys/time.h>
#include <sys/resource.h>
#include <stropts.h>
#if defined(IO)
#include <stdarg.h>
#endif /* IO */
#endif /* SCO5 */
#ifdef USE_POLL
#include <poll.h>
#endif /* USE_POLL */

#ifndef EWOULDBLOCK
#define EWOULDBLOCK EAGAIN
#endif

#ifndef ssize_t
#define ssize_t int
#endif

#else /* !(__FreeBSD__ || _M_UNIX || __linux__ || __dos__) */


#include "internals.h"
#include <sys/types.h>
#ifdef _M_UNIX
#include <sys/socket.h>
#endif /* _M_UNIX */
#include <sys/uio.h>
#include <fcntl.h>

#endif /* !(__FreeBSD__ || _M_UNIX || __linux__ || __dos__) */

#ifndef  __FDS_BITS
#define __FDS_BITS(set) ((set)->fds_bits)
#endif

#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
/*
 * aioread/write may cause a stack overflow in the UNIX kernel which cannot
 * be caught by sighandler. This seems to be a bug in SunOS. We get
 * around this problem by deliberately trying to access a storage
 * location on stack about a page ahead of where we are. This will
 * cause a premature stack overflow (SIGBUS) which *can* be caught
 * by sighandler.
 */
  extern int pthread_page_size;
  volatile static int pthread_access_dummy;
#define ACCESS_STACK \
  MACRO_BEGIN \
    if (*(int *) (pthread_get_sp() - pthread_page_size)) \
      pthread_access_dummy = 0; \
  MACRO_END

#else /* !STACK_CHECK || !SIGNAL_STACK */
#define ACCESS_STACK
#endif /* !STACK_CHECK || !SIGNAL_STACK */

#if !defined(__FreeBSD__) && !defined(SVR4)

#ifndef _M_UNIX

#ifndef FASYNC
#define FASYNC 0
#endif

/*------------------------------------------------------------*/
/*
 * pthread_fds_set_async - set file descriptors to async IO
 */
void pthread_fds_set_async(l, width)
     fd_set *l;
     int width;
{
  int i, mode;

  if (l)
    for (i = 0; i < width; i++)
      if (FD_ISSET(i, l)) {
	mode = fcntl(i, F_GETFL, 0);
	if (!(mode & (O_NDELAY | FASYNC)))
	  FD_SET(i, l);
	fcntl(i, F_SETFL, mode | O_NDELAY | FASYNC);
#ifdef F_SETOWN      
	fcntl(i, F_SETOWN, getpid());
#endif
      }
}

/*------------------------------------------------------------*/
/*
 * pthread_fds_clr_async - clear async flags on previously set file descriptors
 */
void pthread_fds_clr_async(l, width)
     fd_set *l;
     int width;
{
  int i, mode;

  if (l)
    for (i = 0; i < width; i++)
      if (FD_ISSET(i, l)) {
	mode = fcntl(i, F_GETFL, 0);
	fcntl(i, F_SETFL, mode & ~(O_NDELAY | FASYNC));
      }
}
#endif /* !_M_UNIX */

/*------------------------------------------------------------*/
/*
 * pthread_fds_zero - initialization of file descriptor
 */
void pthread_fds_zero(fds, width)
fd_set* fds;
int width;

{
  register int i;

  if (fds !=0)
   for (i = 0; i < howmany(width, NFDBITS); i++)
     __FDS_BITS(fds)[i] = 0;
}

/*------------------------------------------------------------*/
/*
 * pthread_fds_set - initialization of file descriptor
 */
void pthread_fds_set(l, r, width)
     fd_set *l, *r;
{
  if (l)
    if (r)
     {
       register int i;

       for (i = 0; i < howmany(width, NFDBITS); i++)
         __FDS_BITS(l)[i] = __FDS_BITS(r)[i];
     }
    else 
      pthread_fds_zero(l, width);
}

/*------------------------------------------------------------*/
/*
 * pthread_fds_union - bit-wise union of file descriptor fields
 */
void pthread_fds_union(l, r, width)
     fd_set *l,*r;
     int width;
{
  register int i;

  for (i = 0; i < howmany(width, NFDBITS); i++)
    __FDS_BITS(l)[i] = __FDS_BITS(l)[i] | __FDS_BITS(r)[i];
}
#endif /* !defined(__FreeBSD__) && !defined(SVR4) */

#if defined(__FreeBSD__) || defined(_M_UNIX) || defined(__linux__)

#ifdef USE_POLL

/*------------------------------------------------------------*/
/*
 * pollfd_to_fd_set - Convert pollfd vector to fd_set
 */
int pollfd_to_fd_set(nfds, fds, readfds, writefds, exceptfds, width)
  int nfds;
  struct pollfd fds[];
  fd_set *readfds;
  fd_set *writefds;
  fd_set *exceptfds;
  int width;
{
   register int i, how_many = 0;
   pthread_fds_zero(readfds, width);
   pthread_fds_zero(writefds, width);
   pthread_fds_zero(exceptfds, width);
   for (i=0; i<nfds; i++)
    {
      if (fds[i].revents==0)
         continue;
      if (fds[i].revents&POLLNVAL)
       {
         errno = EBADF;
         return -1;
       }
      if (readfds && (fds[i].revents&(POLLIN|POLLERR|POLLHUP))!=0)
       {
          FD_SET(fds[i].fd, readfds);
          how_many++;
       }
      if (writefds && (fds[i].revents&(POLLOUT|POLLERR|POLLHUP))!=0)
       {
          FD_SET(fds[i].fd, writefds);
          how_many++;
       }
      if (exceptfds && (fds[i].revents&(POLLPRI|POLLERR|POLLHUP))!=0)
       {
          FD_SET(fds[i].fd, exceptfds);
          how_many++;
       }
    }

   return how_many;
}

/*------------------------------------------------------------*/
/*
 * fd_set_to_pollfd - Convert fd_set to pollfd vector
 */
int fd_set_to_pollfd(width, readfds, writefds, exceptfds, fds)
   int width;
   fd_set *readfds;
   fd_set *writefds;
   fd_set *exceptfds;
   struct pollfd fds[];
{
   register int i, nfds = 0;
   for (i=0; i<width; i++)
    {
      short events = 0;
      if (readfds && FD_ISSET(i, readfds))
         events |= POLLIN;
      if (writefds && FD_ISSET(i, writefds))
       {
          events |= POLLOUT;
       }
      if (exceptfds && FD_ISSET(i, exceptfds))
       {
          events |= POLLPRI;
       }
      if (events!=0)
       {
         fds[nfds].fd = i;
         fds[nfds].events = events;
         fds[nfds].revents = 0;
         nfds++;
       }
    }

   return nfds;
}

#endif

struct fdentry
{
  int count;
  int flags;
  int blocking;
};

static int fdtablesize;
static struct fdentry* fdtable;

static void fdentryinit(struct fdentry* entry)

{
   entry->count = 0;
   entry->flags = 0;
   entry->blocking = 0;
}

static void fdtableinit()

{
   register int i;
   fdtablesize = pthread_getfdtablesize();
   fdtable = (struct fdentry*) malloc(fdtablesize*sizeof(struct fdentry));
   for (i=0;i<fdtablesize;i++)
     fdentryinit(&fdtable[i]);
}

/*------------------------------------------------------------*/
/*
 * getfdentry - Get fdentry for fd
 * assume SET_KERNEL_FLAG
 */
static struct fdentry* getfdentry(int fd)

{
   if (fd>=fdtablesize)
    {
       struct stat stat;
       /* Verify this is really a good fd.
        */
       if (fstat(fd, &stat)==-1)
        {
          return NULL;
        }
       /* Another thread can resized the fdtablesize
        */
       if (fd>=fdtablesize)
        {
          register int i;
          struct fdentry* newfdtable;
#ifdef MALLOC
          newfdtable=(struct fdentry*) pthread_realloc(fdtable,
                                            (fd+1)*sizeof(struct fdentry));
#else
          newfdtable=(struct fdentry*) realloc(fdtable,
                                            (fd+1)*sizeof(struct fdentry));
#endif
          if (newfdtable==NULL)
           {
             return NULL;
           }
          for (i=fdtablesize; i<=fd; i++)
            fdentryinit(&newfdtable[i]);
          fdtablesize=fd+1;
          fdtable=newfdtable;
        }
    }
   return &fdtable[fd];
}

/*------------------------------------------------------------*/
/*
 * acquire_fdlock() Save the current flags of fd.
 * SET_KERNEL_FLAG if lock acquire
 */
static struct fdentry* acquire_fdlock(int fd)

{
   register struct fdentry* entry;

   if (fd<0)
    {
      errno = EBADF;
      return 0;
    }
   SET_KERNEL_FLAG;
   entry = getfdentry(fd);   
   if (entry==NULL)
    {
      CLEAR_KERNEL_FLAG;
      return 0;
    }
   if (entry->count==0 && (entry->flags=fcntl(fd, F_GETFL, 0))==-1)
    {
      int result = errno;
      CLEAR_KERNEL_FLAG;
      errno = result;
      return 0;
    }
#if defined(SCO5)
   entry->blocking = (entry->flags&(O_NONBLOCK|O_NDELAY))==0;
#else
   entry->blocking = (entry->flags&(O_NONBLOCK|FASYNC))==0;
#endif
   if (entry->count++==0 && entry->blocking) {
      fcntl(fd, F_SETFL, entry->flags|O_NONBLOCK|FASYNC);
#ifdef F_SETOWN      
      fcntl(fd, F_SETOWN, getpid());
#endif
   }
   return entry;
}

/*------------------------------------------------------------*/
/*
 * release_fdlock() Release the fdlock and restore fd flags.
 * assume SET_KERNEL_FLAG
 */
static void release_fdlock(int fd)

{
   int result = errno;
   register struct fdentry* entry = getfdentry(fd);
   if (--entry->count==0)
    {
#if defined(SCO5)
      if ((entry->flags&(O_NONBLOCK|O_NDELAY))==0)
#else
      if ((entry->flags&(O_NONBLOCK))==0)
#endif
         fcntl(fd, F_SETFL, entry->flags);
    }
   CLEAR_KERNEL_FLAG;
   errno = result;
}

volatile int fd_started = FALSE;
#ifdef TRASH /* !defined(_M_UNIX)*/
bin_sem_t fd_server_sem;
fd_queue_t fd_wait_read, fd_wait_write;
static fd_set fd_set_read, fd_set_write;

static struct timespec sleeptime = {0, 100000000}; /* 100ms */

/*
 *  bin_sem_init -
 */
void bin_sem_init(bin_sem_t *s)
{
  pthread_mutex_init(&s->lock, NULL);
  pthread_cond_init(&s->cond, NULL);
  s->flag = FALSE;
}

/*
 *  count_sem_init -
 */
void count_sem_init(count_sem_t *s)
{
  pthread_mutex_init(&s->lock, NULL);
  pthread_cond_init(&s->cond, NULL);
  s->count = 0;
}

/*
 *  fd_enq -
 */
fd_queue_elem_t *fd_enq (fd_queue_t q,
                              int fd)
{
  fd_queue_elem_t *new_elem;

  for (new_elem = q->head; new_elem; new_elem = new_elem->next) {
    if (new_elem->fd == fd) {
      new_elem->count++;
      return(new_elem);
    }
  }

  new_elem = (fd_queue_elem_t *) malloc (sizeof(fd_queue_elem_t));

  bin_sem_init(&new_elem->sem);
  new_elem->fd   = fd;
  new_elem->next = q->head;

  if ((q->head) == NULL) {
    q->tail = new_elem;
  }
  q->head = new_elem;
  new_elem->count = 1;
  return(new_elem);
}  

/*
 *  fd_deq -
 */
void fd_deq_elem (fd_queue_t q,
                  fd_queue_elem_t *q_elem)
{
  fd_queue_elem_t *prev = NULL;
  fd_queue_elem_t *next = q->head;
  
  while (next != NULL && next != q_elem) {
    prev = next;
    next = next->next;
  }

  if (next == NULL) {
#ifdef DEBUG
    fprintf(stderr, "fd_deq_elem(elem %x) failed\n", q_elem);
#endif
    return;
  }

  /*
   * other threads still waiting on this element
   */
  if (--next->count != 0)
    return;

  if (prev == NULL) {
    q->head = next->next;
  } else {
    prev->next = next->next;
  }

  if (next->next == NULL) {
    q->tail = prev;
  }
  /* cannot do free(next) since semaphore still used */
}

/*
 *  fd_server - 
 */
void * fd_server (void* arg)
{
  struct timeval    timeout = { 0, 0 };
  struct timespec   state_changed_timeout;
  static struct timeval timepoll = { 0, 0 };
  fd_queue_elem_t   *q_elem, *q_elem_tmp;   
  int               count, ret;
  sigset_t          block;

  sigfillset(&block);
  sigdelset(&block, SIGKILL);
  sigdelset(&block, SIGSTOP);
  sigdelset(&block, SIGIO);
  sigprocmask(SIG_BLOCK, &block, NULL);
  
  while (TRUE) {
    if (fd_wait_read->head || fd_wait_write->head) {

      FD_ZERO(&fd_set_read);
      FD_ZERO(&fd_set_write);

      pthread_mutex_lock (&fd_server_sem.lock);
      
      for (q_elem = fd_wait_read->head; 
           q_elem; 
           q_elem = q_elem->next){
        FD_SET(q_elem->fd, &fd_set_read);
      }
      for (q_elem = fd_wait_write->head; 
           q_elem; 
           q_elem  = q_elem->next) {
        FD_SET(q_elem->fd, &fd_set_write);
      }
      pthread_mutex_unlock (&fd_server_sem.lock);

      /* test, if some of the filedescriptors in fd_set__read/ fd_set_write
       * are ready, timeout immediately
       */
      while ((count = select(FD_SETSIZE, &fd_set_read, &fd_set_write, NULL, 
                          &timeout)) < OK) {
        if (errno != EINTR)
          abort();
      }
      
      if (count) {
	pthread_mutex_lock(&fd_server_sem.lock);
	q_elem = fd_wait_read->head;
	while (q_elem && count) {
	  
	  if (FD_ISSET(q_elem->fd, &fd_set_read)) {
	    FD_CLR(q_elem->fd, &fd_set_read);
	    
	    q_elem_tmp = q_elem->next;
	    
	    pthread_mutex_lock (&(q_elem->sem.lock));
	    q_elem->sem.flag = TRUE;
	    fd_deq_elem (fd_wait_read, q_elem);
	    pthread_mutex_unlock (&(q_elem->sem.lock));          
	    pthread_cond_signal (&(q_elem->sem.cond));
	    
	    q_elem = q_elem_tmp;
	    count--;
	  } else
	    q_elem = q_elem->next;
	}
      
	q_elem = fd_wait_write->head;
	while (q_elem && count) {
	  
	  if (FD_ISSET(q_elem->fd, &fd_set_write)) {
	    FD_CLR(q_elem->fd, &fd_set_write);
	    
	    q_elem_tmp = q_elem->next;
	    
	    pthread_mutex_lock (&(q_elem->sem.lock));
	    q_elem->sem.flag = TRUE;
	    fd_deq_elem (fd_wait_write, q_elem);
	    pthread_mutex_unlock (&(q_elem->sem.lock));          
	    pthread_cond_signal (&(q_elem->sem.cond));
	    
	    q_elem = q_elem_tmp;
	    count--;
	  } else
	    q_elem = q_elem->next;
	}
	pthread_mutex_unlock(&fd_server_sem.lock);
      }
    }
        
    pthread_mutex_lock(&fd_server_sem.lock);
    if (fd_wait_read->head || fd_wait_write->head) {
          clock_gettime (CLOCK_REALTIME, &state_changed_timeout);
      PLUS_NTIME(state_changed_timeout, state_changed_timeout, sleeptime);

      ret = 0;
      while (!fd_server_sem.flag && ret == 0) {
        ret = pthread_cond_timedwait(&fd_server_sem.cond,
                                     &fd_server_sem.lock,
                                     &state_changed_timeout);
      }
    }  else {
      /* since fd_read and fd_write are empty, wait, until
       * one of them has changed
       */
      while (!fd_server_sem.flag) {
        pthread_cond_wait(&fd_server_sem.cond,
                          &fd_server_sem.lock);
      }
    }
    fd_server_sem.flag = FALSE;
    pthread_mutex_unlock(&fd_server_sem.lock);
  }
}
#endif /* !_M_UNIX */

#if defined(SCO5)

typedef struct
{
  int refcnt;
  pthread_t tid;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} pthread_rmutex_t;

/* Global mutex to reentrant FILE I/O SCO libraries
 */
static 	pthread_rmutex_t stdlock_mutex;
/* To prevent recursive mutex invocation when we are using shared library
 * no compiled with pthreads.
 */

static void pthread_rmutex_init(pthread_rmutex_t* rmutex,pthread_mutexattr_t* attr)

{
    rmutex->refcnt = 0;
    rmutex->tid = NULL;
    pthread_mutex_init(&rmutex->mutex, NULL);
    pthread_cond_init(&rmutex->cond, NULL);
}

static int pthread_rmutex_lock(pthread_rmutex_t* rmutex)

{
    /* Obtiene el mutex sobre la OTable.
     */
    pthread_mutex_lock (&rmutex->mutex);
    while (rmutex->refcnt>0 && rmutex->tid!=pthread_self()) {
      pthread_cond_wait(&rmutex->cond, &rmutex->mutex);
    }
 
    if (rmutex->refcnt++==0) {
       rmutex->tid = pthread_self();
    }
    /* Libera el mutex sobre OTable.
     */
    pthread_mutex_unlock(&rmutex->mutex);
    return 0;  
}

static int pthread_rmutex_unlock (pthread_rmutex_t* rmutex)

{
    int contention;
    /* Obtiene el mutex sobre OTable.
     */
    pthread_mutex_lock (&rmutex->mutex);
    if  (--rmutex->refcnt==0);
       pthread_cond_signal (&rmutex->cond);

    pthread_mutex_unlock(&rmutex->mutex);
    return 0;
}

struct stdlock {
    volatile long init;
    pthread_rmutex_t* mutex;
};

static void stdlock(struct stdlock* lock)

{
	pthread_rmutex_lock(&stdlock_mutex);
}

static void stdunlock(struct stdlock* lock)

{
	pthread_rmutex_unlock(&stdlock_mutex);
}

static int stdtrylock(struct stdlock* lock)

{
	return(FALSE);
}

static int stdtryunlock(struct stdlock* lock)

{
	return(FALSE);
}

struct filebuf
{
	int		__cnt;	/* 0 */
	unsigned char	*__ptr;	/* 4 */
	unsigned char	*__base; /* 8 */
	unsigned char	__flag;	/* c */
	unsigned char	__file;	/* d */
	unsigned char	__buf[2];/* e */
        int d0x10; 
        char* d0x14;
        char* d0x18;
        int d0x1c;
        int d0x20;
        int d0x24;
        struct stdlock lock;
        int d0x30;
        int d0x34;
        int d0x38;
        int d0x3c;
};

extern void (*_libc_stdlock)(struct stdlock* lock);
extern void (*_libc_stdunlock)(struct stdlock* lock);
extern int (*_libc_stdtrylock)(struct stdlock* lock);
extern int (*_libc_stdtryunlock)(struct stdlock* lock);

#if !(defined(DEBUG) || defined(TIMER_DEBUG) || defined(IO_DEBUG))
int sprintf(char* s, const char* format, ...)

{
   int len; /* [ebp-0x48] */
   va_list ap; /* [ebp-0x44] */
   struct filebuf f; /* [ebp-0x40] */

   va_start(ap, format);
   f.__base = (unsigned char*) &f;
   f.__ptr = s;
   f.__cnt = 0x7fffffff;
   f.__flag = 0x2;
   f.d0x14 = s;
   f.d0x18 = s;
   f.d0x20 = -1;
   f.lock.init = 0;
   if (!is_in_kernel)
   stdlock(&f.lock);
   len = _idoprnt(&f, format, ap);
   if (!is_in_kernel)
   stdunlock(&f.lock);
   *f.__ptr = '\0';
   return len;
}
#endif

#endif /* SCO5 */

/*
 *  fd_init - 
 */
void fd_init(void)
{
#ifdef TRASH /*!defined(_M_UNIX)*/
  int i;
  pthread_t server_thread;
  pthread_attr_t attrs;
  struct sched_param param;

  if (fd_started)
    return;

  fd_started = TRUE;

  /*
   * Init the fdtable
   */
  fdtableinit();
  bin_sem_init(&fd_server_sem);

  fd_wait_read  = (fd_queue_t) malloc (sizeof (struct fd_queue));
  fd_wait_read->head = NULL;
  fd_wait_read->tail = NULL;

  fd_wait_write = (fd_queue_t) malloc (sizeof (struct fd_queue));
  fd_wait_write->head = NULL;
  fd_wait_write->tail = NULL;

  pthread_attr_init( &attrs );
  param.sched_priority = sched_get_priority_max(SCHED_FIFO);
  if (!pthread_attr_setschedparam(&attrs, &param))
    pthread_create(&server_thread, &attrs, fd_server, (void *) NULL);     

#elif defined(_M_UNIX)

  /* SCO Open Server 3.2v5 is thread aware, use thread aware code to with
   * FSU pthreads.
   */
#if defined(SCO5)
  typedef void (*voidfunc_t)();
  /* At the moment we only are this routines in a next opportunity will expand
   * support to readv, writev, etc.
   */
  extern voidfunc_t _libc_read;
  extern voidfunc_t _libc_write;
  extern voidfunc_t _libc_select;
  ssize_t pthread_read (int , void*, size_t);
  ssize_t pthread_write (int , const void*, size_t);
  int pthread_select(int, fd_set *, fd_set *, fd_set *, struct timeval *);

  static struct {
	voidfunc_t *p;
	voidfunc_t f;
   } maptable[] = {
	{(voidfunc_t*)&_libc_read, (voidfunc_t) pthread_read},
	{(voidfunc_t*)&_libc_write, (voidfunc_t) pthread_write},
	{(voidfunc_t*)&_libc_select, (voidfunc_t) pthread_select},
	{(voidfunc_t*) 0, (voidfunc_t) 0}
	}; 
	register int i;
#endif

  if (fd_started)
    return;

  fd_started = TRUE;

  /*
   * Init the fdtable
   */
  fdtableinit();

#ifdef SCO5
  /*
   * Init the FILE IO mutex
   */
  pthread_rmutex_init(&stdlock_mutex, NULL);

  /*
   * lock routines as used internally by SCO to lock FILE IO structures.
   */
  _libc_stdlock = stdlock;
  _libc_stdunlock = stdunlock;
  _libc_stdtrylock = stdtrylock;
  _libc_stdtryunlock = stdtryunlock;

  /* Set mapping handling to pthread aware code.
   */
  for (i=0; maptable[i].p; i++)
      *maptable[i].p = maptable[i].f;
#endif /* SCO5 */
#endif /* _M_UNIX */
}

#ifdef SCO5
/*
 *  fd_reader_wait - wait for fd ready for read
 */
int fd_reader_wait (int fd, struct timeval* timeout)

{
#if !defined(_M_UNIX)
  int last;
  fd_queue_elem_t *q_elem;
  pthread_mutex_lock (&fd_server_sem.lock);
  q_elem = fd_enq (fd_wait_read, fd);
  fd_server_sem.flag = TRUE;
  pthread_mutex_unlock (&fd_server_sem.lock);
  pthread_cond_signal (&fd_server_sem.cond);

  pthread_mutex_lock(&(q_elem->sem.lock));
  while (!(q_elem->sem.flag)) {
     pthread_cond_wait (&(q_elem->sem.cond), &(q_elem->sem.lock));
  }
  q_elem->sem.flag = FALSE;
  last = (q_elem->count == 0);
  pthread_mutex_unlock(&(q_elem->sem.lock));
  if (last)
     free(q_elem); /* q_elem semaphore used up until here */
  return 0;
#else
  pthread_t p;
  int result;
  struct timespec p_timeout;
#ifdef USE_POLL
  struct pollfd fds[1];
#endif

  ACCESS_STACK;
  
  p = mac_pthread_self();
  SET_KERNEL_FLAG;
#ifdef USE_POLL
  fds[0].fd = fd;
  fds[0].events = POLLIN;
  fds[0].revents = 0;
  p->nfds = 1;
  p->fds = fds;
  p->wait_on_select = TRUE;
#else
  p->width = fd+1;
  pthread_fds_zero(&(p->readfds), p->width);
  FD_SET(fd, &(p->readfds));
  pthread_fds_zero(&(p->writefds), p->width);
  pthread_fds_zero(&(p->exceptfds), p->width);
  p->wait_on_select = TRUE;
#endif
  p->how_many = 0;
  
  if (timeout) {
    U2P_TIME(p_timeout, *timeout);
    if (pthread_timed_sigwait(p, &p_timeout, REL_TIME, NULL, NULL)) {
      CLEAR_KERNEL_FLAG;
      return(-1);
    }
    p->state |= T_SYNCTIMER;
  }
#ifdef USE_POLL
  gpoll_fds_union(1, fds);
#else
  gwidth = MAX(gwidth, fd+1);
  FD_SET(fd, &greadfds);
#endif
  set_errno(0);
  sigaddset(&p->sigwaitset, AIO_SIG);
  p->state &= ~T_RUNNING;
  p->state |= T_BLOCKED | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  if (get_errno())
   {
     return -1;
   }

  if (p->wait_on_select)
    return(0);

#ifdef USE_POLL
  if (p->how_many==1)
   {
     if (fds[0].revents&POLLNVAL)
      {
        set_errno(EBADF);
        p->how_many = -1;
      }
   }
#endif

  return(p->how_many);
#endif
}

/*
 *  fd_writer_wait - wait for fd ready for write
 */
int fd_writer_wait (int fd, struct timeval* timeout)

{
#if !defined(_M_UNIX)
  int last;
  fd_queue_elem_t *q_elem;
  pthread_mutex_lock (&fd_server_sem.lock);
  q_elem = fd_enq (fd_wait_write, fd);
  fd_server_sem.flag = TRUE;
  pthread_mutex_unlock (&fd_server_sem.lock);
  pthread_cond_signal (&fd_server_sem.cond);

  pthread_mutex_lock(&(q_elem->sem.lock));
  while (!(q_elem->sem.flag)) {
     pthread_cond_wait (&(q_elem->sem.cond), &(q_elem->sem.lock));
  }
  q_elem->sem.flag = FALSE;
  last = (q_elem->count == 0);
  pthread_mutex_unlock(&(q_elem->sem.lock));
  if (last)
     free(q_elem); /* q_elem semaphore used up until here */
  return 0;
#else
  register pthread_t p;
  register int result;
  struct timespec p_timeout;
#ifdef USE_POLL
  struct pollfd fds[1];
#endif
  
  ACCESS_STACK;
  
  p = mac_pthread_self();
  SET_KERNEL_FLAG;
#ifdef USE_POLL
  fds[0].fd = fd;
  fds[0].events = POLLOUT;
  fds[0].revents = 0;
  p->nfds = 1;
  p->fds = fds;
  p->wait_on_select = TRUE;
#else
  p->width = fd+1;
  pthread_fds_zero(&(p->readfds), p->width);
  pthread_fds_zero(&(p->writefds), p->width);
  FD_SET(fd, &(p->writefds));
  pthread_fds_zero(&(p->exceptfds), p->width);
  p->wait_on_select = TRUE;
#endif
  p->how_many = 0;
  
  if (timeout) {
    U2P_TIME(p_timeout, *timeout);
    if (pthread_timed_sigwait(p, &p_timeout, REL_TIME, NULL, NULL)) {
      CLEAR_KERNEL_FLAG;
      return(-1);
    }
    p->state |= T_SYNCTIMER;
  }
#ifdef USE_POLL
  gpoll_fds_union(1, fds);
#else
  gwidth = MAX(gwidth, fd+1);
  FD_SET(fd, &gwritefds);
#endif
  set_errno(0);
  sigaddset(&p->sigwaitset, AIO_SIG);
  p->state &= ~T_RUNNING;
  p->state |= T_BLOCKED | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  if (get_errno())
   {
     return -1;
   }

  if (p->wait_on_select)
    return(0);

#ifdef USE_POLL
  if (p->how_many==1)
   {
     if (fds[0].revents&POLLNVAL)
      {
        set_errno(EBADF);
        p->how_many = -1;
      }
   }
#endif

  return(p->how_many);
#endif
}
#endif

#if defined(__FreeBSD__) || defined(__linux__) || (defined(_M_UNIX) && !defined(SCO5))
/*
 * read - 
 */
ssize_t read(int    fd, 
             void   *buf, 
             size_t nbytes)
{
  int mode;
  pthread_t p;
  int result;

  /*
   * If the mode is O_NDELAY perform a Non Blocking recvfrom & return 
   * immediately.
   */
  if ((mode = fcntl(fd, F_GETFL, 0)) & (O_NDELAY | O_NONBLOCK)) {
    return (pthread_sys_read(fd, buf, nbytes));
  }

  /*
   * Else issue an asynchronous request.
   */
  mode = fcntl(fd, F_GETFL, 0);
  if (fcntl(fd, F_SETFL, (mode | O_NDELAY | FASYNC)) < 0)
    return (-1);
  if (fcntl(fd, F_SETOWN, getpid()) < 0) {
    fcntl (fd, F_SETFL, mode);
    return(-1);
  }

  while (TRUE) {
#ifdef DEBUG
    printf("Try to read\n");
#endif
    result = pthread_sys_read(fd, buf, nbytes);
    if (result != -1) {
      if (fcntl(fd, F_SETFL, mode) != -1)
        return(result);
      return (-1);
    }
    if (errno != EWOULDBLOCK) {
      fcntl(fd, F_SETFL, mode);
      return(-1);
    }
#ifdef DEBUG
    printf("Unsuccessfull\n");
#endif /* DEBUG */
    SET_KERNEL_FLAG; /* No preemption */
    p = mac_pthread_self();

    gwidth = MAX(gwidth, fd+1);
    p->width = fd+1;
    pthread_fds_zero(&(p->readfds), p->width);
    FD_SET(fd, &(p->readfds));
    FD_SET(fd, &greadfds);
    pthread_fds_zero(&(p->writefds), p->width);
    pthread_fds_zero(&(p->exceptfds), p->width);

    sigaddset(&p->sigwaitset, AIO_SIG);
    p->state &= ~T_RUNNING;
    p->state |= T_BLOCKED | T_INTR_POINT;
    if (sigismember(&p->pending, SIGCANCEL) &&
	!sigismember(&p->mask, SIGCANCEL))
      SIG_CLEAR_KERNEL_FLAG(TRUE);
    else {
      pthread_q_deq_head(&ready, PRIMARY_QUEUE);
      SIM_SYSCALL(TRUE);
      CLEAR_KERNEL_FLAG;
    }
  }
}

/*
 * write - 
 */
ssize_t write(int        fd, 
              const void *buf, 
              size_t     nbytes)
{
  int mode;
  pthread_t p;
  int result;

  /*
   * If the mode is O_NDELAY perform a Non Blocking recvfrom & return 
   * immediately.
   */
  if ((mode = fcntl(fd, F_GETFL, 0)) & (O_NDELAY | O_NONBLOCK)) {
    return pthread_sys_write(fd, buf, nbytes);
  }

  /*
   * Else issue an asynchronous request.
   */
  mode = fcntl(fd, F_GETFL, 0);
  if (fcntl(fd, F_SETFL, (mode | O_NDELAY | FASYNC)) < 0)
    return (-1);
  if (fcntl(fd, F_SETOWN, getpid()) < 0) {
    fcntl (fd, F_SETFL, mode);
    return(-1);
  }

  while (TRUE) {
#ifdef DEBUG
    printf("Try to read\n");
#endif
    result = pthread_sys_write(fd, buf, nbytes);
    if (result != -1) {
      if (fcntl(fd, F_SETFL, mode) != -1)
        return(result);
      return (-1);
    }
    if (errno != EWOULDBLOCK) {
      fcntl(fd, F_SETFL, mode);
      return(-1);
    }
#ifdef DEBUG
    printf("Unsuccessfull\n");
#endif /* DEBUG */
    SET_KERNEL_FLAG; /* No preemption */
    p = mac_pthread_self();

    gwidth = MAX(gwidth, fd+1);
    p->width = fd+1;
    pthread_fds_zero(&(p->readfds), p->width);
    pthread_fds_zero(&(p->writefds), p->width);
    FD_SET(fd, &(p->writefds));
    FD_SET(fd, &gwritefds);
    pthread_fds_zero(&(p->exceptfds), p->width);

    sigaddset(&p->sigwaitset, AIO_SIG);
    p->state &= ~T_RUNNING;
    p->state |= T_BLOCKED | T_INTR_POINT;
    if (sigismember(&p->pending, SIGCANCEL) &&
	!sigismember(&p->mask, SIGCANCEL))
      SIG_CLEAR_KERNEL_FLAG(TRUE);
    else {
      pthread_q_deq_head(&ready, PRIMARY_QUEUE);
      SIM_SYSCALL(TRUE);
      CLEAR_KERNEL_FLAG;
    }
  }
}

/*
 * readv - 
 */
ssize_t readv(int fd, READV_CONST struct iovec *iovec, int len)
{
  int mode;
  pthread_t p;
  int result;

  /*
   * If the mode is O_NDELAY perform a Non Blocking recvfrom & return 
   * immediately.
   */
  if ((mode = fcntl(fd, F_GETFL, 0)) & (O_NDELAY | O_NONBLOCK)) {
    return (pthread_sys_readv(fd, iovec, len));
  }

  /*
   * Else issue an asynchronous request.
   */
  mode = fcntl(fd, F_GETFL, 0);
  if (fcntl(fd, F_SETFL, (mode | O_NDELAY | FASYNC)) < 0)
    return (-1);
  if (fcntl(fd, F_SETOWN, getpid()) < 0) {
    fcntl (fd, F_SETFL, mode);
    return(-1);
  }

  while (TRUE) {
#ifdef DEBUG
    printf("Try to read\n");
#endif
  result = pthread_sys_readv(fd, iovec, len);
    if (result != -1) {
      if (fcntl(fd, F_SETFL, mode) != -1)
        return(result);
      return (-1);
    }
    if (errno != EWOULDBLOCK) {
      fcntl(fd, F_SETFL, mode);
      return(-1);
    }
#ifdef DEBUG
    printf("Unsuccessfull\n");
#endif /* DEBUG */
    SET_KERNEL_FLAG; /* No preemption */
    p = mac_pthread_self();

    gwidth = MAX(gwidth, fd+1);
    p->width = fd+1;
    pthread_fds_zero(&(p->readfds), p->width);
    FD_SET(fd, &(p->readfds));
    FD_SET(fd, &greadfds);
    pthread_fds_zero(&(p->writefds), p->width);
    pthread_fds_zero(&(p->exceptfds), p->width);

    sigaddset(&p->sigwaitset, AIO_SIG);
    p->state &= ~T_RUNNING;
    p->state |= T_BLOCKED | T_INTR_POINT;
    if (sigismember(&p->pending, SIGCANCEL) &&
	!sigismember(&p->mask, SIGCANCEL))
      SIG_CLEAR_KERNEL_FLAG(TRUE);
    else {
      pthread_q_deq_head(&ready, PRIMARY_QUEUE);
      SIM_SYSCALL(TRUE);
      CLEAR_KERNEL_FLAG;
    }
  }
}

/*
 * writev - 
 */
ssize_t writev(int fd, WRITEV_CONST struct iovec *iovec, int len)
{
  int mode;
  pthread_t p;
  int result;

  /*
   * If the mode is O_NDELAY perform a Non Blocking recvfrom & return 
   * immediately.
   */
  if ((mode = fcntl(fd, F_GETFL, 0)) & (O_NDELAY | O_NONBLOCK)) {
    return pthread_sys_writev(fd, iovec, len);
  }

  /*
   * Else issue an asynchronous request.
   */
  mode = fcntl(fd, F_GETFL, 0);
  if (fcntl(fd, F_SETFL, (mode | O_NDELAY | FASYNC)) < 0)
    return (-1);
  if (fcntl(fd, F_SETOWN, getpid()) < 0) {
    fcntl (fd, F_SETFL, mode);
    return(-1);
  }

  while (TRUE) {
#ifdef DEBUG
    printf("Try to read\n");
#endif
    result = pthread_sys_writev(fd, iovec, len);
    if (result != -1) {
      if (fcntl(fd, F_SETFL, mode) != -1)
        return(result);
      return (-1);
    }
    if (errno != EWOULDBLOCK) {
      fcntl(fd, F_SETFL, mode);
      return(-1);
    }
#ifdef DEBUG
    printf("Unsuccessfull\n");
#endif /* DEBUG */
    SET_KERNEL_FLAG; /* No preemption */
    p = mac_pthread_self();

    gwidth = MAX(gwidth, fd+1);
    p->width = fd+1;
    pthread_fds_zero(&(p->readfds), p->width);
    pthread_fds_zero(&(p->writefds), p->width);
    FD_SET(fd, &(p->writefds));
    FD_SET(fd, &gwritefds);
    pthread_fds_zero(&(p->exceptfds), p->width);

    sigaddset(&p->sigwaitset, AIO_SIG);
    p->state &= ~T_RUNNING;
    p->state |= T_BLOCKED | T_INTR_POINT;
    if (sigismember(&p->pending, SIGCANCEL) &&
	!sigismember(&p->mask, SIGCANCEL))
      SIG_CLEAR_KERNEL_FLAG(TRUE);
    else {
      pthread_q_deq_head(&ready, PRIMARY_QUEUE);
      SIM_SYSCALL(TRUE);
      CLEAR_KERNEL_FLAG;
    }
  }
}

#elif defined(SCO5)
/*
 * read - 
 */
ssize_t pthread_read(int    fd, 
             void   *buf, 
             size_t nbytes)
{
  int ret;
  register struct fdentry* entry;
  extern int pthread_sys_read();
  extern int fd_reader_wait();

  if ((entry=acquire_fdlock(fd))==0) {
     return(NOTOK);
   }

  while ((ret = pthread_sys_read(fd, buf, nbytes)) < OK) {
    switch(get_errno()) {
    case EWOULDBLOCK:
#ifndef __USE_POSIX
         if (entry->blocking) {
#endif
            release_fdlock(fd);
            if (fd_reader_wait (fd, NULL)==-1)
             return (NOTOK);
            if ((entry=acquire_fdlock(fd))==0)
             {
               return(NOTOK);
             }
            break;
#ifndef __USE_POSIX
          }
#endif
    default:
         release_fdlock(fd);
         return (NOTOK);

#ifdef ERESTART
    case ERESTART:
#endif
    case EINTR:
         break;
    }
  }      
  release_fdlock(fd);
  return(ret);
}

/*
 * write - 
 */
ssize_t pthread_write(int        fd, 
              const void *buf, 
              size_t     nbytes)
{
  int ret;
  register struct fdentry* entry;
  extern int pthread_sys_write();
  extern int fd_writer_wait();

  if ((entry=acquire_fdlock(fd))==0) {
     return(NOTOK);
   }
  while ((ret = pthread_sys_write(fd, buf, nbytes)) < OK) {
    switch(get_errno()) {
    case EWOULDBLOCK:
#ifndef __USE_POSIX
         if (entry->blocking) {
#endif
            release_fdlock(fd);
            if (fd_writer_wait (fd, NULL)==-1)
             return (NOTOK);
            if ((entry=acquire_fdlock(fd))==0)
             {
          #if defined(IO_DEBUG)
               fprintf(stderr, "acquire_fdlock failed: errno=%d\n", errno); fflush(stderr);
          #endif
               return(NOTOK);
             }
            break;
#ifndef __USE_POSIX
          }
#endif
    default:
         release_fdlock(fd);
         return (NOTOK);

#ifdef ERESTART
    case ERESTART:
#endif
    case EINTR:
         break;
    }
  }      
  release_fdlock(fd);
  return(ret);
}

/*
 * readv - 
 */
ssize_t pthread_readv(int fd, const struct iovec *iovec, int len)
{
  int ret;
  register struct fdentry* entry;
  extern int pthread_sys_read();
  extern int fd_reader_wait();

  if ((entry=acquire_fdlock(fd))==0) {
     return(NOTOK);
   }

  while ((ret = pthread_sys_readv(fd, iovec, len)) < OK) {
    switch(get_errno()) {
    case EWOULDBLOCK:
#ifndef __USE_POSIX
         if (entry->blocking) {
#endif
            release_fdlock(fd);
            if (fd_reader_wait (fd, NULL)==-1)
             return (NOTOK);
            if ((entry=acquire_fdlock(fd))==0)
             {
               return(NOTOK);
             }
            break;
#ifndef __USE_POSIX
          }
#endif
    default:
         release_fdlock(fd);
         return (NOTOK);

#ifdef ERESTART
    case ERESTART:
#endif
    case EINTR:
         break;
    }
  }      
  release_fdlock(fd);
  return(ret);
}

/*
 * writev - 
 */
ssize_t pthread_writev(int fd, const struct iovec *iovec, int len)
{
  int ret;
  register struct fdentry* entry;
  extern int pthread_sys_write();
  extern int fd_writer_wait();

  if ((entry=acquire_fdlock(fd))==0) {
     return(NOTOK);
   }
  while ((ret = pthread_sys_writev(fd, iovec, len)) < OK) {
    switch(get_errno()) {
    case EWOULDBLOCK:
#ifndef __USE_POSIX
         if (entry->blocking) {
#endif
            release_fdlock(fd);
            if (fd_writer_wait (fd, NULL)==-1)
             return (NOTOK);
            if ((entry=acquire_fdlock(fd))==0)
             {
          #if defined(IO_DEBUG)
               fprintf(stderr, "acquire_fdlock failed: errno=%d\n", errno); fflush(stderr);
          #endif
               return(NOTOK);
             }
            break;
#ifndef __USE_POSIX
          }
#endif
    default:
         release_fdlock(fd);
         return (NOTOK);

#ifdef ERESTART
    case ERESTART:
#endif
    case EINTR:
         break;
    }
  }      
  release_fdlock(fd);
  return(ret);
}

/*
 * getmsg - 
 */
int getmsg(int    fd, 
             struct strbuf   *ctlptr, 
             struct strbuf   *dataptr, 
             int *flagsp)
{
  int ret;
  register struct fdentry* entry;
  extern int pthread_sys_getmsg();
  extern int fd_reader_wait();

  if ((entry=acquire_fdlock(fd))==0)
   {
     return(NOTOK);
   }

  while ((ret = pthread_sys_getmsg(fd, ctlptr, dataptr, flagsp)) < OK) {
    switch(get_errno()) {
    case EWOULDBLOCK:
#ifndef __linux__
    case EAGAIN:
#endif
         if (entry->blocking)
          {
            release_fdlock(fd);
            if (fd_reader_wait (fd, NULL)==-1)
             return (NOTOK);
            if ((entry=acquire_fdlock(fd))==0)
             {
               return(NOTOK);
             }
            break;
          }
    default: 
         release_fdlock(fd);
         return (NOTOK);

#ifdef ERESTART
    case ERESTART:
#endif
    case EINTR:
         break;
    }
  }      
  release_fdlock(fd);
  return(ret);
}
#endif

#else /* !(__FreeBSD__ || _M_UNIX || __linux__) */

/*------------------------------------------------------------*/
/*
 * read - Same as POSIX.1 read except that it blocks only the current thread
 * rather than entire process.
 */
int READ(fd, buf, nbytes)
     int fd;
     void *buf;
     IO_SIZE_T nbytes;
{
  int mode;
  struct iovec iov[1];
  pthread_t p;
  struct timeval timeout;


  /*
   * If the mode is O_NDELAY perform a Non Blocking read & return immediately.
   */
  if ((mode = fcntl(fd, F_GETFL, 0)) & (O_NDELAY|O_NONBLOCK)) {
    iov[0].iov_base = buf;
    iov[0].iov_len = nbytes;
    return(readv(fd, iov, 1));
  }

  ACCESS_STACK;

  /*
   * Else issue an asynchronous request for nbytes.
   */
  timeout.tv_sec        = 0;
  timeout.tv_usec       = 0;
  p = mac_pthread_self();
  SET_KERNEL_FLAG;
  p->resultp.aio_return = AIO_INPROGRESS;

  if (aioread(fd, buf, nbytes, 0l, SEEK_CUR,
	      (struct aio_result_t *) &p->resultp) < 0) {
    CLEAR_KERNEL_FLAG;
    return(-1);
  }

  if (p->resultp.aio_return != AIO_INPROGRESS) {
    if (p->resultp.aio_return != -1)
      lseek(fd, p->resultp.aio_return, SEEK_CUR);
    else
      set_errno (p->resultp.aio_errno);
    p->state |= T_IO_OVER;
    CLEAR_KERNEL_FLAG;
    aiowait(&timeout);
    return(p->resultp.aio_return);
  }
  sigaddset(&p->sigwaitset, AIO_SIG);
  p->width = 0;
  p->state &= ~T_RUNNING;
  p->state |= T_BLOCKED | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) && !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  /*
   * The case when the read() is interrupted (when the thread receives a
   * signal other than SIGIO), read() returns -1 and errno is set to EINTR
   * (this is done in signal.c when the thread is woken up).
   */
  switch (p->resultp.aio_return) {
  case AIO_INPROGRESS:
    aiocancel((struct aio_result_t *) &p->resultp);
    return (-1);
  case -1:
    aiowait(&timeout);
    set_errno (p->resultp.aio_errno);
    return (-1);
  default:
    aiowait(&timeout);
    lseek(fd, p->resultp.aio_return, SEEK_CUR);
    return(p->resultp.aio_return);
  }
}

/*------------------------------------------------------------*/
/*
 * write - Same as POSIX.1 write except that it blocks only the current
 * thread rather than entire process.
 */
int WRITE(fd, buf, nbytes)
     int fd;
     const void *buf;
     IO_SIZE_T nbytes;
{

  int mode;
  struct iovec iov[1];
  pthread_t p;
  struct timeval timeout;

  /*
   * If the mode is O_NDELAY perform a Non Blocking write & return immediately.
   */
  if ((mode = fcntl(fd, F_GETFL, 0)) & (O_NDELAY|O_NONBLOCK)) {
    iov[0].iov_base = (caddr_t) buf;
    iov[0].iov_len = nbytes;
    return (writev(fd, iov, 1));
  }

  ACCESS_STACK;

  /*
   * Else issue an asynchronous request for nbytes.
   */
  timeout.tv_sec        = 0;
  timeout.tv_usec       = 0;
  p = mac_pthread_self();
  SET_KERNEL_FLAG;
  p->resultp.aio_return = AIO_INPROGRESS;

  if (aiowrite(fd, (char *) buf, nbytes, 0l, SEEK_CUR,
	       (struct aio_result_t *) &p->resultp) < 0) {
    CLEAR_KERNEL_FLAG;
    return(-1);
  }

  if (p->resultp.aio_return != AIO_INPROGRESS) {
    if (p->resultp.aio_return != -1)
      lseek(fd, p->resultp.aio_return, SEEK_CUR);
    else
      set_errno(p->resultp.aio_errno);
    p->state |= T_IO_OVER;
    CLEAR_KERNEL_FLAG;
    aiowait(&timeout);
    return(p->resultp.aio_return);
  }
  sigaddset(&p->sigwaitset, AIO_SIG);
  p->width = 0;
  p->state &= ~T_RUNNING;
  p->state |= T_BLOCKED | T_INTR_POINT; 
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }
  switch (p->resultp.aio_return) {
  case AIO_INPROGRESS:
    aiocancel((struct aio_result_t *) &p->resultp);
    return(-1);
  case -1:
    aiowait(&timeout);
    set_errno (p->resultp.aio_errno);
    return(-1);
  default:
    aiowait(&timeout);
    lseek(fd, p->resultp.aio_return, SEEK_CUR);
    return(p->resultp.aio_return);
  }
}

#endif /* !(__FreeBSD__ || _M_UNIX || __linux__ || __dos__) */

#if defined(IO) && !defined(SVR4) && !defined(__FreeBSD__) && !defined(__dos__)
/*------------------------------------------------------------*/
/*
 * select - Same as BSD select except that it blocks only the current thread
 * rather than entire process.
 */
#if defined(SCO5)
int pthread_select(width, readfds, writefds, exceptfds, timeout)
     int width;
     fd_set *readfds, *writefds, *exceptfds;
     struct timeval *timeout;
#else
int select(width, readfds, writefds, exceptfds, timeout)
     int width;
     fd_set *readfds, *writefds, *exceptfds;
     struct timeval *timeout;
#endif
{
  struct timeval mintimeout = {0, 0};
  pthread_t p;
  int result = 0;
#ifdef USE_POLL
  int nfds;
  struct pollfd* fds;
#endif
  struct timespec p_timeout;
 
  ACCESS_STACK;
  
  p = mac_pthread_self();
  SET_KERNEL_FLAG;
 
#ifdef USE_POLL
  if (width>0)
     fds = (struct pollfd*) malloc(sizeof(struct pollfd)* width);
  else
     fds = NULL;
  nfds = fd_set_to_pollfd(width, readfds, writefds, exceptfds, fds);
#else 
#if !defined(_M_UNIX)
  pthread_fds_set_async(readfds, width);
  pthread_fds_set_async(writefds, width);
  pthread_fds_set_async(exceptfds, width); 
#endif
  pthread_fds_set(&(p->readfds), readfds, width);
  pthread_fds_set(&(p->writefds), writefds, width);
  pthread_fds_set(&(p->exceptfds), exceptfds, width); 
#endif

#ifdef USE_POLL
  if (nfds>0) result = POLL(fds, nfds, 0);
  if (result != 0)
   {
     if (result > 0)
       result = pollfd_to_fd_set(nfds, fds, readfds, writefds, exceptfds, width);
     if (fds) {
       int save_errno = errno;
       free(fds);
       errno = save_errno;
     }
     CLEAR_KERNEL_FLAG;
     return result;
   }
#else  
  result = SELECT(width, readfds, writefds, exceptfds, &mintimeout);
  if (result != 0) {
    CLEAR_KERNEL_FLAG;
    return result;
  }
#endif

  if (timeout && !timerisset(timeout)) {
    pthread_fds_set(readfds, NULL, width);  
    pthread_fds_set(writefds, NULL, width); 
    pthread_fds_set(exceptfds, NULL, width);
#ifdef USE_POLL
    if (fds)
      free(fds);
#endif
    CLEAR_KERNEL_FLAG;
    return 0;
  }
 
#ifdef USE_POLL 
  nfds = fd_set_to_pollfd(width, readfds, writefds, exceptfds, fds);
#endif
  if (timeout) {
    U2P_TIME(p_timeout, *timeout);
    if (pthread_timed_sigwait(p, &p_timeout, REL_TIME, NULL, NULL)) {
      CLEAR_KERNEL_FLAG;
#ifdef USE_POLL
      if (fds)
       {
         free(fds);
       }
#endif
      switch(get_errno()) {
      case EAGAIN:
#ifdef ETIME
      case ETIME:
#endif
           pthread_fds_set(readfds, NULL, width);  
           pthread_fds_set(writefds, NULL, width); 
           pthread_fds_set(exceptfds, NULL, width);
           return 0;

      default:
           set_errno(result);
           return(-1);
      }
    }
    p->state |= T_SYNCTIMER;
  }
#ifdef USE_POLL
  p->nfds = nfds;
  p->fds = fds;
  gpoll_fds_union(p->nfds, p->fds);
#else
  p->width = width;
  gwidth = MAX(gwidth, width);
  pthread_fds_union(&greadfds, &(p->readfds), width);
  pthread_fds_union(&gwritefds, &(p->writefds), width);
  pthread_fds_union(&gexceptfds, &(p->exceptfds), width);
#endif
  if (width>0)
   {
     p->wait_on_select = TRUE;
     sigaddset(&p->sigwaitset, AIO_SIG);
   }
  else
     p->wait_on_select = FALSE;
  p->how_many = 0;
  set_errno(0);
  p->state &= ~T_RUNNING;
  p->state |= T_BLOCKED | T_INTR_POINT;
  if (sigismember(&p->pending, SIGCANCEL) &&
      !sigismember(&p->mask, SIGCANCEL))
    SIG_CLEAR_KERNEL_FLAG(TRUE);
  else {
    pthread_q_deq_head(&ready, PRIMARY_QUEUE);
    SIM_SYSCALL(TRUE);
    CLEAR_KERNEL_FLAG;
  }

  if (get_errno())
   {
     result = get_errno();
#ifdef USE_POLL
     if (fds) free(fds);
#endif
     set_errno(result);
     return -1;
   }

#ifndef _M_UNIX
  pthread_fds_clr_async(readfds, width);
  pthread_fds_clr_async(writefds, width);
  pthread_fds_clr_async(exceptfds, width); 
#endif

  if (p->wait_on_select)
   {
#ifdef USE_POLL
     if (fds) free(fds);
#endif
    return(0);
   }

  if (p->how_many > 0) {
#ifdef USE_POLL
     p->how_many = 
	pollfd_to_fd_set(nfds, fds, readfds, writefds, exceptfds, width);
#else
     if (readfds)
        pthread_fds_set(readfds, &(p->readfds), width);  
     if (writefds)
        pthread_fds_set(writefds, &(p->writefds), width); 
     if (exceptfds)
        pthread_fds_set(exceptfds, &(p->exceptfds), width);
#endif
  }
#ifdef USE_POLL
  result = get_errno();
  if (fds) free(fds);
  set_errno(result);
#endif
  return(p->how_many);
}
#endif /* IO && DEF_RR && !SVR4 && !__FreeBSD__ && !__linux__ && !__dos__ */

#if defined(IO) && !defined(__FreeBSD__) && !defined(SVR4)
/*------------------------------------------------------------*/
/*
 * accept - Same as BSD accept except that it blocks only the current thread
 * rather than entire process.
 */
int accept(s, addr, addrlen)
     int s;
#if defined(__SOCKADDR_ARG) || defined(__SOCKADDR_ALLTYPES)
     __SOCKADDR_ARG addr;
     socklen_t *addrlen;
#else
     struct sockaddr *addr;
     int *addrlen;
#endif
{
#if !defined(_M_UNIX)
  int mode;
  int flags, blocking;
  pthread_t p;
  struct timeval timeout;
  int result;

#if !defined(__FreeBSD__) && !defined(_M_UNIX) && !defined(__linux__) && !defined(__dos__)
  ACCESS_STACK;
#endif

  /*
   * If the mode is O_NDELAY perform a Non Blocking accept & return 
   * immediately.
   */
  if ((mode = fcntl(s, F_GETFL, 0)) & (O_NDELAY | O_NONBLOCK)) {
    return (ACCEPT(s, addr, addrlen));
  }

  /*
   * Else issue an asynchronous request
   */

  mode = fcntl(s, F_GETFL, 0);
  if (fcntl(s, F_SETFL, (mode | O_NDELAY | FASYNC)) < 0)
    return (-1);
  if (fcntl(s, F_SETOWN, getpid()) < 0) {
    fcntl (s, F_SETFL, mode);
    return(-1);
  }

  while (TRUE) {
#ifdef DEBUG
    printf("Try to accept\n");
#endif
    result = ACCEPT(s, addr, addrlen);
    if (result != -1) {
      if ((fcntl(result, F_SETFL, mode) != -1) &&
          (fcntl(s, F_SETFL, mode) != -1))
        return(result);
      return (-1);
    }
    if (errno != EWOULDBLOCK) {
      fcntl(s, F_SETFL, mode);
      return(-1);
    }
#ifdef DEBUG
    printf("Unsuccessfull\n");
#endif /* DEBUG */
    SET_KERNEL_FLAG; /* No preemption */
    p = mac_pthread_self();

    gwidth = MAX(gwidth, s+1);
    p->width = s+1;
    pthread_fds_zero(&(p->readfds), p->width);
    FD_SET(s, &(p->readfds));
    FD_SET(s, &greadfds);
    pthread_fds_zero(&(p->writefds), p->width);
    pthread_fds_zero(&(p->exceptfds), p->width);

    sigaddset(&p->sigwaitset, AIO_SIG);
    p->state &= ~T_RUNNING;
    p->state |= T_BLOCKED | T_INTR_POINT;
    if (sigismember(&p->pending, SIGCANCEL) &&
	!sigismember(&p->mask, SIGCANCEL))
      SIG_CLEAR_KERNEL_FLAG(TRUE);
    else {
      pthread_q_deq_head(&ready, PRIMARY_QUEUE);
      SIM_SYSCALL(TRUE);
      CLEAR_KERNEL_FLAG;
    }
#ifdef __linux__
    /*
     * There appears to be a problem (bug?) with Linux:
     * Under SunOS, a SIGIO/POLL reception followed by another
     * non-blocking call to accept() returns w/ a valid socket descriptor;
     * under Linux, a SIGIO/POLL reception followed by another
     * non-blocking call to accept() returns -1 and EWOULDBLOCK
     * as if the socket was not ready; no more signals follow and the
     * thread remains blocked, even though the connection becomes available.
     * The latter fact is observed when we add a 1 microsec. nanosleep
     * delay right here, i.e. after the signal but before the the
     * non-blocking call to accept(); in this case, accept returns w/
     * a valid socket descriptor. It seems that when the signal arrives,
     * the TCP data has been noticed but maybe has not been received in full.
     * This behavior only shows up for TCP between two nodes; it does not
     * show for TCP on a local node (via localhost loopback).
     * Instead of using a 1 microsec. timeout, we perform a blocking call
     * to accept after getting the signal since this is probably less
     * overhead, even though all threads (the entire process) will be
     * blocked till the accept completes.
     * Is this a bug in Linux since SunOS does not behave this way?
     * The Linux sources /usr/src/linux/net/ipv4/tcp_input.c:tcp_rcv()
     * have a call to sock_wake_async() but the work done afterwards
     * does not seem to affect accept(), as far as I can tell.
     * in /usr/src/linux/net/tcp.c:accept(), tcp_find_established() is called.
     * It checks for the receive queue if any entry has
     * p->sk->state == TCP_ESTABLISHED || p->sk->state >= TCP_FIN_WAIT1
     * and TCP_ESTABLISHED is set in tcp_ack() and tcp_rcv(),
     * in the latter case before the sock_wake_async().
     * Does anybody know an answer / kernel fix?
     *
     * fcntl(s, F_SETFL, mode);
     */
    /*
     * can't do blocking accept either since SIGALRM may have come in
     * or SIGIO is fake wakeup, e.g. would block entire process.
     * Hence, back to nanosleep w/ 1us. Bad!
     */
    {
      struct timespec rqtp, rmtp;

      rqtp.ts_sec = 0;
      rqtp.ts_nsec = 1000;
      nanosleep(&rqtp, &rmtp);
    }
#endif /* __linux__ */
  }
#else /* _M_UNIX */
  int result;
  register struct fdentry* entry;

  /*
   * Set mode to no blocking
   */

  if ((entry=acquire_fdlock(s))==0)
     return (NOTOK);

  while ((result = ACCEPT(s, addr, addrlen)) < 0)
   {
     switch(get_errno()) {
#if defined(ERESTART)
     case ERESTART:
#endif
     case EINTR:
          break;
     case EWOULDBLOCK:
#ifndef __linux__
    case EAGAIN:
#endif
          if (entry->blocking)
           {
            release_fdlock(s);
            if (fd_reader_wait(s, NULL) == -1)
             return (NOTOK);
            if ((entry=acquire_fdlock(s))==0)
             {
               return(NOTOK);
             }
            break;
           }
      default:
          release_fdlock(s);
          return (NOTOK);
     }
   }
  if (result != -1 && entry->blocking)
   {
     if (fcntl(result, F_SETFL, entry->flags)==-1)
      {
        close(result);
        release_fdlock(s);
        return -1;
      }
   }
  release_fdlock(s);
  return (result);
#endif
}

/*------------------------------------------------------------*/
/*
 * recvfrom - Same as POSIX.1 recvfrom except that it blocks only
 * the current thread rather than entire process.
 */
int recvfrom(s, buf, len, flags, from, fromlen)
#if defined(__linux__) && (defined(__SOCKADDR_ARG) || defined(__SOCKADDR_ALLTYPES))
     int s;
     void *buf;
     size_t len;
     int flags;
     __SOCKADDR_ARG from;
     socklen_t *fromlen;
#else /* !__linux__ */
     int s;
     char *buf;
     int len;
     int flags;
     struct sockaddr *from;
     int *fromlen;
#endif /* !__linux__ */
{
  int mode;
  pthread_t p;
  int result;
#if defined(__linux__) && (defined(__SOCKADDR_ARG) || defined(__SOCKADDR_ALLTYPES))
  socklen_t fromlenparam;
#else /* !__linux__ */
  int fromlenparam;
#endif /* !__linux__ */
#ifdef DEBUG
  int retry = 0;
#endif

#if !defined(__FreeBSD__) && !defined(_M_UNIX) && !defined(__linux__) && !defined(__dos__)
  ACCESS_STACK;
#endif

  /*
   * If the mode is O_NDELAY perform a Non Blocking recvfrom & return 
   * immediately.
   */
  if ((mode = fcntl(s, F_GETFL, 0)) & (O_NDELAY | O_NONBLOCK)) {
    return (RECVFROM(s, buf, len, flags, from, fromlen));
  }

  /*
   * Else issue an asynchronous request for len.
   */
  mode = fcntl(s, F_GETFL, 0);
  if (fcntl(s, F_SETFL, (mode | O_NDELAY | FASYNC)) < 0)
    return (-1);
  if (fcntl(s, F_SETOWN, getpid()) < 0) {
    fcntl (s, F_SETFL, mode);
    return(-1);
  }

  while (TRUE) {
#ifdef DEBUG
    printf("Try to recvfrom\n");
#endif
    fromlenparam = *fromlen;
    result = RECVFROM(s, buf, len, flags, from, &fromlenparam);
    if (result != -1) {
      *fromlen = fromlenparam;
      if (fcntl(s, F_SETFL, mode) != -1)
        return(result);
      return (-1);
    }
    if (errno != EWOULDBLOCK) {
      fcntl(s, F_SETFL, mode);
      return(-1);
    }
#ifdef DEBUG
    retry++;
    if (retry > 1)
      printf("Recvfrom unsuccessfull retry %d\n", retry); fflush(stdout);
#endif /* DEBUG */
    SET_KERNEL_FLAG; /* No preemption */
    p = mac_pthread_self();

    gwidth = MAX(gwidth, s+1);
    p->width = s+1;
    pthread_fds_zero(&(p->readfds), p->width);
    FD_SET(s, &(p->readfds));
    FD_SET(s, &greadfds);
    pthread_fds_zero(&(p->writefds), p->width);
    pthread_fds_zero(&(p->exceptfds), p->width);

    sigaddset(&p->sigwaitset, AIO_SIG);
    p->state &= ~T_RUNNING;
    p->state |= T_BLOCKED | T_INTR_POINT;
    if (sigismember(&p->pending, SIGCANCEL) &&
	!sigismember(&p->mask, SIGCANCEL))
      SIG_CLEAR_KERNEL_FLAG(TRUE);
    else {
      pthread_q_deq_head(&ready, PRIMARY_QUEUE);
      SIM_SYSCALL(TRUE);
      CLEAR_KERNEL_FLAG;
    }
  }
}

/*------------------------------------------------------------*/
/*
 * sendto - Same as POSIX.1 sendto except that it blocks only
 * the current thread rather than entire process.
 */
int sendto(s, msg, len, flags, to, tolen)
#if defined(__linux__) && (defined(__SOCKADDR_ARG) || defined(__SOCKADDR_ALLTYPES))
     int s;
     const void *msg;
     size_t len;
     int flags;
     __CONST_SOCKADDR_ARG to;
     socklen_t tolen;
#else /* !__linux__ */
     int s;
     char *msg;
     int len;
     int flags;
     struct sockaddr *to;
     int tolen;
#endif /* !__linux__ */
{
  int mode;
  pthread_t p;
  int result;

#if !defined(__FreeBSD__) && !defined(_M_UNIX) && !defined(__linux__) && !defined(__dos__)
  ACCESS_STACK;
#endif

  /*
   * If the mode is O_NDELAY perform a Non Blocking recvfrom & return 
   * immediately.
   */
  if ((mode = fcntl(s, F_GETFL, 0)) & (O_NDELAY | O_NONBLOCK)) {
    return (SENDTO(s, msg, len, flags, to, tolen));
  }

  /*
   * Else issue an asynchronous request.
   */
  mode = fcntl(s, F_GETFL, 0);
  if (fcntl(s, F_SETFL, (mode | O_NDELAY | FASYNC)) < 0)
    return (-1);
  if (fcntl(s, F_SETOWN, getpid()) < 0) {
    fcntl (s, F_SETFL, mode);
    return(-1);
  }

  while (TRUE) {
#ifdef DEBUG
    printf("Try to sendto\n");
#endif
    result = SENDTO(s, msg, len, flags, to, tolen);
    if (result != -1) {
      if (fcntl(s, F_SETFL, mode) != -1)
        return(result);
      return (-1);
    }
    if (errno != EWOULDBLOCK) {
      fcntl(s, F_SETFL, mode);
      return(-1);
    }
#ifdef DEBUG
    printf("Unsuccessfull\n");
#endif /* DEBUG */
    SET_KERNEL_FLAG; /* No preemption */
    p = mac_pthread_self();

    gwidth = MAX(gwidth, s+1);
    p->width = s+1;
    pthread_fds_zero(&(p->readfds), p->width);
    pthread_fds_zero(&(p->writefds), p->width);
    FD_SET(s, &(p->writefds));
    FD_SET(s, &gwritefds);
    pthread_fds_zero(&(p->exceptfds), p->width);

    sigaddset(&p->sigwaitset, AIO_SIG);
    p->state &= ~T_RUNNING;
    p->state |= T_BLOCKED | T_INTR_POINT;
    if (sigismember(&p->pending, SIGCANCEL) &&
	!sigismember(&p->mask, SIGCANCEL))
      SIG_CLEAR_KERNEL_FLAG(TRUE);
    else {
      pthread_q_deq_head(&ready, PRIMARY_QUEUE);
      SIM_SYSCALL(TRUE);
      CLEAR_KERNEL_FLAG;
    }
  }
}

#endif /* IO && !__FreeBSD__ && !SVR4 */

#if defined(_M_UNIX)
/*
 * connect - 
 */
int connect(s, from, namelen)
   int s;
   const struct sockaddr* from;
   int     namelen;

{
  int ret;
  register struct fdentry* entry;

  if ((entry=acquire_fdlock(s))==0)
   {
     return(NOTOK);
   }

  while ((ret = CONNECT(s, from, namelen)) < OK) {
    switch(get_errno()) {
#ifdef ERESTART
    case ERESTART:
#endif
    case EINTR:
         break;

    case EALREADY:
    case EWOULDBLOCK:
#ifndef __linux__
    case EAGAIN:
#endif
    case EINPROGRESS:
         if (entry->blocking)
          {
            int tmpnamelen;
            struct sockaddr tmpname;

            release_fdlock(s);
	    if (fd_writer_wait(s, NULL)==-1)
             return (NOTOK);
	    tmpnamelen = sizeof(tmpname);
	    /* OK now lets see if it really worked */
	    if (((ret = GETPEERNAME(s, &tmpname, &tmpnamelen)) < 0)
	    &&  (errno == ENOTCONN))
             {
	       tmpnamelen = sizeof(ret);

	       /* Get the error, this function should not fail */
	       GETSOCKOPT(s, SOL_SOCKET, SO_ERROR, &ret, &tmpnamelen); 
	       errno = ret;
	       return (NOTOK);
	     }
           else
            {
               return ret;
            }
          }
    default:
         release_fdlock(s);
         return (NOTOK);
    }
  }      
  release_fdlock(s);
  return(ret);
}
#endif /* _M_UNIX */

/*------------------------------------------------------------*/
#if defined(STACK_CHECK) && defined(SIGNAL_STACK)
/*
 * pthread_io_end - dummy function used to do pc mapping to check
 *                  stack_overflow.
 */
void pthread_io_end()
{
  return;
}
/*------------------------------------------------------------*/
#endif /* STACK_CHECK && SIGNAL_STACK */

#endif /* IO */
