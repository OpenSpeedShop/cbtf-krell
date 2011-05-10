/*******************************************************************************
** Copyright (c) 2007 William Hachfeld. All Rights Reserved.
** Copyright (c) 2007-2011 The Krell Institute. All Rights Reserved.
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
 * Definition of the CBTF_InitializeDataHeader() function.
 *
 */

#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Messages/DataHeader.h"

#if !defined(CBTF_OFFLINE_SERVICE)
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#endif

#include <pthread.h>
#include <string.h>
#include <unistd.h>



/**
 * Initialize performance data header.
 *
 * Zeroes a performance data header and fills in the experiment and collector
 * identifiers as well as identifying information for the calling thread.
 *
 * @param experiment    Identifier of experiment to contain the data.
 * @param collector     Identifier of collector gathering data.
 * @param header        Performance data header to be initialized.
 */
void CBTF_InitializeDataHeader(int experiment, int collector,
			       CBTF_DataHeader* header)
{
    /* Check assertions */
    Assert(header != NULL);

    /* Zero the header */
    memset(header, 0, sizeof(CBTF_DataHeader));

    /* Fill in the specified experiment and collector identifiers */
    header->experiment = experiment;
    header->collector = collector;

    /* Fill in the name of the host containing this thread */
    Assert(gethostname(header->host, HOST_NAME_MAX) == 0);


#if !defined(CBTF_OFFLINE_SERVICE)
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_INET;
    hints.ai_protocol = PF_INET;

    struct addrinfo* results = NULL;
    getaddrinfo(header->host, NULL, &hints, &results);

    if((results != NULL) &&
       ( ntohl(((struct sockaddr_in*) (results->ai_addr))->sin_addr.s_addr)
	 == INADDR_LOOPBACK)) {
	freeaddrinfo(results);
	results = NULL;
	getaddrinfo(header->host, NULL, &hints, &results);
    }

    if(results != NULL) {
	if(results->ai_canonname != NULL)
	    strcpy(header->host,results->ai_canonname);
	freeaddrinfo(results);
    }
#endif

    /* Fill in the identifier of the process containing this thread */
    header->pid = getpid();

    /* Fill in the identifier of this thread */
    header->posix_tid = pthread_self();
}
