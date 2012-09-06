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
 * MPI FORTRAN function wrappers for the MPI tracing collector.
 * NOTE: The fortran wrappers call the C wrapper implementation
 * in the wrappers.c file (where the actual tracing is done).
 * This means that stacktraces from fortran applications will
 * have one extra frame through this file.
 *
 */

#include <mpi.h>

#if defined (SGI_MPT)
#define MPI_Status_c2f(c,f) *(MPI_Status *)f=*(MPI_Status *)c
#define MPI_Status_f2c(f,c) *(MPI_Status *)c=*(MPI_Status *)f 
#endif

/* Is there a better way to do this? */
#ifndef MPI_STATUS_SIZE
#define MPI_STATUS_SIZE 5
#endif

/* This macro is inspired by the vampirtrace macro and
 * is specific to the needs of OpenSpeedShop. We need to
 * wrap fortran functions such that we can use the wrappers
 * with libmonitor.so AND libmonitor_wrap.a.
 */

#define OSS_WRAP_FORTRAN( upper_case, wrapper_function, \
	static_name, signature, params) \
  void wrapper_function##_ signature { wrapper_function params; } \
  void static_name##_ signature { wrapper_function params; } \
  void wrapper_function##__ signature { wrapper_function params; } \
  void static_name##__ signature { wrapper_function params; } \
  void upper_case signature { wrapper_function params; }


/*
 * MPI_Irecv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_irecv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_irecv
#endif
  (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* source,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Irecv(buf, *count, MPI_Type_f2c(*datatype), *source, *tag,
                    MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_IRECV,mpi_irecv,__wrap_mpi_irecv,
   (char* buf,
    MPI_Fint* count,
    MPI_Fint *datatype, 
    MPI_Fint* source,
    MPI_Fint* tag,
    MPI_Fint *comm, 
    MPI_Fint *request,
    MPI_Fint *ierr),
    (buf, count, datatype, source, tag, comm, request, ierr))

/*
 * MPI_Recv
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_recv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_recv
#endif
   (char* buf,
    MPI_Fint* count,
    MPI_Fint *datatype, 
    MPI_Fint* source,
    MPI_Fint* tag,
    MPI_Fint *comm, 
    MPI_Fint *status,
    MPI_Fint *ierr)
{
  MPI_Status c_status;
  *ierr = MPI_Recv(buf, *count, MPI_Type_f2c(*datatype), *source, *tag,
                   MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_RECV,mpi_recv,__wrap_mpi_recv,
   (char* buf,
    MPI_Fint* count,
    MPI_Fint *datatype, 
    MPI_Fint* source,
    MPI_Fint* tag,
    MPI_Fint *comm, 
    MPI_Fint *status,
    MPI_Fint *ierr),
    (buf, count, datatype, source, tag, comm, status, ierr))

/*
 * MPI_Recv_init
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_recv_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_recv_init
#endif
   (void* buf, 
    MPI_Fint* count, 
    MPI_Datatype* datatype, 
    MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Comm* comm, 
    MPI_Request *request,
    MPI_Fint* ierr)
{
  *ierr = MPI_Recv_init(buf, *count, *datatype, *source, *tag, *comm, request);
}
OSS_WRAP_FORTRAN(MPI_RECV_INIT,mpi_recv_init,__wrap_mpi_recv_init,
   (void* buf, 
    MPI_Fint* count, 
    MPI_Datatype* datatype, 
    MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Comm* comm, 
    MPI_Request *request,
    MPI_Fint* ierr),
    (buf, count, datatype, source, tag, comm, request, ierr))
  
/*
 * MPI_Iprobe
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_iprobe
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_iprobe
#endif
   (MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *flag, 
    MPI_Fint *status,
    MPI_Fint* ierr)
{
  MPI_Status c_status;
  *ierr = MPI_Iprobe(*source, *tag, MPI_Comm_f2c(*comm), flag, &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_IPROBE,mpi_iprobe,__wrap_mpi_iprobe,
   (MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *flag, 
    MPI_Fint *status,
    MPI_Fint* ierr),
    (source, tag, comm, flag, status, ierr))
  
/*
 * MPI_probe
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_probe
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_probe
#endif
   (MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *status,
    MPI_Fint* ierr)
{
  MPI_Status c_status;
  *ierr = MPI_Probe(*source, *tag, MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_PROBE,mpi_probe,__wrap_mpi_probe,
   (MPI_Fint* source, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *status,
    MPI_Fint* ierr),
   (source, tag, comm, status, ierr))


/*
 * MPI_Isend
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_isend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_isend
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest,
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Isend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                    MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_ISEND,mpi_isend,__wrap_mpi_isend,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))


/*
 * MPI_Bsend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_bsend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_bsend
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Comm* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Bsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                    MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_BSEND,mpi_bsend,__wrap_mpi_bsend,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, ierr))
  

/*
 * MPI_Bsend_init
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_bsend_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_bsend_init
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Bsend_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                         MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_BSEND_INIT,mpi_bsend_init,__wrap_mpi_bsend_init,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))

  
/*
 * MPI_Ibsend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_ibsend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_ibsend
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Ibsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                     MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_IBSEND,mpi_ibsend,__wrap_mpi_ibsend,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))
  

/*
 * MPI_Irsend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_irsend    
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_irsend    
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint *request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Irsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                     MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_IRSEND,mpi_irsend,__wrap_mpi_irsend,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))
    

/*
 * MPI_Issend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_issend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_issend
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Issend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                     MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_ISSEND,mpi_issend,__wrap_mpi_issend,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm, 
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))
  
/*
 * MPI_Rsend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_rsend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_rsend
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint *ierr)
{
  *ierr = MPI_Rsend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                    MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_RSEND,mpi_rsend,__wrap_mpi_rsend,
   (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* dest,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, ierr))
  
  
/*
 * MPI_Rsend_init
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_rsend_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_rsend_init
#endif
   (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* dest,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Rsend_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                         MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_RSEND_INIT,mpi_rsend_init,__wrap_mpi_rsend_init,
   (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* dest,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))



#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_send
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_send
#endif
   (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* dest,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr =  MPI_Send(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
		    MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_SEND,mpi_send,__wrap_mpi_send,
   (char* buf,
    MPI_Fint* count,
    MPI_Fint* datatype,
    MPI_Fint* dest,
    MPI_Fint* tag,
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, ierr))

  

/*
 * MPI_Send_init
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_send_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_send_init
#endif
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Send_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                        MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_SEND_INIT,mpi_send_init,__wrap_mpi_send_init,
   (char* buf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))

  
/*
 * MPI_Ssend
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_ssend
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_ssend
#endif
   (char* buf,
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Ssend(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                    MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_SSEND,mpi_ssend,__wrap_mpi_ssend,
   (char* buf,
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, ierr))

  
  
/*
 * MPI_Ssend_init
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_ssend_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_ssend_init
#endif
   (char* buf,
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  *ierr = MPI_Ssend_init(buf, *count, MPI_Type_f2c(*datatype), *dest, *tag,
                         MPI_Comm_f2c(*comm), &l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_SSEND_INIT,mpi_ssend_init,__wrap_mpi_ssend_init,
   (char* buf,
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* dest, 
    MPI_Fint* tag, 
    MPI_Fint* comm,
    MPI_Fint* request,
    MPI_Fint* ierr),
   (buf, count, datatype, dest, tag, comm, request, ierr))


  
/*
 * MPI_Waitall
 */
#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_waitall
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_waitall
#endif
   (MPI_Fint* count, 
    MPI_Fint array_of_requests[], 
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr)
{
  int i;
  MPI_Request* l_request = 0;
  MPI_Status* c_status = 0;

  if (*count > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*count));
    c_status = (MPI_Status*)malloc(sizeof(MPI_Status)*(*count));
    for (i=0; i<*count; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Waitall(*count, l_request, c_status);
  for (i=0; i<*count; i++) {
    array_of_requests[i] = MPI_Request_c2f(l_request[i]);
  }
  if (*ierr == MPI_SUCCESS) {
    for (i=0; i<*count; i++) {
      MPI_Status_c2f(&(c_status[i]), &(array_of_statuses[i][0]));
    }
  }
}
OSS_WRAP_FORTRAN(MPI_WAITALL,mpi_waitall,__wrap_mpi_waitall,
   (MPI_Fint* count, 
    MPI_Fint array_of_requests[], 
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr),
   (count, array_of_requests, array_of_statuses, ierr))



/*
 * MPI_Finalize
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_finalize
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_finalize
#endif
    (MPI_Fint *ierr )
{
    *ierr = MPI_Finalize();
}
OSS_WRAP_FORTRAN(MPI_FINALIZE,mpi_finalize,__wrap_mpi_finalize,
    (MPI_Fint *ierr ),
    (ierr))


/*
 * MPI_Waitsome
 
    What do I do with out and in count?
    
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_waitsome
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_waitsome
#endif
   (MPI_Fint* incount,
    MPI_Fint array_of_requests[],
    MPI_Fint* outcount,
    MPI_Fint array_of_indices[],
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr)
{
  int i, j, found;
  MPI_Request *l_request = 0;
  MPI_Status  *c_status = 0;

  if (*incount > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*incount));
    c_status = (MPI_Status*)malloc(sizeof(MPI_Status)*(*incount));
    for (i=0; i<*incount; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Waitsome(*incount, l_request, outcount, array_of_indices,
                       c_status);
  if (*ierr == MPI_SUCCESS) {
    for (i=0; i<*incount; i++) {
      if (i < *outcount) {
        if (array_of_indices[i] >= 0) {
          array_of_requests[array_of_indices[i]] =
            MPI_Request_c2f(l_request[array_of_indices[i]]);
        }
      } else {
        found = j = 0;
        while ( (!found) && (j<*outcount) ) {
          if (array_of_indices[j++] == i) found = 1;
        }
        if (!found) array_of_requests[i] = MPI_Request_c2f(l_request[i]);
      }
    }
    for (i=0; i<*outcount; i++) {
      MPI_Status_c2f(&c_status[i], &(array_of_statuses[i][0]));
      /* See the description of waitsome in the standard;
         the Fortran index ranges are from 1, not zero */
      if (array_of_indices[i] >= 0) array_of_indices[i]++;
    }
  }
}
OSS_WRAP_FORTRAN(MPI_WAITSOME,mpi_waitsome,__wrap_mpi_waitsome,
   (MPI_Fint* incount,
    MPI_Fint array_of_requests[],
    MPI_Fint* outcount,
    MPI_Fint array_of_indices[],
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr),
   (incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierr))


/*
 * MPI_Testsome
 
    What do I do with out and in count?
    
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_testsome
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_testsome
#endif
   (MPI_Fint* incount,
    MPI_Fint array_of_requests[],
    MPI_Fint *outcount,
    MPI_Fint array_of_indices[],
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr)
{
  int i, j, found;
  MPI_Request *l_request = 0;
  MPI_Status  *c_status = 0;

  if (*incount > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*incount));
    c_status = (MPI_Status*)malloc(sizeof(MPI_Status)*(*incount));
    for (i=0; i<*incount; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Testsome(*incount, l_request, outcount, array_of_indices,
                         c_status);
  if (*ierr == MPI_SUCCESS) {
    for (i=0; i<*incount; i++) {
      if (i < *outcount) {
        array_of_requests[array_of_indices[i]] =
          MPI_Request_c2f(l_request[array_of_indices[i]]);
      } else {
        found = j = 0;
        while ( (!found) && (j<*outcount) ) {
          if (array_of_indices[j++] == i) found = 1;
        }
        if (!found) array_of_requests[i] = MPI_Request_c2f(l_request[i]);
      }
    }
    for (i=0; i<*outcount; i++) {
      MPI_Status_c2f(&c_status[i], &(array_of_statuses[i][0]));
      /* See the description of testsome in the standard;
         the Fortran index ranges are from 1, not zero */
      if (array_of_indices[i] >= 0) array_of_indices[i]++;
    }
  }
}
OSS_WRAP_FORTRAN(MPI_TESTSOME,mpi_testsome,__wrap_mpi_testsome,
   (MPI_Fint* incount,
    MPI_Fint array_of_requests[],
    MPI_Fint *outcount,
    MPI_Fint array_of_indices[],
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr),
   (incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierr))



/*
 * MPI_Waitany
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_waitany
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_waitany
#endif
   (MPI_Fint* count,   
    MPI_Fint array_of_requests[],
    MPI_Fint* index, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{
  int i;
  MPI_Request *l_request = 0;
  MPI_Status c_status;

  if (*count > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*count));
    for (i=0; i<*count; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Waitany(*count, l_request, index, &c_status);
  if (*ierr == MPI_SUCCESS) {
    if (*index >= 0) {
      /* index may be MPI_UNDEFINED if all are null */
      array_of_requests[*index] = MPI_Request_c2f(l_request[*index]);

      /* See the description of waitany in the standard;
         the Fortran index ranges are from 1, not zero */
      (*index)++;
    }
    MPI_Status_c2f(&c_status, status);
  }
}
OSS_WRAP_FORTRAN(MPI_WAITANY,mpi_waitany,__wrap_mpi_waitany,
   (MPI_Fint* count,   
    MPI_Fint array_of_requests[],
    MPI_Fint* index, 
    MPI_Fint* status,
    MPI_Fint* ierr),
   (count, array_of_requests, index, status, ierr))


/*
 * MPI_Unpack
 
    Which size do I use, insize or outsize?
    I'm going to use outsize.
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_unpack
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_unpack
#endif
   (char* inbuf, 
    MPI_Fint* insize, 
    MPI_Fint *position, 
    char *outbuf, 
    MPI_Fint* outcount, 
    MPI_Fint* datatype, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Unpack(inbuf,*insize,*position,outbuf,*outcount,
			*datatype,MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_UNPACK,mpi_unpack,__wrap_mpi_unpack,
   (char* inbuf, 
    MPI_Fint* insize, 
    MPI_Fint *position, 
    char *outbuf, 
    MPI_Fint* outcount, 
    MPI_Fint* datatype, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (inbuf, insize, position, outbuf, outcount, datatype, comm, ierr))


/*
 * MPI_Wait
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_wait
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_wait
#endif
   (MPI_Fint *request, 
    MPI_Fint *status,
    MPI_Fint* ierr)
{
  MPI_Request l_request;
  MPI_Status c_status;
  l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Wait(&l_request, &c_status);
  *request = MPI_Request_c2f(l_request);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_WAIT,mpi_wait,__wrap_mpi_wait,
   (MPI_Fint *request,
    MPI_Fint *status,
    MPI_Fint* ierr),
   (request, status, ierr))


/*
 * MPI_Testany
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_testany
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_testany
#endif
   (MPI_Fint* count,
    MPI_Fint *array_of_requests,
    MPI_Fint *index,
    MPI_Fint *flag,
    MPI_Fint *status,
    MPI_Fint* ierr)
{
  int i;
  MPI_Request *l_request = 0;
  MPI_Status c_status;

  if (*count > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*count));
    for (i=0; i<*count; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Testany(*count, l_request, index, flag, &c_status);
  if (*ierr == MPI_SUCCESS) {
    if (*flag && *index >= 0) {
      /* index may be MPI_UNDEFINED if all are null */
      array_of_requests[*index] = MPI_Request_c2f(l_request[*index]);

      /* See the description of waitany in the standard;
         the Fortran index ranges are from 1, not zero */
      (*index)++;
    }
    MPI_Status_c2f(&c_status, status);
  }
}
OSS_WRAP_FORTRAN(MPI_TESTANY,mpi_testany,__wrap_mpi_testany,
   (MPI_Fint* count,
    MPI_Fint *array_of_requests,
    MPI_Fint *index,
    MPI_Fint *flag,
    MPI_Fint *status,
    MPI_Fint* ierr),
   (count, array_of_requests, index, flag, status,ierr))



/*
 * MPI_Testall
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_testall
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_testall
#endif
   (MPI_Fint* count,
    MPI_Fint array_of_requests[],
    MPI_Fint *flag, 
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr)
{
  int i;
  MPI_Request *l_request = 0;
  MPI_Status *c_status = 0;

  if (*count > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*count));
    c_status = (MPI_Status*)malloc(sizeof(MPI_Status)*(*count));
    for (i=0; i<*count; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Testall(*count, l_request, flag, c_status);
  for (i=0; i<*count; i++) {
    array_of_requests[i] = MPI_Request_c2f(l_request[i]);
  }
  if (*ierr == MPI_SUCCESS && *flag) {
    for (i=0; i<*count; i++) {
      MPI_Status_c2f(&(c_status[i]), &(array_of_statuses[i][0]));
    }
  }
}
OSS_WRAP_FORTRAN(MPI_TESTALL,mpi_testall,__wrap_mpi_testall,
   (MPI_Fint* count,
    MPI_Fint array_of_requests[],
    MPI_Fint *flag, 
    MPI_Fint array_of_statuses[][MPI_STATUS_SIZE],
    MPI_Fint* ierr),
   (count, array_of_requests, flag, array_of_statuses, ierr))


/*
 * MPI_Test
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_test
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_test
#endif
   (MPI_Fint *request, 
    MPI_Fint *flag, 
    MPI_Fint *status,
    MPI_Fint* ierr)
{
  MPI_Status c_status;
  MPI_Request l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Test(&l_request, flag, &c_status);
  if (*ierr != MPI_SUCCESS) return;
  *request = MPI_Request_c2f(l_request);
  if (flag) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_TEST,mpi_test,__wrap_mpi_test,
   (MPI_Fint *request, 
    MPI_Fint *flag, 
    MPI_Fint *status,
    MPI_Fint* ierr),
   (request, flag, status, ierr))


/*
 * MPI_Scan
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_scan
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_scan
#endif
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Scan(sendbuf, recvbuf, *count, MPI_Type_f2c(*datatype),
                   MPI_Op_f2c(*op), MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_SCAN,mpi_scan,__wrap_mpi_scan,
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, recvbuf, count, datatype, op, comm, ierr))


/*
 * MPI_Request_free
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_request_free
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_request_free
#endif
   (MPI_Fint *request, MPI_Fint* ierr)
{
  MPI_Request l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Request_free(&l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_REQUEST_FREE,mpi_request_free,__wrap_mpi_request_free,
   (MPI_Fint *request, MPI_Fint* ierr),
   (request, ierr))


/*
 * MPI_Reduce_scatter
 
    This is questionable with recvcounts.
    
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_reduce_scatter
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_reduce_scatter
#endif
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts,
                             MPI_Type_f2c(*datatype), MPI_Op_f2c(*op),
			     MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_REDUCE_SCATTER,mpi_reduce_scatter,__wrap_mpi_reduce_scatter,
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, recvbuf, recvcounts, datatype, op, comm, ierr))


/*
 * MPI_Reduce
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_reduce
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_reduce
#endif
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Reduce(sendbuf, recvbuf, *count, MPI_Type_f2c(*datatype),
                     MPI_Op_f2c(*op), *root, MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_REDUCE,mpi_reduce,__wrap_mpi_reduce,
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, recvbuf, count, datatype, op, root, comm, ierr))


/*
 * MPI_Pack
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_pack
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_pack
#endif
   (char* inbuf, 
    MPI_Fint* incount, 
    MPI_Fint* datatype, 
    char *outbuf, 
    MPI_Fint* outsize, 
    MPI_Fint *position, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Pack(inbuf,*incount, MPI_Type_f2c(*datatype),outbuf,
		   *outsize,*position, MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_PACK,mpi_pack,__wrap_mpi_pack,
   (char* inbuf, 
    MPI_Fint* incount, 
    MPI_Fint* datatype, 
    char *outbuf, 
    MPI_Fint* outsize, 
    MPI_Fint *position, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (inbuf, incount, datatype, outbuf, outsize, position, comm, ierr))


/* MPI_Init */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_init
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_init
#endif
    (MPI_Fint *ierr )
{
    *ierr = MPI_Init(0, (char***)0);
}
OSS_WRAP_FORTRAN(MPI_INIT,mpi_init,__wrap_mpi_init,
    (MPI_Fint *ierr ),
    (ierr))


/*
 * MPI_Get_count
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int mpi_get_count
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_mpi_get_count
#endif
   (MPI_Fint *status,
    MPI_Fint *datatype,
    MPI_Fint *count,
    MPI_Fint *ierr)
{
  MPI_Status c_status;
  *ierr = MPI_Get_count(&c_status,MPI_Type_f2c(*datatype),*count);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_GET_COUNT,mpi_get_count,__wrap_mpi_get_count,
   (MPI_Fint *status,
    MPI_Fint *datatype,
    MPI_Fint *count,
    MPI_Fint *ierr),
   (status, datatype, count, ierr))


/*
 * MPI_Gatherv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int mpi_gatherv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_mpi_gatherv
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint *displs, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Gatherv(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
                      recvbuf, recvcounts, displs, MPI_Type_f2c(*recvtype),
                      *root, MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_GATHERV,mpi_gatherv,__wrap_mpi_gatherv,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint *displs, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, ierr))


/*
 * MPI_Gather
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_gather
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_gather
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Gather(sendbuf, *sendcount, MPI_Type_f2c(*sendtype), recvbuf,
                     *recvcount, MPI_Type_f2c(*recvtype), *root,
                     MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_GATHER,mpi_gather,__wrap_mpi_gather,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))

/*
 * MPI_Cancel
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
int mpi_cancel
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
int __wrap_mpi_cancel
#endif
   (MPI_Fint *request, MPI_Fint* ierr)
{
  MPI_Request l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Cancel(&l_request);
}
OSS_WRAP_FORTRAN(MPI_CANCEL,mpi_cancel,__wrap_mpi_cancel,
   (MPI_Fint *request, MPI_Fint* ierr),
   (request,ierr))


/*
 * MPI_Bcast
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_bcast
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_bcast
#endif
   (char* buffer, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Bcast(buffer, *count, MPI_Type_f2c(*datatype), *root,
                    MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_BCAST,mpi_bcast,__wrap_mpi_bcast,
   (char* buffer, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (buffer, count, datatype, root, comm, ierr))

/*
 * MPI_Barrier
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_barrier
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_barrier
#endif
    (MPI_Fint* comm, MPI_Fint* ierr)
{
  *ierr = MPI_Barrier(MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_BARRIER,mpi_barrier,__wrap_mpi_barrier,
    (MPI_Fint* comm, MPI_Fint* ierr),
    (comm, ierr))

/*
 * MPI_Alltoallv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_alltoallv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_alltoallv
#endif
   (char* sendbuf, 
    MPI_Fint *sendcounts, 
    MPI_Fint *sdispls, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* *recvcounts, 
    MPI_Fint* *rdispls, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Alltoallv(sendbuf, sendcounts, sdispls, MPI_Type_f2c(*sendtype),
                        recvbuf, recvcounts, rdispls, MPI_Type_f2c(*recvtype),
                        MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_ALLTOALLV,mpi_alltoallv,__wrap_mpi_alltoallv,
   (char* sendbuf, 
    MPI_Fint *sendcounts, 
    MPI_Fint *sdispls, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* *recvcounts, 
    MPI_Fint* *rdispls, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype,comm, ierr))


/*
 * MPI_Alltoall
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_alltoall
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_alltoall
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Alltoall(sendbuf, *sendcount, MPI_Type_f2c(*sendtype), recvbuf,
                       *recvcount, MPI_Type_f2c(*recvtype),
                       MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_ALLTOALL,mpi_alltoall,__wrap_mpi_alltoall,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr))


/*
 * MPI_Allreduce
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_allreduce
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_allreduce
#endif
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Allreduce(sendbuf, recvbuf, *count, MPI_Type_f2c(*datatype),
                        MPI_Op_f2c(*op), MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_ALLREDUCE,mpi_allreduce,__wrap_mpi_allreduce,
   (char* sendbuf, 
    char* recvbuf, 
    MPI_Fint* count, 
    MPI_Fint* datatype, 
    MPI_Fint* op, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, recvbuf, count, datatype, op, comm, ierr))


/*
 * MPI_Allgatherv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_allgatherv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_allgatherv
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint *displs, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Allgatherv(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
                         recvbuf, recvcounts, displs, MPI_Type_f2c(*recvtype),
                         MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_ALLGATHERV,mpi_allgatherv,__wrap_mpi_allgatherv,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint *recvcounts, 
    MPI_Fint *displs, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, ierr))


/*
 * MPI_Allgather
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_allgather
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_allgather
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Allgather(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
                        recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
                        MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_ALLGATHER,mpi_allgather,__wrap_mpi_allgather,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierr))


/*
 * MPI_Scatter
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_scatter
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_scatter
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Scatter(sendbuf, *sendcount, MPI_Type_f2c(*sendtype),
                      recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
                      *root, MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_SCATTER,mpi_scatter,__wrap_mpi_scatter,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))


/*
 * MPI_Scatterv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_scatterv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_scatterv
#endif
   (char* sendbuf, 
    MPI_Fint* sendcounts, 
    MPI_Fint* displs,
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr)
{
  *ierr = MPI_Scatterv(sendbuf, sendcounts, displs, MPI_Type_f2c(*sendtype),
                       recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
                       *root, MPI_Comm_f2c(*comm));
}
OSS_WRAP_FORTRAN(MPI_SCATTERV,mpi_scatterv,__wrap_mpi_scatterv,
   (char* sendbuf, 
    MPI_Fint* sendcounts, 
    MPI_Fint* displs,
    MPI_Fint* sendtype, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* root, 
    MPI_Fint* comm,
    MPI_Fint* ierr),
   (sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, ierr))


/*
 * MPI_Sendrecv
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_sendrecv
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_sendrecv
#endif
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    MPI_Fint* dest, 
    MPI_Fint* sendtag, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* source, 
    MPI_Fint* recvtag, 
    MPI_Fint* comm, 
    MPI_Fint* status,
    MPI_Fint* ierr)
{
  MPI_Status c_status;

  *ierr = MPI_Sendrecv(sendbuf, *sendcount, MPI_Type_f2c(*sendtype), *dest,
                       *sendtag, recvbuf, *recvcount, MPI_Type_f2c(*recvtype),
                       *source, *recvtag, MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_SENDRECV,mpi_sendrecv,__wrap_mpi_sendrecv,
   (char* sendbuf, 
    MPI_Fint* sendcount, 
    MPI_Fint* sendtype, 
    MPI_Fint* dest, 
    MPI_Fint* sendtag, 
    char* recvbuf, 
    MPI_Fint* recvcount, 
    MPI_Fint* recvtype, 
    MPI_Fint* source, 
    MPI_Fint* recvtag, 
    MPI_Fint* comm, 
    MPI_Fint* status,
    MPI_Fint* ierr),
   (sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status, ierr))


/*
 * MPI_Sendrecv_replace
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_sendrecv_replace
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_sendrecv_replace
#endif
    (char* buf, 
     MPI_Fint* count, 
     MPI_Fint* datatype, 
     MPI_Fint* dest, 
     MPI_Fint* sendtag, 
     MPI_Fint* source, 
     MPI_Fint* recvtag, 
     MPI_Fint* comm, 
     MPI_Fint* status,
     MPI_Fint* ierr)
{
  MPI_Status c_status;
  *ierr = MPI_Sendrecv_replace(buf, *count, MPI_Type_f2c(*datatype), *dest,
                               *sendtag, *source, *recvtag,
                               MPI_Comm_f2c(*comm), &c_status);
  if (*ierr == MPI_SUCCESS) MPI_Status_c2f(&c_status, status);
}
OSS_WRAP_FORTRAN(MPI_SENDRECV_REPLACE,mpi_sendrecv_replace,__wrap_mpi_sendrecv_replace,
    (char* buf, 
     MPI_Fint* count, 
     MPI_Fint* datatype, 
     MPI_Fint* dest, 
     MPI_Fint* sendtag, 
     MPI_Fint* source, 
     MPI_Fint* recvtag, 
     MPI_Fint* comm, 
     MPI_Fint* status,
     MPI_Fint* ierr),
    (buf, count, datatype, dest, sendtag, source, recvtag, comm, status, ierr))


#if 0

/*
 *-----------------------------------------------------------------------------
 *
 * Cartesian Toplogy functions
 *
 *-----------------------------------------------------------------------------
 */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_cart_create
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_cart_create
#endif
		     (MPI_Comm comm_old,
                     int ndims,
                     int* dims,
                     int* periodv,
                     int reorder,
                     MPI_Comm* comm_cart)
{
}

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_cart_sub 
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_cart_sub 
#endif
		   (MPI_Comm comm,
                   int *rem_dims,
                   MPI_Comm *newcomm)
{
}



#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_graph_create
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_graph_create
#endif
		     (MPI_Comm comm_old,
                      int nnodes,
                      int* index,
                      int* edges,
                      int reorder,
                      MPI_Comm* comm_graph)
{
}
#endif

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_intercomm_create
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_intercomm_create
#endif
     (MPI_Fint* local_comm,
      MPI_Fint* local_leader,
      MPI_Fint* peer_comm,
      MPI_Fint* remote_leader,
      MPI_Fint* tag,
      MPI_Fint *newintercomm,
      MPI_Fint* ierr)

{
  MPI_Comm l_newintercomm;
  *ierr = MPI_Intercomm_create(MPI_Comm_f2c(*local_comm), *local_leader,
                               MPI_Comm_f2c(*peer_comm), *remote_leader,
				*tag, &l_newintercomm);
  if (*ierr == MPI_SUCCESS) *newintercomm = MPI_Comm_c2f(l_newintercomm);
}
OSS_WRAP_FORTRAN(MPI_INTERCOMM_CREATE,mpi_intercomm_create,__wrap_mpi_intercomm_create,
     (MPI_Fint* local_comm,
      MPI_Fint* local_leader,
      MPI_Fint* peer_comm,
      MPI_Fint* remote_leader,
      MPI_Fint* tag,
      MPI_Fint *newintercomm,
      MPI_Fint* ierr),
     (local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm, ierr))

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_intercomm_merge
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_intercomm_merge
#endif
    (MPI_Fint* intercomm, MPI_Fint* high, MPI_Fint *newcomm, MPI_Fint* ierr)
{
  MPI_Comm l_newcomm;
  *ierr = MPI_Intercomm_merge(MPI_Comm_f2c(*intercomm), *high,
                              &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
}
OSS_WRAP_FORTRAN(MPI_INTERCOMM_MERGE,mpi_intercomm_merge,__wrap_mpi_intercomm_merge,
    (MPI_Fint* intercomm, MPI_Fint* high, MPI_Fint *newcomm, MPI_Fint* ierr),
    (intercomm, high, newcomm, ierr))


/* ------- Destructors ------- */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_comm_free
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_comm_free
#endif
    (MPI_Fint* comm ,MPI_Fint* ierr)
{
  MPI_Comm l_comm = MPI_Comm_f2c(*comm);
  *ierr = MPI_Comm_free(&l_comm);
  if (*ierr == MPI_SUCCESS) *comm = MPI_Comm_c2f(l_comm);
}
OSS_WRAP_FORTRAN(MPI_COMM_FREE,mpi_comm_free,__wrap_mpi_comm_free,
    (MPI_Fint* comm ,MPI_Fint* ierr),
    (comm, ierr))


/*
 *-----------------------------------------------------------------------------
 *
 * Communicator management
 *
 *-----------------------------------------------------------------------------
 */

/* ------- Constructors ------- */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_comm_dup
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_comm_dup
#endif
    (MPI_Fint* comm, MPI_Fint* newcomm, MPI_Fint* ierr)
{
  MPI_Comm l_newcomm;
  *ierr = MPI_Comm_dup(MPI_Comm_f2c(*comm), &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
}
OSS_WRAP_FORTRAN(MPI_COMM_DUP,mpi_comm_dup,__wrap_mpi_comm_dup,
    (MPI_Fint* comm, MPI_Fint* newcomm, MPI_Fint* ierr),
    (comm, newcomm, ierr))

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_comm_create
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_comm_create
#endif
    (MPI_Fint* comm, MPI_Fint* group, MPI_Fint* newcomm, MPI_Fint* ierr)
{
  MPI_Comm l_newcomm;
  *ierr = MPI_Comm_create(MPI_Comm_f2c(*comm), MPI_Group_f2c(*group),
                          &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
}
OSS_WRAP_FORTRAN(MPI_COMM_CREATE,mpi_comm_create,__wrap_mpi_comm_create,
    (MPI_Fint* comm, MPI_Fint* group, MPI_Fint* newcomm, MPI_Fint* ierr),
    (comm, group, newcomm, ierr))

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_comm_split
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_comm_split
#endif
    (MPI_Fint* comm, MPI_Fint* color, MPI_Fint* key,
     MPI_Fint* newcomm ,MPI_Fint* ierr )
{
  MPI_Comm l_newcomm;
  *ierr = MPI_Comm_split(MPI_Comm_f2c(*comm), *color, *key, &l_newcomm);
  if (*ierr == MPI_SUCCESS) *newcomm = MPI_Comm_c2f(l_newcomm);
}
OSS_WRAP_FORTRAN(MPI_COMM_SPLIT,mpi_comm_split,__wrap_mpi_comm_split,
    (MPI_Fint* comm, MPI_Fint* color, MPI_Fint* key,
     MPI_Fint* newcomm, MPI_Fint* ierr ),
    (comm, color, key, newcomm, ierr ))


/* -- MPI_Start -- */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_start
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_start
#endif
    ( MPI_Fint* request, MPI_Fint* ierr)
{
  MPI_Request l_request = MPI_Request_f2c(*request);
  *ierr = MPI_Start(&l_request);
  if (*ierr == MPI_SUCCESS) *request = MPI_Request_c2f(l_request);
}
OSS_WRAP_FORTRAN(MPI_START,mpi_start,__wrap_mpi_start,
    ( MPI_Fint* request, MPI_Fint* ierr),
    (request, ierr))

/* -- MPI_Startall -- */

#if defined (CBTF_SERVICE_USE_OFFLINE) && !defined(CBTF_STATIC)
void mpi_startall
#elif defined (CBTF_STATIC) && defined (CBTF_SERVICE_USE_OFFLINE)
void __wrap_mpi_startall
#endif
    (MPI_Fint* count,
     MPI_Fint array_of_requests[],
     MPI_Fint* ierr)
{
  int i;
  MPI_Request *l_request = 0;

  if (*count > 0) {
    l_request = (MPI_Request*)malloc(sizeof(MPI_Request)*(*count));
    for (i=0; i<*count; i++) {
      l_request[i] = MPI_Request_f2c(array_of_requests[i]);
    }
  }
  *ierr = MPI_Startall(*count, l_request);
  if (*ierr == MPI_SUCCESS) {
    for (i=0; i<*count; i++) {
      array_of_requests[i] = MPI_Request_c2f(l_request[i]);
    }
  }
}
OSS_WRAP_FORTRAN(MPI_STARTALL,mpi_startall,__wrap_mpi_startall,
    (MPI_Fint* count,
     MPI_Fint array_of_requests[],
     MPI_Fint* ierr),
    (count, array_of_requests, ierr))
