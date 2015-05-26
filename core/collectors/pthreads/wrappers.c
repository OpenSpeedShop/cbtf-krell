/*******************************************************************************
** Copyright (c) 2011-2015 The Krell Institute. All Rights Reserved.
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

#include "KrellInstitute/Messages/Pthreads_data.h"
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
#include <pthread.h>

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg)
#else
int pthreads_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_create");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_CREATE;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real POSIX thread function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_create(thread,attr,start_routine,arg);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_create");
    retval = (*realfunc)(thread,attr,start_routine,arg);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)thread;
        event.ptr2 = (uint64_t)attr;
        event.ptr3 = (uint64_t)*start_routine;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_create);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_mutex_init( pthread_mutex_t* mtx,
                       const pthread_mutexattr_t* attr)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_mutex_init( pthread_mutex_t* mtx,
                       const pthread_mutexattr_t* attr)
#else
int pthreads_pthread_mutex_init( pthread_mutex_t* mtx,
                       const pthread_mutexattr_t* attr)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_mutex_init");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_MUTEX_INIT;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real POSIX thread function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_mutex_init(mtx,attr);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_mutex_init");
    retval = (*realfunc)(mtx,attr);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)mtx;
        event.ptr2 = (uint64_t)attr;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_mutex_init);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_mutex_destroy( pthread_mutex_t* mtx)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_mutex_destroy( pthread_mutex_t* mtx)
#else
int pthreads_pthread_mutex_init( pthread_mutex_t* mtx)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_mutex_destroy");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_MUTEX_DESTROY;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_mutex_destroy(mtx);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_mutex_destroy");
    retval = (*realfunc)(mtx);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)mtx;
        event.ptr2 = (uint64_t)NULL;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_mutex_destroy);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_mutex_lock( pthread_mutex_t* mtx)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_mutex_lock( pthread_mutex_t* mtx)
#else
int pthreads_pthread_mutex_lock( pthread_mutex_t* mtx)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_mutex_lock");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_MUTEX_LOCK;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_mutex_lock(mtx);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_mutex_lock");
    retval = (*realfunc)(mtx);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)mtx;
        event.ptr2 = (uint64_t)NULL;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_mutex_lock);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_mutex_unlock( pthread_mutex_t* mtx)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_mutex_unlock( pthread_mutex_t* mtx)
#else
int pthreads_pthread_mutex_unlock( pthread_mutex_t* mtx)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_mutex_unlock");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_MUTEX_UNLOCK;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_mutex_unlock(mtx);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_mutex_unlock");
    retval = (*realfunc)(mtx);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)mtx;
        event.ptr2 = (uint64_t)NULL;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_mutex_unlock);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_mutex_trylock( pthread_mutex_t* mtx)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_mutex_trylock( pthread_mutex_t* mtx)
#else
int pthreads_pthread_mutex_trylock( pthread_mutex_t* mtx)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_mutex_trylock");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_MUTEX_TRYLOCK;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real function */
    // if lock is held by any thread call returns immediately else
    // the lock is aquired by calling thread.
    // See manpage for details on how the calling threads mutex
    // attributes affect this call.
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_mutex_trylock(mtx);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_mutex_trylock");
    retval = (*realfunc)(mtx);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)mtx;
        event.ptr2 = (uint64_t)NULL;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_mutex_trylock);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_cond_init(pthread_cond_t* cnd, const pthread_condattr_t* attr)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_cond_init(pthread_cond_t* cnd, const pthread_condattr_t* attr)
#else
int pthreads_pthread_cond_init(pthread_cond_t* cnd, const pthread_condattr_t* attr)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_cond_init");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_COND_INIT;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_cond_init(cnd,attr);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_cond_init");
    retval = (*realfunc)(cnd,attr);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)cnd;
        event.ptr2 = (uint64_t)attr;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_cond_init);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_cond_destroy(pthread_cond_t* cnd)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_cond_destroy(pthread_cond_t* cnd)
#else
int pthreads_pthread_cond_destroy(pthread_cond_t* cnd)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_cond_destroy");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_COND_DESTROY;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_cond_destroy(cnd);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_cond_destroy");
    retval = (*realfunc)(cnd);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)cnd;
        event.ptr2 = (uint64_t)NULL;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_cond_destroy);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_cond_signal(pthread_cond_t* cnd)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_cond_signal(pthread_cond_t* cnd)
#else
int pthreads_pthread_cond_signal(pthread_cond_t* cnd)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_cond_signal");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_COND_SIGNAL;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_cond_signal(cnd);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_cond_signal");
    retval = (*realfunc)(cnd);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)cnd;
        event.ptr2 = (uint64_t)NULL;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_cond_signal);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_cond_broadcast(pthread_cond_t* cnd)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_cond_broadcast(pthread_cond_t* cnd)
#else
int pthreads_pthread_cond_broadcast(pthread_cond_t* cnd)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_cond_broadcast");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_COND_BROADCAST;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real function */
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_cond_broadcast(cnd);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_cond_broadcast");
    retval = (*realfunc)(cnd);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)cnd;
        event.ptr2 = (uint64_t)NULL;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_cond_broadcast);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_cond_wait(pthread_cond_t* cnd, pthread_mutex_t* mtx)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_cond_wait(pthread_cond_t* cnd, pthread_mutex_t* mtx)
#else
int pthreads_pthread_cond_wait(pthread_cond_t* cnd, pthread_mutex_t* mtx)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_cond_wait");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_COND_WAIT;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real function */
    // There is an implied unlock and then lock of the mutex.
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_cond_wait(cnd,mtx);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_cond_wait");
    retval = (*realfunc)(cnd,mtx);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)cnd;
        event.ptr2 = (uint64_t)mtx;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_cond_wait);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_SERVICE_BUILD_STATIC)
int pthread_cond_timedwait(pthread_cond_t* cnd, pthread_mutex_t* mtx,
			   const struct timespec* tspec)
#elif defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_pthread_cond_timedwait(pthread_cond_t* cnd, pthread_mutex_t* mtx,
			   const struct timespec* tspec)
#else
int pthreads_pthread_cond_timedwait(pthread_cond_t* cnd, pthread_mutex_t* mtx,
			   const struct timespec* tspec)
#endif
{
    int retval,eval;
    CBTF_pthreadt_event event;

    bool_t dotrace = pthreads_do_trace("pthread_cond_timedwait");

    if (dotrace) {
        eval = pthreads_start_event(&event);
	if (eval) {
        event.start_time = CBTF_GetTime();
        event.pthread_type = CBTF_PTHREAD_COND_TIMEDWAIT;
	} else {
	    dotrace = FALSE;
	}
    }

    /* Call the real function */
    // There is an implied unlock and then lock of the mutex.
    // the return val will be ETIMEDOUT if condition is not
    // met in the time specified by tspec.
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
    retval = __real_pthread_cond_timedwait(cnd,mtx,tspec);
#else
    int (*realfunc)() = dlsym (RTLD_NEXT, "pthread_cond_timedwait");
    retval = (*realfunc)(cnd,mtx,tspec);
#endif


    if (dotrace) {
        event.stop_time = CBTF_GetTime();
	event.retval = (uint64_t)retval;
        event.ptr1 = (uint64_t)cnd;
        event.ptr2 = (uint64_t)mtx;
        event.ptr3 = (uint64_t)NULL;

    /* Record event and it's stacktrace*/
#if defined (CBTF_SERVICE_BUILD_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
        pthreads_record_event(&event, (uint64_t) __real_pthread_cond_timedwait);
#else
        pthreads_record_event(&event, CBTF_GetAddressOfFunction((*realfunc)));
#endif
    }
    
    /* Return the real function's return value to the caller */
    return retval;
}
