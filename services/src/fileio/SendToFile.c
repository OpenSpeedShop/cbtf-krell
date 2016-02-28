/*******************************************************************************
** Copyright (c) 2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2009-2015 The Krell Institute. All Rights Reserved.
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
 * Definition of the CBTF_SetSendToFile() and CBTF_SendToFile() functions.
 *
 */

#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Services/Path.h"

#include <errno.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


// record the executable path once here.
static char* executable_path = NULL;

/** Type defining the items stored in thread-local storage. */
typedef struct {

    /** Path of the file to which data should be written. **/
    char path[PATH_MAX];

} TLS;

#ifdef USE_EXPLICIT_TLS

/**
 * Thread-local storage key.
 *
 * Key used for looking up our thread-local storage. This key <em>must</em>
 * be globally unique across the entire Open|SpeedShop code base.
 */
static const uint32_t TLSKey = 0xFEEDBEEF;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif



/**
 * Set the "send-to" file.
 *
 * Set the name of the file to which subsequent CBTF_SendToFile() calls will
 * write their data. The name is not specified directly. It is derived from the
 * unique identifier of the collector writing the data, the identifier of the
 * process/thread making the call, the process' executable name, and a suffix
 * provided by the caller. Insures that the specified file exists.
 *
 * @param unique_id    Unique identifier of the collector writing data.
 * @param suffix       File suffix to be used.
 *
 * @ingroup RuntimeAPI
 */

void __CBTF_SetSendToFile(const char* host, pid_t pid, pthread_t posix_tid,
			  const char* unique_id, const char* suffix)
{

    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    // FIXME. Use the real __libc_malloc here in case some other tool
    // is wrapping malloc.
    TLS* tls = malloc(sizeof(TLS));
    Assert(tls != NULL);
    CBTF_SetTLS(TLSKey, tls);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);

    /* Check preconditions */
    Assert(unique_id != NULL);
    Assert(suffix != NULL);

    char* cbtf_rawdata_dir = NULL;
    char dir_path[PATH_MAX];
    int fd;

    /* Get our executable path */
    if (executable_path == NULL) {
	executable_path = strdup(CBTF_GetExecutablePath());
    }
    
    
    /* Create the directory path containing the file and the file itself */
    /* We need to add the hostname to the directory path and not just the pid.
     * e.g. cbtf-rawdata-host-pid rather than cbtf-rawdata-pid.
     * This is due to some mpi implementations allowing a pid to be the same
     * for multiple ranks as long as those ranks exist on different hosts.
     * Seen on hyperion with mvapich.
     */

    cbtf_rawdata_dir = getenv("CBTF_RAWDATA_DIR");
    sprintf(dir_path, "%s/cbtf-rawdata-%s-%d",
	    (cbtf_rawdata_dir != NULL) ? cbtf_rawdata_dir : "/tmp",
	     host,pid);

    char *exe_name = basename(executable_path);
    if(posix_tid == 0) {
	sprintf(tls->path, "%s/%s-%lld", dir_path, exe_name, pid);
	sprintf(tls->path, "%s.%s", tls->path, suffix);
    } else {
	sprintf(tls->path, "%s/%s-%lld-%llu", dir_path, exe_name, (int64_t)pid,(uint64_t)posix_tid);
	sprintf(tls->path, "%s.%s", tls->path, suffix);
    }


    if ( (getenv("CBTF_DEBUG_FILEIO_SERVICE") != NULL)) {
	fprintf(stderr,"CBTF_SetSendToFile ready for %s\n",tls->path);
    }

    /* Insure the directory path to contain the file exists */
    struct stat st;

#if 0
    /* this was the old method */
    if(stat(dir_path,&st) != 0) {
	mkdir(dir_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
#else

    if (stat(dir_path, &st) == 0 && S_ISDIR (st.st_mode)) {

        /* No need to make the directory.  It already exists. */
	if ( (getenv("CBTF_DEBUG_FILEIO_SERVICE") != NULL)) {
            fprintf(stderr,"CBTF_SetSendToFile pathname %s exists and is a directory\n",
		dir_path);
        }
       
    } else {
        /* The directory does not exist.
	 * Try to make the directory in this section of code.
	 * Use a while loop to test the status of mkdir in case
	 * it fails for some reason. On a cluster running nvidia-nmi
	 * under the watch command (run command by default every 2 secs)
	 * the mkdir on an NFS directory was interupted.
	 * TODO: if some threshold of trys is exceeded, abort.
	 */
        int status = -1;
        int save_errno = 0;
        int try_count = 0;
        while (status != 0 ) {
   	   status = mkdir(dir_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
           save_errno = errno;
           try_count = try_count + 1;
        }
	if ( (getenv("CBTF_DEBUG_FILEIO_SERVICE") != NULL)) {
            fprintf(stderr,"CBTF_SetSendToFile mkdir dir_path:%s status=%d errno:%d try_count:%d\n",
		dir_path, status, save_errno, try_count);
        }
    } 
        
#endif


    /* Insure the file itself exists */
    fd = open(tls->path, O_CREAT | O_APPEND,
	      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if(fd >= 0)
	close(fd);
}


void CBTF_EventSetSendToFile(CBTF_EventHeader* header, const char* unique_id,
			const char* suffix)
{
   __CBTF_SetSendToFile(header->host, header->pid, header->posix_tid,
			unique_id,suffix);
}

void CBTF_SetSendToFile(CBTF_DataHeader* header, const char* unique_id,
			const char* suffix)
{
   __CBTF_SetSendToFile(header->host, header->pid, header->posix_tid,
			unique_id,suffix);
}


/**
 * Send performance data to a file.
 *
 * Sends performance data to the current "send-to" file previously specified by
 * CBTF_SetSendToFile(). Any header generation and data encoding is performed
 * by the caller. Here the data is treated purely as a buffer of bytes to be
 * sent.
 *
 * @param size    Size of the data to be sent (in bytes).
 * @param data    Pointer to the data to be sent.
 * @return        Integer "1" if succeeded or "0" if failed.
 *
 * @ingroup RuntimeAPI
 */
int CBTF_SendToFile(const unsigned size, const void* data)
{
    unsigned encoded_size;
    char buffer[8]; /* Large enough to encode one 32-bit unsigned integer */
    XDR xdrs;
    int fd;

    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    Assert(tls != NULL);
    
    /* Create an XDR stream using the encoding buffer */
    xdrmem_create(&xdrs, buffer, sizeof(buffer), XDR_ENCODE);

    /* Encode the size of the data to be sent */
    Assert(xdr_u_int(&xdrs, (void*)&size) == TRUE);

    /* Get the encoded size */
    encoded_size = xdr_getpos(&xdrs);
    
    /* Close the XDR stream */
    xdr_destroy(&xdrs);

    /* Open the file for writing */
    Assert((fd = open(tls->path, O_WRONLY | O_APPEND)) >= 0);

    /* Write the size of the data to be sent */
    Assert(write(fd, buffer, encoded_size) == encoded_size);
    
    /* Write the data */
    Assert(write(fd, data, size) == size);

/* fsync call taken out due to slow processing time reported at LLNL */
/* via Matt Legendre for a LLNL user. */
#if 0
    /* Flush the data to disk. We could also test for fsyncdata
     * and use that as our fsync vi a #define.
     */
    Assert(fsync(fd) == 0);
#endif
    
    /* Close the file */
    Assert(close(fd) == 0);

    /* Indicate success to the caller */
    return 1;
}
