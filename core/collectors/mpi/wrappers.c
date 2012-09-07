/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2008-2012 The Krell Institute. All Rights Reserved.
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

/** @file
 *
 * MPI function wrappers for the MPI tracing collector.
 * NOTE: This file only differs from the mpit wrappers.c
 * by the extra details mpit records. We are no longer
 * using the mkwrapper tool to generate the mpi wrappers.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Messages/Mpi_data.h"
#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Time.h"

#include <mpi.h>

#if defined (CBTF_SERVICE_USE_OFFLINE)
//extern int CBTF_mpi_rank;
int CBTF_mpi_rank;
#endif

static int debug_trace = 0;

/*
 * MPI_Irecv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Irecv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Irecv
#else
int mpi_PMPI_Irecv
#endif
		    (void* buf, int count, MPI_Datatype datatype, int source, 
		    int tag, MPI_Comm comm, MPI_Request* request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Irecv");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.source = source;

    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);

    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;

    /* Initialize unused arguments */
    event.destination = -1;
#endif


    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Irecv));

    }

    return retval;
}

/*
 * MPI_Recv
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Recv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Recv
#else
int mpi_PMPI_Recv
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int source, 
    int tag, 
    MPI_Comm comm, 
    MPI_Status *status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Recv");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Recv(buf, count, datatype, source,  tag, comm, status);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.destination = -1;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Recv));

    }

    return retval;
}
  

/*
 * MPI_Recv_init
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Recv_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Recv_init
#else
int mpi_PMPI_Recv_init
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int source, 
    int tag, 
    MPI_Comm comm, 
    MPI_Request *request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Recv_init");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Recv_init(buf, count, datatype, source,  tag, comm, request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.destination = -1;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Recv_init));

    }

    return retval;
}
  
/*
 * MPI_Iprobe
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Iprobe
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Iprobe
#else
int mpi_PMPI_Iprobe
#endif
    (int source, 
    int tag, 
    MPI_Comm comm, 
    int *flag, 
    MPI_Status *status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Iprobe");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Iprobe(source, tag, comm, flag, status);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.retval = retval;

    /* Initialize unused arguments */
    event.destination = -1;
    event.datatype = 0;
    event.size = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Iprobe));

    }

    return retval;
}
  
/*
 * MPI_probe
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Probe
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Probe
#else
int mpi_PMPI_Probe
#endif
    (int source, 
    int tag, 
    MPI_Comm comm, 
    MPI_Status *status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Probe");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Probe(source, tag, comm, status);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.retval = retval;

    /* Initialize unused arguments */
    event.destination = -1;
    event.datatype = 0;
    event.size = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Probe));

    }

    return retval;
}
  


/*
 * MPI_Isend
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Isend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Isend
#else
int mpi_PMPI_Isend
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest,
    int tag, 
    MPI_Comm comm, 
    MPI_Request* request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Isend");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    event.destination = dest;
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Isend));

    }

    return retval;
}

/*
 * MPI_Bsend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Bsend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Bsend
#else
int mpi_PMPI_Bsend
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Bsend");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Bsend(buf, count, datatype, dest, tag, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Bsend));

    }

    return retval;
}
  

/*
 * MPI_Bsend_init
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Bsend_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Bsend_init
#else
int mpi_PMPI_Bsend_init
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm,
    MPI_Request* request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("Bsend_init");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Bsend_init));

    }

    return retval;
}
  
/*
 * MPI_Ibsend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Ibsend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Ibsend
#else
int mpi_PMPI_Ibsend
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm, 
    MPI_Request *request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Ibsend");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Ibsend(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Ibsend));

    }

    return retval;
}
  
/*
 * MPI_Irsend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Irsend    
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Irsend
#else
int mpi_PMPI_Irsend
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm, 
    MPI_Request *request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Irsend");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Irsend(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Irsend));

    }

    return retval;
}
    
/*
 * MPI_Issend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Issend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Issend
#else
int mpi_PMPI_Issend
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm, 
    MPI_Request *request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Issend");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Issend(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Issend));

    }

    return retval;
}
  
/*
 * MPI_Rsend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Rsend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Rsend
#else
int mpi_PMPI_Rsend
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Rsend");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Rsend(buf, count, datatype, dest, tag, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Rsend));

    }

    return retval;
}
  
  
/*
 * MPI_Rsend_init
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Rsend_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Rsend_init
#else
int mpi_PMPI_Rsend_init
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm,
    MPI_Request* request )
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Rsend_init");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Rsend_init));

    }

    return retval;
}
  
/*
 * MPI_Send
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Send
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Send
#else
int mpi_PMPI_Send
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Send");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Send(buf, count, datatype, dest, tag, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Send));

    }

    return retval;
}
  
  
/*
 * MPI_Send_init
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Send_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Send_init
#else
int mpi_PMPI_Send_init
#endif
    (void* buf, 
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm,
    MPI_Request* request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Send_init");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Send_init));

    }

    return retval;
}
  
/*
 * MPI_Ssend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Ssend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Ssend
#else
int mpi_PMPI_Ssend
#endif
    (void* buf,
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Ssend");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Ssend(buf, count, datatype, dest, tag, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Ssend));

    }

    return retval;
}
  
  
/*
 * MPI_Ssend_init
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Ssend_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Ssend_init
#else
int mpi_PMPI_Ssend_init
#endif
    (void* buf,
    int count, 
    MPI_Datatype datatype, 
    int dest, 
    int tag, 
    MPI_Comm comm,
    MPI_Request* request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Ssend_init");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.source));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.tag = tag;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Ssend_init));

    }

    return retval;
}
  
/*
 * MPI_Waitall
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Waitall
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Waitall
#else
int mpi_PMPI_Waitall
#endif
    (int count, 
    MPI_Request *array_of_requests, 
    MPI_Status *status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Waitall");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Waitall(count, array_of_requests, status);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Waitall));

    }

    return retval;
}

/*
 * MPI_Finalize
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Finalize()
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Finalize()
#else
int mpi_PMPI_Finalize()
#endif
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Finalize");

    if (dotrace) {

    mpi_start_event(&event);

#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
#endif

    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Finalize();

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.source = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Finalize));

    }

    return retval;
}

/*
 * MPI_Waitsome
 
    What do I do with out and in count?
    
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Waitsome
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Waitsome
#else
int mpi_PMPI_Waitsome
#endif
    (int incount, 
    MPI_Request *array_of_requests, 
    int *outcount, 
    int *array_of_indices, 
    MPI_Status *array_of_statuses)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Waitsome");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Waitsome( incount, array_of_requests, 
    	    	    	    outcount, array_of_indices, 
			    array_of_statuses);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Waitsome));

    }

    return retval;
}

/*
 * MPI_Testsome
 
    What do I do with out and in count?
    
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Testsome
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Testsome
#else
int mpi_PMPI_Testsome
#endif
    (int incount, 
    MPI_Request *array_of_requests, 
    int *outcount, 
    int *array_of_indices, 
    MPI_Status *array_of_statuses)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Testsome");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Testsome( incount, array_of_requests, 
    	    	    	    outcount, array_of_indices, 
			    array_of_statuses);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Testsome));

    }

    return retval;
}

/*
 * MPI_Waitany
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Waitany
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Waitany
#else
int mpi_PMPI_Waitany
#endif
    (int count,   
    MPI_Request *array_of_requests, 
    int *index, 
    MPI_Status *status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Waitany");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Waitany(count,  array_of_requests, index, status);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Waitany));

    }

    return retval;
}

/*
 * MPI_Unpack
 
    Which size do I use, insize or outsize?
    I'm going to use outsize.
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Unpack
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Unpack
#else
int mpi_PMPI_Unpack
#endif
    (void* inbuf, 
    int insize, 
    int *position, 
    void *outbuf, 
    int outcount, 
    MPI_Datatype datatype, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Unpack");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Unpack(inbuf, insize, position, outbuf, 
    	    	    	 outcount, datatype, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = outcount * datatype_size;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
    event.communicator = -1;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Unpack));

    }

    return retval;
}

/*
 * MPI_Wait
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Wait
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Wait
#else
int mpi_PMPI_Wait
#endif
    (MPI_Request *request, 
    MPI_Status *status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Wait");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Wait(request, status);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Wait));

    }

    return retval;
}

/*
 * MPI_Testany
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Testany
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Testany
#else
int mpi_PMPI_Testany
#endif
    (int count,
    MPI_Request *array_of_requests,
    int *index,
    int *flag,
    MPI_Status *status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Testany");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Testany(count,array_of_requests, index, flag, status);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Testany));

    }

    return retval;
}

/*
 * MPI_Testall
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Testall
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Testall
#else
int mpi_PMPI_Testall
#endif
    (int count,
    MPI_Request *array_of_requests, 
    int *flag, 
    MPI_Status *status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Testall");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Testall(count, array_of_requests, flag, status);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Testall));

    }

    return retval;
}

/*
 * MPI_Test
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Test
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Test
#else
int mpi_PMPI_Test
#endif
    (MPI_Request *request, 
    int *flag, 
    MPI_Status *status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Test");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Test(request, flag, status);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Test));

    }

    return retval;
}

/*
 * MPI_Scan
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Scan
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Scan
#else
int mpi_PMPI_Scan
#endif
    (void* sendbuf, 
    void* recvbuf, 
    int count, 
    MPI_Datatype datatype, 
    MPI_Op op, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Scan");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Scan));

    }

    return retval;
}

/*
 * MPI_Request_free
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Request_free
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Request_free
#else
int mpi_PMPI_Request_free
#endif
    (MPI_Request *request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Request_free");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Request_free(request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Request_free));

    }

    return retval;
}

/*
 * MPI_Reduce_scatter
 
    This is questionable with recvcounts.
    
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Reduce_scatter
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Reduce_scatter
#else
int mpi_PMPI_Reduce_scatter
#endif
    (void* sendbuf, 
    void* recvbuf, 
    int *recvcounts, 
    MPI_Datatype datatype, 
    MPI_Op op, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Reduce_scatter");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcounts,  datatype, op, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = *recvcounts * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Reduce_scatter));

    }

    return retval;
}

/*
 * MPI_Reduce
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Reduce
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Reduce
#else
int mpi_PMPI_Reduce
#endif
    (void* sendbuf, 
    void* recvbuf, 
    int count, 
    MPI_Datatype datatype, 
    MPI_Op op, 
    int root, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Reduce");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Reduce));

    }

    return retval;
}

/*
 * MPI_Pack
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Pack
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Pack
#else
int mpi_PMPI_Pack
#endif
    (void* inbuf, 
    int incount, 
    MPI_Datatype datatype, 
    void *outbuf, 
    int outsize, 
    int *position, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Pack");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Pack( inbuf, incount, datatype, outbuf, 
    	    	    	outsize, position, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = incount * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Pack));

    }

    return retval;
}

/*
 * MPI_Init
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Init
#else
int mpi_PMPI_Init
#endif
    (int *argc, 
    char ***argv)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Init");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Init(argc, argv);

#if defined (CBTF_SERVICE_USE_OFFLINE)
    int oss_rank = -1;
    PMPI_Comm_rank(MPI_COMM_WORLD, &oss_rank);
    CBTF_mpi_rank = oss_rank;
#endif

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Init));

    }

    return retval;
}

/*
 * MPI_Get_count
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Get_count
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Get_count
#else
int mpi_PMPI_Get_count
#endif
    (MPI_Status *status, MPI_Datatype datatype, int *count)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Get_count");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Get_count(status, datatype, count);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = *count * datatype_size;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
    event.communicator = -1;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Get_count));

    }

    return retval;
}

/*
 * MPI_Gatherv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Gatherv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Gatherv
#else
int mpi_PMPI_Gatherv
#endif
    (void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int *recvcounts, 
    int *displs, 
    MPI_Datatype recvtype, 
    int root, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Gatherv");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, 
    	    	    	  recvcounts, displs, recvtype, root, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = *recvcounts * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
    event.communicator = -1;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Gatherv));

    }

    return retval;
}

/*
 * MPI_Gather
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Gather
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Gather
#else
int mpi_PMPI_Gather
#endif
    (void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int recvcount, 
    MPI_Datatype recvtype, 
    int root, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Gather");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Gather(sendbuf, sendcount, sendtype, 
    	    	    	 recvbuf, recvcount, recvtype, 
			 root, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = recvcount * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Gather));

    }

    return retval;
}

/*
 * MPI_Cancel
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Cancel
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Cancel
#else
int mpi_PMPI_Cancel
#endif
    (MPI_Request *request)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Cancel");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Cancel(request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Cancel));

    }

    return retval;
}

/*
 * MPI_Bcast
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Bcast
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Bcast
#else
int mpi_PMPI_Bcast
#endif
    (void* buffer, 
    int count, 
    MPI_Datatype datatype, 
    int root, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Bcast");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Bcast(buffer, count, datatype, root, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Bcast));

    }

    return retval;
}

/*
 * MPI_Barrier
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Barrier
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Barrier
#else
int mpi_PMPI_Barrier
#endif
    (MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Barrier");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Barrier(comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    event.communicator = (int64_t) comm;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Barrier));

    }

    return retval;
}

/*
 * MPI_Alltoallv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Alltoallv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Alltoallv
#else
int mpi_PMPI_Alltoallv
#endif
    (void* sendbuf, 
    int *sendcounts, 
    int *sdispls, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int *recvcounts, 
    int *rdispls, 
    MPI_Datatype recvtype, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Alltoallv");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Alltoallv(sendbuf, sendcounts, sdispls, 
    	    	    	    sendtype, recvbuf, recvcounts, 
			    rdispls, recvtype, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = *recvcounts * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Alltoallv));

    }

    return retval;
}

/*
 * MPI_Alltoall
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Alltoall
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Alltoall
#else
int mpi_PMPI_Alltoall
#endif
    (void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int recvcount, 
    MPI_Datatype recvtype, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Alltoall");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Alltoall( sendbuf, sendcount, sendtype, 
    	    	    	    recvbuf, recvcount, recvtype, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = recvcount * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Alltoall));

    }

    return retval;
}

/*
 * MPI_Allreduce
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Allreduce
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Allreduce
#else
int mpi_PMPI_Allreduce
#endif
    (void* sendbuf, 
    void* recvbuf, 
    int count, 
    MPI_Datatype datatype, 
    MPI_Op op, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Allreduce");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    event.size = count * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) datatype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Allreduce));

    }

    return retval;
}

/*
 * MPI_Allgatherv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Allgatherv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Allgatherv
#else
int mpi_PMPI_Allgatherv
#endif
    (void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int *recvcounts, 
    int *displs, 
    MPI_Datatype recvtype, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Allgatherv");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Allgatherv(sendbuf, sendcount, sendtype, 
    	    	    	     recvbuf, recvcounts, displs, 
			     recvtype, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = *recvcounts * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Allgatherv));

    }

    return retval;
}

/*
 * MPI_Allgather
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Allgather
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Allgather
#else
int mpi_PMPI_Allgather
#endif
    (void* sendbuf, 
    int sendcount, 
    MPI_Datatype sendtype, 
    void* recvbuf, 
    int recvcount, 
    MPI_Datatype recvtype, 
    MPI_Comm comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Allgather");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, 
    	    	    	    recvcount, recvtype, comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    PMPI_Comm_rank(MPI_COMM_WORLD, &(event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    event.size = recvcount * datatype_size;
    event.communicator = (int64_t) comm;
    event.datatype = (int64_t) recvtype;
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.tag = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Allgather));

    }

    return retval;
}

/*
 * MPI_Scatter
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Scatter
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Scatter
#else
int mpi_PMPI_Scatter
#endif
    	(void*     	sendbuf, 
    	int 	    	sendcount, 
    	MPI_Datatype	sendtype, 

    	void*     	recvbuf, 
    	int 	    	recvcount, 
    	MPI_Datatype	recvtype, 

    	int 	    	root, 
    	MPI_Comm    	comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event send_event,recv_event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Scatter");

    if (dotrace) {


    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    /* Set up the send record */
    mpi_start_event(&send_event);
    mpi_start_event(&recv_event);
    send_event.source = root;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(send_event.source));
    PMPI_Type_size(sendtype, &datatype_size);
    send_event.size = sendcount * datatype_size;
    send_event.datatype = (int64_t) sendtype;
    

    /* Initialize unused arguments */
    send_event.tag = 0;
    send_event.start_time = CBTF_GetTime();
#else
    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();
#endif

    }

    retval = PMPI_Scatter( sendbuf, sendcount, sendtype,  
    	    	    	    recvbuf, recvcount, recvtype, 
			    root, comm);

    if (dotrace) {

    
    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    send_event.stop_time = CBTF_GetTime();
    send_event.communicator = (int64_t) comm;
    send_event.retval = retval;
    recv_event.start_time = send_event.start_time;
    recv_event.stop_time = send_event.stop_time;
    mpi_record_event(&send_event, CBTF_GetAddressOfFunction(PMPI_Scatter));

    /* Set up the recv record */
    PMPI_Type_size(recvtype, &datatype_size);
    recv_event.size = recvcount * datatype_size;
    recv_event.datatype = (int64_t) recvtype;

    recv_event.communicator = (int64_t) comm;
    recv_event.retval = retval;

    /* Initialize unused arguments */
    recv_event.tag = 0;
    mpi_record_event(&recv_event, CBTF_GetAddressOfFunction(PMPI_Scatter));
#else
    event.stop_time = CBTF_GetTime();
    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Scatter));
#endif


    }

    return retval;
}

/*
 * MPI_Scatterv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Scatterv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Scatterv
#else
int mpi_PMPI_Scatterv
#endif
    	(void*     	sendbuf, 
    	int* 	    	sendcounts, 
	int*	    	displs,
    	MPI_Datatype	sendtype, 

    	void*     	recvbuf, 
    	int 	    	recvcount, 
    	MPI_Datatype	recvtype, 

    	int 	    	root, 
    	MPI_Comm    	comm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event send_event,recv_event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Scatterv");

    if (dotrace) {


    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    /* Set up the send record */
    mpi_start_event(&send_event);
    mpi_start_event(&recv_event);
    send_event.source = root;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(send_event.source));
    PMPI_Type_size(sendtype, &datatype_size);
    /* This is surly wrong */
    send_event.size = sendcounts[0] * datatype_size;
    send_event.datatype = (int64_t) sendtype;
    send_event.start_time = CBTF_GetTime();
#else
    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();
#endif
    

    }

    retval = PMPI_Scatterv( sendbuf, sendcounts, displs, sendtype,  
    	    	    	    recvbuf, recvcount, recvtype, 
			    root, comm);

    if (dotrace) {

    
    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    send_event.stop_time = CBTF_GetTime();
    send_event.communicator = (int64_t) comm;
    send_event.retval = retval;
    recv_event.start_time = send_event.start_time;
    recv_event.stop_time = send_event.stop_time;

    /* Initialize unused arguments */
    send_event.tag = 0;

    send_event.stop_time = CBTF_GetTime();
    mpi_record_event(&send_event, CBTF_GetAddressOfFunction(PMPI_Scatterv));
#else
    event.stop_time = CBTF_GetTime();
    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Scatterv));
#endif

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    /* Set up the recv record */
    PMPI_Type_size(recvtype, &datatype_size);
    recv_event.size = recvcount * datatype_size;
    recv_event.datatype = (int64_t) recvtype;

    recv_event.communicator = (int64_t) comm;
    recv_event.retval = retval;

    /* Initialize unused arguments */
    recv_event.tag = 0;
    recv_event.stop_time = CBTF_GetTime();
    mpi_record_event(&recv_event, CBTF_GetAddressOfFunction(PMPI_Scatterv));
#endif


    }

    return retval;
}

/*
 * MPI_Sendrecv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Sendrecv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Sendrecv
#else
int mpi_PMPI_Sendrecv
#endif
    	(void*     	sendbuf, 
    	int 	    	sendcount, 
    	MPI_Datatype	sendtype, 
    	int 	    	dest, 
    	int	    	sendtag, 

    	void*     	recvbuf, 
    	int 	    	recvcount, 
    	MPI_Datatype	recvtype, 
    	int 	    	source, 
    	int	    	recvtag, 

    	MPI_Comm comm, 
    	MPI_Status* status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event send_event,recv_event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Sendrecv");

    if (dotrace) {


    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    /* Set up the send record */
    mpi_start_event(&send_event);
    mpi_start_event(&recv_event);
    send_event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(send_event.source));
    PMPI_Type_size(sendtype, &datatype_size);
    send_event.size = sendcount * datatype_size;
    send_event.tag = sendtag;
    send_event.datatype = (int64_t) sendtype;
    send_event.start_time = CBTF_GetTime();
#else
    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();
#endif

    }

    retval = PMPI_Sendrecv( sendbuf, sendcount, sendtype, dest, sendtag, 
    	    	    	    recvbuf, recvcount, recvtype, source, recvtag,
			    comm, status);

    if (dotrace) {

    
    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    send_event.stop_time = CBTF_GetTime();
    send_event.communicator = (int64_t) comm;
    send_event.retval = retval;
    recv_event.start_time = send_event.start_time;
    recv_event.stop_time = send_event.stop_time;
    mpi_record_event(&send_event, CBTF_GetAddressOfFunction(PMPI_Sendrecv));

    /* Set up the recv record */
    recv_event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(recv_event.destination));
    PMPI_Type_size(recvtype, &datatype_size);
    recv_event.size = recvcount * datatype_size;
    recv_event.tag = recvtag;
    recv_event.datatype = (int64_t) recvtype;

    recv_event.communicator = (int64_t) comm;
    recv_event.retval = retval;

    send_event.stop_time = CBTF_GetTime();
    mpi_record_event(&recv_event, CBTF_GetAddressOfFunction(PMPI_Sendrecv));
#else
    event.stop_time = CBTF_GetTime();
    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Sendrecv));
#endif

    }

    return retval;
}

/*
 * MPI_Sendrecv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Sendrecv_replace
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Sendrecv_replace
#else
int mpi_PMPI_Sendrecv_replace
#endif
    	(void*     	buf, 
    	int 	    	count, 
    	MPI_Datatype	datatype, 
    	int 	    	dest, 
    	int	    	sendtag, 

    	int 	    	source, 
    	int	    	recvtag, 

    	MPI_Comm    	comm, 
    	MPI_Status* 	status)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event send_event,recv_event;
    int datatype_size;
#else
    CBTF_mpi_event event;
#endif
    
    bool_t dotrace = mpi_do_trace("MPI_Sendrecv_replace");

    if (dotrace) {


    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    /* Set up the send record */
    mpi_start_event(&send_event);
    mpi_start_event(&recv_event);
    send_event.destination = dest;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(send_event.source));
    PMPI_Type_size(datatype, &datatype_size);
    send_event.size = count * datatype_size;
    send_event.tag = sendtag;
    send_event.datatype = (int64_t) datatype;
    send_event.start_time = CBTF_GetTime();
#else
    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();
#endif
    

    }

    retval = PMPI_Sendrecv_replace( buf, count, datatype, dest, sendtag, 
    	    	    	    	    source, recvtag,
			    	    comm, status);

    if (dotrace) {

    
    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    send_event.stop_time = CBTF_GetTime();
    send_event.communicator = (int64_t) comm;
    send_event.retval = retval;
    recv_event.start_time = send_event.start_time;
    recv_event.stop_time = send_event.stop_time;
    mpi_record_event(&send_event, CBTF_GetAddressOfFunction(PMPI_Sendrecv_replace));

    /* Set up the recv record */
    recv_event.source = source;
    PMPI_Comm_rank(MPI_COMM_WORLD, &(recv_event.destination));
    PMPI_Type_size(datatype, &datatype_size);
    recv_event.size = count * datatype_size;
    recv_event.tag = recvtag;
    recv_event.datatype = (int64_t) datatype;

    recv_event.communicator = (int64_t) comm;
    recv_event.retval = retval;
    mpi_record_event(&recv_event, CBTF_GetAddressOfFunction(PMPI_Sendrecv_replace));
#else
    event.stop_time = CBTF_GetTime();
    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Sendrecv_replace));
#endif


    }

    return retval;
}



/*
 *-----------------------------------------------------------------------------
 *
 * Cartesian Toplogy functions
 *
 *-----------------------------------------------------------------------------
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Cart_create
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Cart_create
#else
int mpi_PMPI_Cart_create
#endif
		     (MPI_Comm comm_old,
                     int ndims,
                     int* dims,
                     int* periodv,
                     int reorder,
                     MPI_Comm* comm_cart)
{

    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpi_PMPI_Cart_create called, comm_old = %d \n", comm_old);
      fflush(stderr);
    }

    bool_t dotrace = mpi_do_trace("MPI_Cart_create");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Cart_create(comm_old, ndims, dims, periodv, reorder, comm_cart);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif


    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Cart_create));

    }

    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Cart_sub
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Cart_sub
#else
int mpi_PMPI_Cart_sub
#endif
		   (MPI_Comm comm,
                   int *rem_dims,
                   MPI_Comm *newcomm)
{

    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpi_PMPI_Cart_sub called, comm = %d \n", comm);
      fflush(stderr);
    }

    bool_t dotrace = mpi_do_trace("MPI_Cart_sub");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Cart_sub(comm, rem_dims, newcomm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Cart_sub));

    }

    return retval;
}



#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Graph_create
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Graph_create
#else
int mpi_PMPI_Graph_create
#endif
		      (MPI_Comm comm_old,
                      int nnodes,
                      int* index,
                      int* edges,
                      int reorder,
                      MPI_Comm* comm_graph)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    bool_t dotrace = mpi_do_trace("MPI_Graph_create");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpi_PMPI_Graph_create called, comm_old= %d \n", comm_old);
      fflush(stderr);
    }

    }

    retval = PMPI_Graph_create(comm_old, nnodes, index, edges, reorder, comm_graph);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Graph_create));

    }

    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Intercomm_create
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Intercomm_create
#else
int mpi_PMPI_Intercomm_create
#endif
			  (MPI_Comm local_comm,
                          int local_leader,
                          MPI_Comm peer_comm,
                          int remote_leader,
                          int tag,
                          MPI_Comm *newintercomm)

{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpi_PMPI_Intercomm_create called, local_comm = %d \n", local_comm);
      fflush(stderr);
    }

    bool_t dotrace = mpi_do_trace("MPI_Intercomm_create");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();


    }

    retval = PMPI_Intercomm_create(local_comm, local_leader, peer_comm,
                                 remote_leader, tag, newintercomm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Intercomm_create));

    }

    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Intercomm_merge
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Intercomm_merge
#else
int mpi_PMPI_Intercomm_merge
#endif
			 (MPI_Comm intercomm,
                         int high,
                         MPI_Comm *newcomm)
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpi_PMPI_Intercomm_merge called, intercomm = %d \n", intercomm);
      fflush(stderr);
    }

    bool_t dotrace = mpi_do_trace("MPI_Intercomm_merge");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Intercomm_merge(intercomm, high, newcomm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Intercomm_merge));

    }

    return retval;
}


/* ------- Destructors ------- */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Comm_free
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Comm_free
#else
int mpi_PMPI_Comm_free
#endif
			(MPI_Comm* comm )
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpi_PMPI_Comm_free called, comm = %d \n", comm);
      fflush(stderr);
    }

    bool_t dotrace = mpi_do_trace("MPI_Comm_free");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Comm_free(comm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Comm_free));

    }

    return retval;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Communicator management
 *
 *-----------------------------------------------------------------------------
 */

/* ------- Constructors ------- */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Comm_dup
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Comm_dup
#else
int mpi_PMPI_Comm_dup
#endif
			(MPI_Comm comm,
                        MPI_Comm* newcomm )
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpi_PMPI_Comm_dup called, comm = %d \n", comm);
      fflush(stderr);
    }

    bool_t dotrace = mpi_do_trace("MPI_Comm_dup");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Comm_dup(comm, newcomm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Comm_dup));

    }

    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Comm_create
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Comm_create
#else
int mpi_PMPI_Comm_create
#endif
		     (MPI_Comm comm,
                     MPI_Group group,
                     MPI_Comm* newcomm )
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    bool_t dotrace = mpi_do_trace("MPI_Comm_create");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpi_PMPI_Comm_create called, comm = %d \n", comm);
      fflush(stderr);
    }

    }

    retval = PMPI_Comm_create(comm, group, newcomm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Comm_create));

    }

    return retval;
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Comm_split
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Comm_split
#else
int mpi_PMPI_Comm_split
#endif
			  (MPI_Comm comm,
                          int color,
                          int key,
                          MPI_Comm* newcomm )
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpi_PMPI_Comm_split called, comm = %d \n", comm);
      fflush(stderr);
    }

    bool_t dotrace = mpi_do_trace("MPI_Comm_split");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Comm_split(comm, color, key, newcomm);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Comm_split));

    }

    return retval;
}


/* -- MPI_Start -- */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Start
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Start
#else
int mpi_PMPI_Start
#endif
		( MPI_Request* request )
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    if (debug_trace) {
      fprintf(stderr, "WRAPPER, mpi_PMPI_Start called\n");
      fflush(stderr);
    }

    bool_t dotrace = mpi_do_trace("MPI_Start");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Start(request);

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Start));

    }

    return retval;
}

/* -- MPI_Startall -- */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int MPI_Startall
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_MPI_Startall
#else
int mpi_PMPI_Startall
#endif
			( int count,
                        MPI_Request *array_of_requests )
{
    int retval;
#if defined(EXTENDEDTRACE)
    CBTF_mpit_event event;
#else
    CBTF_mpi_event event;
#endif

    if (debug_trace) {
        fprintf(stderr, "WRAPPER, mpi_PMPI_Startall called, count = %d \n", count);
        fflush(stderr);
    }

    bool_t dotrace = mpi_do_trace("MPI_Startall");

    if (dotrace) {

    mpi_start_event(&event);
    event.start_time = CBTF_GetTime();

    }

    retval = PMPI_Startall( count, array_of_requests );

    if (dotrace) {

    event.stop_time = CBTF_GetTime();

    /*TRACE DETAILS*/
#if defined(EXTENDEDTRACE)
    event.retval = retval;

    /* Initialize unused arguments */
    event.source = -1;
    event.size = 0;
    event.tag = 0;
    event.communicator = -1;
    event.destination = -1;
    event.datatype = 0;
#endif

    mpi_record_event(&event, CBTF_GetAddressOfFunction(PMPI_Startall));

    }

    return retval;
}
