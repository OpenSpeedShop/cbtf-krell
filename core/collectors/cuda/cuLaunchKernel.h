/*******************************************************************************
** Copyright (c) 2012 Argo Navis Technologies. All Rights Reserved.
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

/**
 * @file
 *
 * Callback functions invoked by cupti_callback() every time the CUDA function:
 *
 *     cuLaunchKernel
 *
 * is entered or exited. This is not a traditional header file in that it is not
 * intended for use <em>anywhere</em> except in "collector.h". It exists purely
 * to help manage the size and repetiveness of these callback functions.
 */



/**
 * Entry callback for cuLaunchKernel().
 */
inline void cuLaunchKernel_enter_callback(
    const cuLaunchKernel_params* const params
    )
{
#if !defined(NDEBUG)
    if (debug)
    {
        printf("[CBTF/CUDA] cuLaunchKernel_enter_callback()\n");
    }
#endif

    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* ... */
}



/**
 * Exit callback for cuLaunchKernel().
 */
inline void cuLaunchKernel_exit_callback(
    const cuLaunchKernel_params* const params
    )
{
#if !defined(NDEBUG)
    if (debug)
    {
        printf("[CBTF/CUDA] cuLaunchKernel_exit_callback()\n");
    }
#endif

    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    
    /* ... */
}
