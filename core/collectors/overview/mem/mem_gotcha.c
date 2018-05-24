/*******************************************************************************
** Copyright (c) 2018 The Krell Institute. All Rights Reserved.
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
#include <stdbool.h>
#include <stdlib.h>
#include <gotcha/gotcha.h>

/* these externs record the values from the wrappers
 * when do_trace is true.
 */
extern bool collector_do_trace();
extern void TLS_start_mem_event(CBTF_memt_event* event);
extern void TLS_record_mem_event(const CBTF_memt_event* event, uint64_t function);

/* original call pointers */
static void* (*wrappee_malloc)(size_t);
static void* (*wrappee_calloc)(size_t, size_t);
static void* (*wrappee_realloc)(void*, size_t);
static void* (*wrappee_free)(void*);
static int (*wrappee_posix_memalign)(void **, size_t, size_t);
static int (*wrappee_memalign)(size_t, size_t);

/* the wrappers */
static void* malloc_wrapper(size_t);
static void* calloc_wrapper(size_t, size_t);
static void* realloc_wrapper(void*, size_t);
static void* free_wrapper(void*);
static int posix_memalign_wrapper(void **, size_t, size_t);
static int memalign_wrapper(size_t, size_t);

struct gotcha_binding_t wrap_actions [] = {
    { "malloc", malloc_wrapper, &wrappee_malloc },
    { "calloc", calloc_wrapper, &wrappee_calloc },
    { "realloc", realloc_wrapper, &wrappee_realloc },
    { "free", free_wrapper, &wrappee_free },
    { "posix_memalign", posix_memalign_wrapper, &wrappee_posix_memalign },
    { "memalign", memalign_wrapper, &wrappee_memalign }
};

void init_mem_wrappers()
{
    gotcha_wrap(wrap_actions, sizeof(wrap_actions)/sizeof(struct gotcha_binding_t), "mem_overview");
}

static void* malloc_wrapper(size_t size)
{
    void* retval;
    CBTF_memt_event event;

    bool dotrace = collector_do_trace();
    //bool dotrace = true;

    if (dotrace) {
        TLS_start_mem_event(&event);
        event.start_time = CBTF_GetTime();
        event.mem_type = CBTF_MEM_MALLOC;
    }
    retval =  wrappee_malloc(size);
    if (dotrace) {
        event.stop_time = CBTF_GetTime();
        event.retval = (uint64_t)retval;
        event.ptr = 0;
        event.size1 = size;
        event.size2 = size;

        TLS_record_mem_event(&event, CBTF_GetAddressOfFunction(malloc));
    }
    return retval;
}

static void* calloc_wrapper(size_t count, size_t size)
{
    return wrappee_calloc(count,size);
}

static void* realloc_wrapper(void* oldPtr, size_t size)
{
    return wrappee_realloc(oldPtr,size);
}

static void* free_wrapper(void* ptr)
{
    return wrappee_free(ptr);
}

static int posix_memalign_wrapper(void ** memptr, size_t alignment, size_t size)
{
    return wrappee_posix_memalign(memptr,alignment,size);
}

static int memalign_wrapper(size_t blocksize, size_t bytes)
{
    return wrappee_memalign(blocksize,bytes);
}
