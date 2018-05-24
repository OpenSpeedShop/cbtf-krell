/*******************************************************************************
** Copyright (c) 2017 The Krell Institute. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#define _GNU_SOURCE
#ifndef __USE_GNU 
#define __USE_GNU /* XXX for RTLD_NEXT on Linux */ 
#endif /* !__USE_GNU */
#define _XOPEN_SOURCE 500 /* for readlink */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Messages/IO_data.h"
#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Time.h"


#if !defined(CBTF_SERVICE_USE_OFFLINE)
#include <syscall.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include <dlfcn.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>

#include "overviewTLS.h"

/* Start part 2 of 2 for Hack to get around inconsistent syscall definitions */
#include <sys/syscall.h>
#ifdef __NR_pread64  /* Newer kernels renamed but it's the same.  */
# ifndef __NR_pread
# define __NR_pread __NR_pread64
# endif
#endif

#ifdef __NR_pwrite64  /* Newer kernels renamed but it's the same.  */
# ifndef __NR_pwrite
#  define __NR_pwrite __NR_pwrite64
# endif
#endif
/* End part 2 of 2 for Hack to get around inconsistent syscall definitions */

/*
 * IO Wrapper Functions
 *
 */

/*
The following IO SYS calls are traced by the Overview Collector.
These calls are traced using their weak names
(e.g. "open" rather than "__libc_open").

see /usr/include/bits/syscall.h for details.
SYS_pread;
SYS_read;
SYS_readv;
SYS_pwrite;
SYS_write;
SYS_writev;
SYS_lseek;
SYS_creat;
SYS_open;
SYS_close;
SYS_dup;
SYS_dup2;
SYS_pipe;
SYS_open64;
SYS_creat64;
SYS_pread64;
SYS_pwrite64;
SYS_lseek64;
*/


#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
#ifdef read
#undef read
#endif
ssize_t read(int fd, void *buf, size_t count) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
ssize_t __wrap_read(int fd, void *buf, size_t count) 
#else
ssize_t ioread(int fd, void *buf, size_t count) 
#endif
{
    ssize_t retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_read(fd, buf, count);
#else
    static ssize_t (*f_read) (int, void *, size_t) = NULL;
    char *error;
    dlerror();    /* Clear any existing error */
    if (!f_read) {
      *(void **) (&f_read) = dlsym(RTLD_NEXT, "read");

      if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
      }
    }
    retval = (*f_read)(fd, buf, count);
#endif


    event.bytes = retval;
    event.kind = Read;
    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_read));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_read);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*f_read)));
#endif

    /* Return the real IO function's return value to the caller */
    return retval;
}

//#ifndef DEBUG
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
ssize_t write(int fd, __const void *buf, size_t count) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
ssize_t __wrap_write(int fd, __const void *buf, size_t count) 
#else
ssize_t iowrite(int fd, void *buf, size_t count) 
#endif
{    
    ssize_t retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_write(fd, buf, count);
#else
    //ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "write");
    ssize_t (*realfunc) (int, const void *, size_t);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "write");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(fd, buf, count);
#endif


    event.bytes = retval;
    event.kind = Write;
    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_write));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_write);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif

    /* Return the real IO function's return value to the caller */
    return retval;
}
//#endif

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
off_t lseek(int fd, off_t offset, int whence) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
off_t __wrap_lseek(int fd, off_t offset, int whence) 
#else
off_t iolseek(int fd, off_t offset, int whence) 
#endif
{
    off_t retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_lseek(fd, offset, whence);
#else
    off_t (*realfunc) (int, off_t, int);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "lseek");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(fd, offset, whence);
#endif


    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_lseek));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_lseek);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
__off64_t lseek64(int fd, __off64_t offset, int whence) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
__off64_t __wrap_lseek64(int fd, __off64_t offset, int whence) 
#else
off_t iolseek64(int fd, off_t offset, int whence) 
#endif
{
    off_t retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_lseek64(fd, offset, whence);
#else
    off_t (*realfunc) (int, off_t, int);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "lseek64");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(fd, offset, whence);
#endif


    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_lseek64));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_lseek64);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int open(const char *pathname, int flags, mode_t mode) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_open(const char *pathname, int flags, mode_t mode) 
#else
int ioopen(const char *pathname, int flags, mode_t mode) 
#endif
{
    int retval = 0;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_open(pathname, flags, mode);
#else
    int (*realfunc) (const char *, int, mode_t);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "open");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(pathname, flags, mode);
#endif


    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_open));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_open);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int open64(const char *pathname, int flags, mode_t mode) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_open64(const char *pathname, int flags, mode_t mode) 
#else
int ioopen64(const char *pathname, int flags, mode_t mode) 
#endif
{
    int retval = 0;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_open64(pathname, flags, mode);
#else
    int (*realfunc) (const char *, int, mode_t);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "open64");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(pathname, flags, mode);
#endif


    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_open64));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_open64);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int close(int fd) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_close(int fd) 
#else
int ioclose(int fd) 
#endif
{
    int retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_close(fd);
#else
    int (*realfunc) (int);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "close");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(fd);
#endif

    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_close));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_close);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int dup(int oldfd) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_dup(int oldfd) 
#else
int iodup(int oldfd) 
#endif
{
    int retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_dup(oldfd);
#else
    int (*realfunc) (int);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "dup");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(oldfd);
#endif


    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_dup));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_dup);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int dup2(int oldfd, int newfd) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_dup2(int oldfd, int newfd) 
#else
int iodup2(int oldfd, int newfd) 
#endif
{
    int retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_dup2(oldfd,newfd);
#else
    int (*realfunc) (int,int);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "dup2");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(oldfd,newfd);
#endif


    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_dup2));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_dup2);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int creat(char *pathname, mode_t mode) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_creat(char *pathname, mode_t mode) 
#else
int iocreat(char *pathname, mode_t mode) 
#endif
{
    int retval = 0;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_creat(pathname,mode);
#else
    int (*realfunc) (char *,mode_t);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "creat");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(pathname,mode);
#endif


    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_creat));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_creat);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int creat64(char *pathname, mode_t mode) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_creat64(char *pathname, mode_t mode) 
#else
int iocreat64(char *pathname, mode_t mode) 
#endif
{
    int retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_creat64(pathname,mode);
#else
    int (*realfunc) (char *,mode_t);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "creat64");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(pathname,mode);
#endif

    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_creat64));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_creat64);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    

    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pipe(int filedes[2]) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pipe(int filedes[2]) 
#else
int iopipe(int filedes[2]) 
#endif
{
    int retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pipe(filedes);
#else
    int (*realfunc) (int *);
    char *error;
    dlerror();    /* Clear any existing error */
    *(void **) (&realfunc) = dlsym(RTLD_NEXT, "pipe");
    if ((error = dlerror()) != NULL)  {
	fprintf(stderr, "%s\n", error);
    }
    retval = (*realfunc)(filedes);
#endif


    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_pipe));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_pipe);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
ssize_t pread(int fd, void *buf, size_t count, off_t offset) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
ssize_t __wrap_pread(int fd, void *buf, size_t count, off_t offset) 
#else
ssize_t iopread(int fd, void *buf, size_t count, off_t offset) 
#endif
{
    ssize_t retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pread(fd, buf, count, offset);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pread");
    retval = (*realfunc)(fd, buf, count, offset);
#endif


    event.bytes = retval;
    event.kind = Read;
    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_pread));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_pread);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
ssize_t pread64(int fd, void *buf, size_t count, __off64_t offset) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
ssize_t __wrap_pread64(int fd, void *buf, size_t count, __off64_t offset) 
#else
ssize_t iopread64(int fd, void *buf, size_t count, off_t offset) 
#endif
{
    ssize_t retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pread64(fd, buf, count, offset);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pread64");
    retval = (*realfunc)(fd, buf, count, offset);
#endif


    event.bytes = retval;
    event.kind = Read;
    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_pread64));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_pread64);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

#ifndef DEBUG
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
ssize_t pwrite(int fd, __const void *buf, size_t count, __off_t offset) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
ssize_t __wrap_pwrite(int fd, __const void *buf, size_t count, __off_t offset) 
#else
ssize_t iopwrite(int fd, void *buf, size_t count, off_t offset) 
#endif
{
    ssize_t retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pwrite(fd, buf, count, offset);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pwrite");
    retval = (*realfunc)(fd, buf, count, offset);
#endif


    event.bytes = retval;
    event.kind = Write;
    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_pwrite));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_pwrite);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif

#ifndef DEBUG
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
ssize_t pwrite64(int fd, __const void *buf, size_t count, __off64_t offset) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
ssize_t __wrap_pwrite64(int fd, __const void *buf, size_t count, __off64_t offset) 
#else
ssize_t iopwrite64(int fd, void *buf, size_t count, off_t offset) 
#endif
{
    ssize_t retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pwrite64(fd, buf, count, offset);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "pwrite64");
    retval = (*realfunc)(fd, buf, count, offset);
#endif


    event.bytes = retval;
    event.kind = Write;
    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_pwrite64));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_pwrite64);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
#endif

//#if !defined(CBTF_SERVICE_USE_OFFLINE)
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
ssize_t readv(int fd, const struct iovec *vector, int count) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
ssize_t __wrap_readv(int fd, const struct iovec *vector, int count) 
#else
ssize_t ioreadv(int fd, const struct iovec *vector, int count) 
#endif
{
    ssize_t retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_readv(fd, vector, count);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "readv");
    retval = (*realfunc)(fd, vector, count);
#endif


    event.bytes = retval;
    event.kind = Read;
    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_readv));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_readv);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}


//#ifndef DEBUG
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
ssize_t writev(int fd, const struct iovec *vector, int count) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
ssize_t __wrap_writev(int fd, const struct iovec *vector, int count) 
#else
ssize_t iowritev(int fd, const struct iovec *vector, int count) 
#endif
{
    ssize_t retval;
    CBTF_overview_iop_event event;
    uint64_t start_time = 0;

    TLS_start_io_event(&event);
    start_time = CBTF_GetTime();

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_writev(fd, vector, count);
#else
    ssize_t (*realfunc)() = dlsym (RTLD_NEXT, "writev");
    retval = (*realfunc)(fd, vector, count);
#endif


    event.bytes = retval;
    event.kind = Write;
    event.time = CBTF_GetTime() - start_time;

    /* Record event and it's stacktrace*/
#if defined(RUNTIME_PLATFORM_BGQ)
#if defined(HAVE_TARGET_SHARED) && ! defined (CBTF_SERVICE_BUILD_STATIC) 
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((const void *) __real_writev));
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    TLS_record_io_event(&event, (uint64_t) __real_writev);
#else
    TLS_record_io_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    
    /* Return the real IO function's return value to the caller */
    return retval;
}
//#endif
//#endif
