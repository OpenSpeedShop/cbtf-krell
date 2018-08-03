/*******************************************************************************
** Copyright (c) 2011-18 The Krell Institute. All Rights Reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Messages/Mem_data.h"
#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Time.h"


#if !defined(CBTF_SERVICE_USE_OFFLINE)
#include <unistd.h>
#include <fcntl.h>
#endif

#include <dlfcn.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <stdlib.h>

extern bool_t mem_do_trace(const char* traced_func);
extern void mem_start_event(CBTF_memt_event* event);
extern void mem_record_event(const CBTF_memt_event* event, uint64_t function);

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
#if 0
static bool is_initializing;

static void* (*f_malloc)(size_t);
static void* (*f_calloc)(size_t, size_t);
static void* (*f_realloc)(void*, size_t);
static void* (*f_free)(void*);
static int (*f_posix_memalign)(void **, size_t, size_t);
static int (*f_memalign)(size_t, size_t);

static void mem_f_initialize()
{
    f_malloc = dlsym(RTLD_NEXT, "malloc");
    f_calloc = dlsym(RTLD_NEXT, "calloc");
    f_realloc = dlsym(RTLD_NEXT, "realloc");
    f_free = dlsym(RTLD_NEXT, "free");
    f_posix_memalign = dlsym(RTLD_NEXT, "posix_memalign");
    f_memalign = dlsym(RTLD_NEXT, "memalign");
}
#endif
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void* __real_malloc(size_t);
void* __real_calloc(size_t, size_t);
void* __real_realloc(void*, size_t);
void* __real_free(void*);
int __real_posix_memalign(void **, size_t, size_t);
int __real_memalign(size_t, size_t);
#endif

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
#ifdef malloc
#undef malloc
#endif
void* malloc(size_t size) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void* __wrap_malloc(size_t size)
#else
void* memmalloc(size_t size)
#endif
{
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
#endif

    void* retval;
    CBTF_memt_event event;

    bool_t dotrace = mem_do_trace("malloc");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
	event.mem_type = CBTF_MEM_MALLOC;
    }

    /* Call the real MEM function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_malloc(size);
#else
    static void* (*f_malloc)(size_t) = NULL;
    if (!f_malloc)
	f_malloc = dlsym(RTLD_NEXT, "malloc");
    retval = f_malloc(size);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
	event.ptr = 0;
	event.size1 = size;
	event.size2 = size;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_malloc);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction(f_malloc));
#endif
    }
    
    /* Return the real MEM function's return value to the caller */
    return retval;
}


/* NOTE: The openmpi libraries call calloc before we are setup and
 * calloc calls fail.  In addition, using dlsym can also cause a crash.
 * The temporary solution for the dynamic case is to rely on __libc_calloc
 * as the real dynamic libc calloc call.
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
#ifdef calloc
#undef calloc
#endif
void* calloc(size_t count, size_t size) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void* __wrap_calloc(size_t count, size_t size)
#else
void* memcalloc(size_t count, size_t size)
#endif
{
    void* retval;
    CBTF_memt_event event;

    bool_t dotrace = mem_do_trace("calloc");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
	event.mem_type = CBTF_MEM_CALLOC;
    }

    /* Call the real MEM function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_calloc(count,size);
#else
    extern void *__libc_calloc(size_t, size_t);
    retval = __libc_calloc(count,size);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
	event.ptr = 0;
	event.size1 = count;
	event.size2 = size;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_calloc);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction(__libc_calloc));
#endif
    }
    
    /* Return the real MEM function's return value to the caller */
    return retval;
}


#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
#ifdef realloc
#undef realloc
#endif
void* realloc(void* oldPtr, size_t size) 
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void* __wrap_realloc(void* oldPtr, size_t size)
#else
void* memrealloc(void* oldPtr, size_t size)
#endif
{
    static void* (*f_realloc)(void*, size_t);
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
    if (f_realloc == NULL) {
	f_realloc = dlsym (RTLD_NEXT, "realloc");
    }
    if (f_realloc == NULL)
	return NULL;
#endif

    void* retval;
    CBTF_memt_event event;

    bool_t dotrace = mem_do_trace("realloc");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
	event.mem_type = CBTF_MEM_REALLOC;
    }

    /* Call the real MEM function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_realloc(oldPtr,size);
#else
    retval = f_realloc(oldPtr,size);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
	event.ptr = (uint64_t)oldPtr;
	event.size1 = size;
	event.size2 = size;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_realloc);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction(f_realloc));
#endif
    }
    
    /* Return the real MEM function's return value to the caller */
    return retval;
}


#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int posix_memalign(void ** memptr, size_t alignment, size_t size)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_posix_memalign(void ** memptr, size_t alignment, size_t size)
#else
int memposix_memalign(void ** memptr, size_t alignment, size_t size)
#endif
{    
    static int (*f_posix_memalign)(void **, size_t, size_t);
    int retval;
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
    if (f_posix_memalign == NULL) {
	f_posix_memalign = dlsym (RTLD_NEXT, "posix_memalign");
    }
    if (f_posix_memalign == NULL)
	return 0;
#endif

    CBTF_memt_event event;

    bool_t dotrace = mem_do_trace("posix_memalign");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
	event.mem_type = CBTF_MEM_POSIX_MEMALIGN;
    }

    /* Call the real MEM function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_posix_memalign(memptr,alignment,size);
#else
    retval = f_posix_memalign(memptr,alignment,size);

#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
	event.ptr = (uint64_t)(*memptr);
	event.size1 = alignment;
	event.size2 = size;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_posix_memalign);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction(f_posix_memalign));
#endif
    }
    

    /* Return the real MEM function's return value to the caller */
    return retval;
}



#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int memalign(size_t blocksize, size_t bytes)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_memalign(size_t blocksize, size_t bytes)
#else
int memmemalign(size_t blocksize, size_t bytes)
#endif
{    
    int retval;
    static int (*f_memalign)(size_t, size_t);
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
    if (f_memalign == NULL) {
	f_memalign = dlsym (RTLD_NEXT, "memalign");
    }
    if (f_memalign == NULL)
	return 0;
#endif
    CBTF_memt_event event;

    bool_t dotrace = mem_do_trace("memalign");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
	event.mem_type = CBTF_MEM_MEMALIGN;
    }

    /* Call the real MEM function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_memalign(blocksize,bytes);
#else
    retval = f_memalign(blocksize,bytes);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
	event.ptr = 0;
	event.size1 = blocksize;
	event.size2 = bytes;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_memalign);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction(f_memalign));
#endif
    }
    

    /* Return the real MEM function's return value to the caller */
    return retval;
}



#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
#ifdef free
#undef free
#endif
void free(void * ptr)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_free(void * ptr)
#else
void memfree(void * ptr)
#endif
{
    static void* (*f_free)(void*);
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
    if (f_free == NULL) {
	f_free = dlsym (RTLD_NEXT, "free");
    }
    if (f_free == NULL)
	return;
#endif
    CBTF_memt_event event;

    bool_t dotrace = mem_do_trace("free");

    /* when ptr is NULL free is a no-op. We could record these if desired
     * but the cost is high.  Only reason to record is to pinpoint the
     * locations where code is calling free with a NULL ptr.
     */
    if (ptr == NULL) dotrace = FALSE;

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
	event.mem_type = CBTF_MEM_FREE;
    }

    /* Call the real MEM function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    __real_free(ptr);
#else
    f_free(ptr);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = 0;
	event.ptr = (uint64_t)ptr;
	event.size1 = 0;
	event.size2 = 0;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_free);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction(f_free));
#endif
    }
    
}


