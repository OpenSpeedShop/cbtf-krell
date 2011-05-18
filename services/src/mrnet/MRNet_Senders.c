/*******************************************************************************
** Copyright (c) The Krell Institute. 2011  All Rights Reserved.
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
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rpc/rpc.h>
#include "mrnet_lightweight/MRNet.h"

#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/File.h"
#include "KrellInstitute/Messages/OfflineEvents.h"
#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
#include "KrellInstitute/Messages/Thread.h"
#include "KrellInstitute/Messages/ThreadEvents.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Data.h"
#include "KrellInstitute/Services/Timer.h"
#include "KrellInstitute/Services/TLS.h"

typedef struct {
    CBTF_Protocol_ThreadNameGroup tgrp;
    struct {
        CBTF_Protocol_ThreadName tnames[4096];
    } tgrpbuf;

} TLS;

#ifdef USE_EXPLICIT_TLS

/**
 *  Thread-local storage key.
 *
 *  Key used for looking up our thread-local storage. This key <em>must</em>
 *  be globally unique across the entire Open|SpeedShop code base.
 **/
static const uint32_t TLSKey = 0x0000AEF3;

#else

/** Thread-local storage. */
static __thread TLS the_tls;

#endif

void CBTF_create_ThreadName(CBTF_Protocol_ThreadName *tname,
			    CBTF_EventHeader header)
{
    tname->experiment = 0;
    tname->host = strdup(header.host);
    tname->pid = header.pid;
    tname->has_posix_tid = header.has_posix_tid;
    tname->posix_tid = header.posix_tid;
}

void CBTF_create_ThreadNameGroup(CBTF_Protocol_ThreadNameGroup *tgrp,
				 CBTF_Protocol_ThreadName tname)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
        return;
    tgrp->names.names_len = 0;
    tgrp->names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tgrp->names.names_len]),
           &tname, sizeof(tname));
    tgrp->names.names_len++;
}

void CBTF_send_CreatedProcess_message(CBTF_EventHeader header)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
        return;

    CBTF_EventHeader orig_header;
    CBTF_InitializeEventHeader(0, /* Experiment */
                                1, /* Collector */
                                &orig_header);
    //orig_header.host=strdup(header.host);
    orig_header.pid=-1;
    orig_header.posix_tid=0;
    orig_header.has_posix_tid=false;

    CBTF_Protocol_ThreadName origtname;
    CBTF_create_ThreadName(&origtname, orig_header);

    CBTF_Protocol_ThreadName tname;
    CBTF_create_ThreadName(&tname, header);

    CBTF_Protocol_CreatedProcess message;
    message.original_thread = origtname;
    message.created_thread = tname;

    tls->tgrp.names.names_len = 0;
    tls->tgrp.names.names_val = tls->tgrpbuf.tnames;
    memset(tls->tgrpbuf.tnames, 0, sizeof(tls->tgrpbuf.tnames));

    memcpy(&(tls->tgrpbuf.tnames[tls->tgrp.names.names_len]),
           &tname, sizeof(tname));
    tls->tgrp.names.names_len++;

     CBTF_MRNet_Send( CBTF_PROTOCOL_TAG_CREATED_PROCESS,
                           (xdrproc_t) xdr_CBTF_Protocol_CreatedProcess,
                          &message);
}

void CBTF_send_AttachedToThreads_message(CBTF_Protocol_ThreadNameGroup tgrp)
{
    /* Access our thread-local storage */
#ifdef USE_EXPLICIT_TLS
    TLS* tls = CBTF_GetTLS(TLSKey);
#else
    TLS* tls = &the_tls;
#endif
    if (tls == NULL)
        return;

    CBTF_Protocol_AttachedToThreads message;
    message.threads = tgrp;

    CBTF_MRNet_Send(CBTF_PROTOCOL_TAG_ATTACHED_TO_THREADS,
                    (xdrproc_t) xdr_CBTF_Protocol_AttachedToThreads,
                    &message);

}


void CBTF_send_ThreadStateChanged_message(CBTF_EventHeader header,
					  CBTF_Protocol_ThreadState state)
{
    CBTF_Protocol_ThreadName tname;
    CBTF_create_ThreadName(&tname, header);

    CBTF_Protocol_ThreadNameGroup tgrp;
    CBTF_create_ThreadNameGroup(&tgrp, tname);

    CBTF_Protocol_ThreadsStateChanged message;
    message.threads = tgrp;
    message.state = state;

    CBTF_MRNet_Send(CBTF_PROTOCOL_TAG_THREADS_STATE_CHANGED,
                   (xdrproc_t) xdr_CBTF_Protocol_ThreadsStateChanged, &message);
}

void CBTF_send_LoadedLinkedObject_message(CBTF_EventHeader header,
					  bool is_executable,
					  CBTF_Protocol_Offline_LinkedObject objects)
{
    CBTF_Protocol_ThreadName tname;
    CBTF_create_ThreadName(&tname, header);

    CBTF_Protocol_ThreadNameGroup tgrp;
    CBTF_create_ThreadNameGroup(&tgrp, tname);

    CBTF_Protocol_FileName dsoFilename;
    dsoFilename.path = strdup(objects.objname);

    CBTF_Protocol_LoadedLinkedObject message;
    memset(&message, 0, sizeof(message));

    message.threads = tgrp;
    message.linked_object = dsoFilename;
    message.time = objects.time_begin;
    message.range.begin =  objects.addr_begin;
    message.range.end = objects.addr_end;
    message.is_executable = is_executable;

    CBTF_MRNet_Send(CBTF_PROTOCOL_TAG_LOADED_LINKED_OBJECT,
                   (xdrproc_t) xdr_CBTF_Protocol_LoadedLinkedObject, &message);
}

void CBTF_send_UnloadedLinkedObject_message(CBTF_EventHeader header,
					    CBTF_Protocol_Offline_LinkedObject objects)
{
    CBTF_Protocol_ThreadName tname;
    CBTF_create_ThreadName(&tname, header);

    CBTF_Protocol_ThreadNameGroup tgrp;
    CBTF_create_ThreadNameGroup(&tgrp, tname);

    CBTF_Protocol_FileName dsoFilename;
    dsoFilename.path = strdup(objects.objname);

    CBTF_Protocol_UnloadedLinkedObject message;
    memset(&message, 0, sizeof(message));

    message.threads = tgrp;
    message.linked_object = dsoFilename;
    message.time = objects.time_begin;

    CBTF_MRNet_Send(CBTF_PROTOCOL_TAG_UNLOADED_LINKED_OBJECT,
                   (xdrproc_t) xdr_CBTF_Protocol_UnloadedLinkedObject, &message);
}
