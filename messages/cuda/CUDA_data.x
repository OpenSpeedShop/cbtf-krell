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

/** @file Specification of the CUDA collector's blobs. */

%#include "KrellInstitute/Messages/Address.h"
%#include "KrellInstitute/Messages/File.h"
%#include "KrellInstitute/Messages/Thread.h"
%#include "KrellInstitute/Messages/Time.h"



/**
 * Enumeration of the different request types enqueue by the CUDA driver.
 */
enum CUDA_RequestTypes
{
    LaunchKernel = 0,
    MemoryCopy = 1,
    MemorySet = 2
};

/**
 * Message emitted when the CUDA driver enqueues a request.
 */
struct CUDA_EnqueueRequest
{
    /** Type of request that was enqueued. */
    CUDA_RequestTypes type;

    /** Time at which the request was enqueued. */
    CBTF_Protocol_Time time;

    /** CUDA stream for which the request was enqueued. */
    uint32_t stream;

    /**
     * Call site of the request. This is an index into the stack_traces array
     * of the CBTF_cuda_data containing this message.
     */
    uint32_t call_site;
};



/**
 * Message emitted when the CUDA driver loads a module.
 */
struct CUDA_LoadedModule
{
    /** Time at which the module was unloaded. */
    CBTF_Protocol_Time time;

    /** Name of the file containing the module that was loaded. */
    CBTF_Protocol_FileName module;
    
    /** Handle within the CUDA driver of the loaded module .*/
    CBTF_Protocol_Address handle;
};



/**
 * Message emitted when the CUDA driver resolves a function.
 */
struct CUDA_ResolvedFunction
{
    /** Time at which the function was resolved. */
    CBTF_Protocol_Time time;

    /** Handle within the CUDA driver of the module containing the function. */
    CBTF_Protocol_Address module_handle;
    
    /** Name of the function being resolved. */
    string function<>;
    
    /** Handle within the CUDA Drvier of the resolved function. */
    CBTF_Protocol_Address handle;    
};



/**
 * Message emitted when the CUDA driver unloads a module.
 */
struct CUDA_UnloadedModule
{
    /** Time at which the module was unloaded. */
    CBTF_Protocol_Time time;

    /** Handle within the CUDA driver of the unloaded module .*/
    CBTF_Protocol_Address handle;
};



/**
 * Enumeration of the different types of messages that are encapsulated within
 * this collector's blobs. See the note on CBTF_cuda_data for more information.
 */
enum CUDA_MessageTypes
{
    EnqueueRequest = 0,
    LoadedModule = 1,
    ResolvedFunction = 2,
    UnloadedModule = 3
};



/**
 * Union of the different types of messages that are encapsulated within this
 * collector's blobs. See the note on CBTF_cuda_data for more information.
 */
union CBTF_cuda_message switch (unsigned type)
{
    case   EnqueueRequest:   CUDA_EnqueueRequest enqueue_request;
    case     LoadedModule:     CUDA_LoadedModule loaded_module;
    case ResolvedFunction: CUDA_ResolvedFunction resolved_function;
    case   UnloadedModule:   CUDA_UnloadedModule unloaded_module;

    default: void;
};



/**
 * Structure of the blob containing our performance data.
 *
 * @note    The CUDA driver contains a separate loader that is used to load
 *          binary code modules onto the GPU, resolve symbols, and invoke
 *          functions (kernels). Additionally, there are multiple types of
 *          performance data generated by this collector. These issues lead
 *          to this collector's performance data blobs being significantly
 *          more complex than those of the typical collector. To facilitate
 *          maximum reuse of existing collector infrastructure, all of the
 *          different data generated by this collector is "packed" into one
 *          performance data blob type using a XDR discriminated union.
 *
 * @sa http://en.wikipedia.org/wiki/Tagged_union
 */
struct CBTF_cuda_data
{
    /** Individual messages containing data gathered by this collector. */
    CBTF_cuda_message messages<>;

    /**
     * Unique, null-terminated, stack traces referenced by the messages.
     *
     * @note    Because calls to the CUDA driver typically occur from a limited
     *          set of call sites, grouping them together in this way can result
     *          in significant data compression.
     */
    CBTF_Protocol_Address stack_traces<>;
};