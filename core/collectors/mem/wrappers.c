/*******************************************************************************
** Copyright (c) 2011 The Krell Institute. All Rights Reserved.
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

#include "KrellInstitute/Messages/Mem_data.h"

#include <dlfcn.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>

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
    void* retval;
    CBTF_mem_event event;

    bool_t dotrace = mem_do_trace("malloc");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
    }

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_malloc(size);
#else
    void* (*realfunc)() = dlsym (RTLD_NEXT, "malloc");
    retval = (*realfunc)(size);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_malloc);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    return retval;
}

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
    CBTF_mem_event event;

    bool_t dotrace = mem_do_trace("calloc");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
    }

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_calloc(count,size);
#else
    //void* (*realfunc)() = dlsym (RTLD_NEXT, "calloc");
    //retval = (*realfunc)(count,size);
    retval = __libc_calloc(count,size);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_calloc);
#else
        //mem_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
        mem_record_event(&event, CBTF_GetAddressOfFunction((__libc_calloc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
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
    void* retval;
    CBTF_mem_event event;

    bool_t dotrace = mem_do_trace("realloc");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
    }

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_realloc(oldPtr,size);
#else
    void* (*realfunc)() = dlsym (RTLD_NEXT, "realloc");
    retval = (*realfunc)(oldPtr,size);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_realloc);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
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
    int retval;
    CBTF_mem_event event;

    bool_t dotrace = mem_do_trace("posix_memalign");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
    }

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_posix_memalign(memptr,alignment,size);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "posix_memalign");
    retval = (*realfunc)(memptr,alignment,size);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_posix_memalign);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    

    /* Return the real IO function's return value to the caller */
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
    CBTF_mem_event event;

    bool_t dotrace = mem_do_trace("memalign");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
    }

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_posix_memalign(blocksize,bytes);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "memalign");
    retval = (*realfunc)(blocksize,bytes);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __realmemalign);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    

    /* Return the real IO function's return value to the caller */
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
    //void retval;
    CBTF_mem_event event;

    bool_t dotrace = mem_do_trace("free");

    if (dotrace) {
        mem_start_event(&event);
        event.start_time = CBTF_GetTime();
    }

    /* Call the real IO function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    __real_free(ptr);
#else
    void (*realfunc)() = dlsym (RTLD_NEXT, "free");
    (*realfunc)(ptr);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        mem_record_event(&event, (uint64_t) __real_free);
#else
        mem_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real IO function's return value to the caller */
    //return retval;
}
