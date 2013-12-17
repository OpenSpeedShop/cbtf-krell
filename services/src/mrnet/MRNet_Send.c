/*******************************************************************************
** Copyright (c) The Krell Institute. 2011-2013  All Rights Reserved.
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
 * Definition of the MRNET LW send functions.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//#include "KrellInstitute/CBTF/Impl/MessageTags.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/EventHeader.h"
#include "KrellInstitute/Messages/Blob.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"

#include <rpc/rpc.h>
#include "mrnet_lightweight/MRNet.h"


Network_t* CBTF_MRNet_netPtr;
// make the id of the stream we want global and use Network_get_Stream
// locally in the send function to retrieve it.
//Stream_t* CBTF_MRNet_stream;
static int stream_id = 0;
static int mrnet_connected = 0;


static int CBTF_MRNet_getParentInfo(const char* file, int rank, char* phost, char* pport, char* prank)
{
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
	fprintf(stderr,"ENTER CBTF_MRNet_getParentInfo with rank %d\n",rank);
    }
#endif

    FILE *cfile;

    cfile = fopen(file, "r");

    if( cfile ) {
        while( !feof(cfile) ) {
            char line[256];
            if(fgets(line, sizeof(line), cfile) == 0) {
                break;
            }

            char pname[64];
            int tpport, tprank, trank;
            int matches = sscanf( line, "%s %d %d %d",
                                  pname, &tpport, &tprank, &trank );
            if( matches != 4 ) {
                fprintf(stderr, "CBTF_MRNet_getParentInfo: Error while scanning %s\n", file);
                fclose(cfile);;
                return 1;
            }

            if( trank == rank ) {
                sprintf(phost, "%s", pname);
                sprintf(pport, "%d", tpport);
                sprintf(prank, "%d", tprank);
                fclose(cfile);;

#ifndef NDEBUG
		if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
                    fprintf(stderr,"EXIT CBTF_MRNet_getParentInfo with rank %d, phost %s, pport %s, prank %s\n",
                        rank,phost,pport,prank);
		}
#endif
                return 0;
            }

        }
        fclose(cfile);;
    }

    return 1;
}

int CBTF_MRNet_LW_connect (const int con_rank)
{
    if (mrnet_connected) {
        return;
    }

    const char* connfile = getenv("CBTF_MRNETBE_CONNECTIONS");
    if (connfile == NULL) {
	const char* connections_dir = getenv("PWD");
	char buf[4096];
	//sprintf(buf,"%s%s",connections_dir,"/.cbtf/attachBE_connections");
	sprintf(buf,"%s%s",connections_dir,"/attachBE_connections");
	connfile = strdup(buf);
    }

    char parHostname[64], myHostname[64], parPort[10], parRank[10], myRank[10];

    /* sequential jobs pass con_rank of -1. In that case we need to
     * use a rank of 0 and mRank of 10000 for the connection.
     */
    Rank mRank;
    int rank_to_use = con_rank;

    if (con_rank < 0 ) {
	mRank = 10000;
	rank_to_use = 0;
    } else {
	mRank = 10000 + con_rank;
    }

    sprintf(myRank, "%d", mRank);


    if( CBTF_MRNet_getParentInfo(connfile, rank_to_use, parHostname, parPort, parRank) != 0 ) {
	fprintf(stderr, "CBTF_MRNet_LW_connect: Failed to parse connections file\n");
	fprintf(stderr, "CBTF_MRNet_LW_connect: myRank %s, mrank %d, con_rank %d\n",myRank,mRank,rank_to_use);
        return -1;
    }

    while( gethostname(myHostname, 64) == -1 ) {}
    myHostname[63] = '\0';

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
	fprintf(stderr, "CBTF_MRNet_LW_connect: myRank %s, mRank %d, host %s\n",myRank,mRank,myHostname);
    }
#endif

    int BE_argc = 6;
    char* BE_argv[6];


    char be_name[24];
    sprintf(be_name,"%s%s","cbtfBE",myRank);

    BE_argv[0] = be_name;
    BE_argv[1] = parHostname;
    BE_argv[2] = parPort;
    BE_argv[3] = parRank;
    BE_argv[4] = myHostname;
    BE_argv[5] = myRank;

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
	fprintf(stderr,"CBTF_MRNet_LW_connect: argv 0=%s, 1=%s, 2=%d, 3=%d, 4=%s, 5=%d\n",
        BE_argv[0], BE_argv[1], strtoul( BE_argv[2], NULL, 10 ),
        strtoul( BE_argv[3], NULL, 10 ), BE_argv[4], strtoul( BE_argv[5], NULL, 10 ));

	fprintf( stderr, "CBTF_MRNet_LW_connect: Backend %s[%s] connecting to %s:%s[%s]\n",
        myHostname,myRank,parHostname,parPort,myRank);
    }
#endif

    Packet_t * p;
    p = (Packet_t *)malloc(sizeof(Packet_t));
    Assert(p);

    CBTF_MRNet_netPtr = Network_CreateNetworkBE(BE_argc, BE_argv);
    Assert(CBTF_MRNet_netPtr);

    int tag;
    const char* fmt_str = "%d";

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
        fprintf(stderr, "CBTF_MRNet_LW_connect:  TRYING TO ESTABLISH CBTF_MRNet_stream\n");
    }
#endif

    Stream_t* CBTF_MRNet_stream;

    if (Network_recv(CBTF_MRNet_netPtr, &tag, p, &CBTF_MRNet_stream) != 1) {
        fprintf(stderr, "CBTF_MRNet_LW_connect: BE receive failure\n");
	abort();
    }

    stream_id = CBTF_MRNet_stream->id;
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
        fprintf(stderr,
	"CBTF_MRNet_LW_connect: got tag %d, stream id %d, sync_filter_id %d, us_filter_id %d, ds_filter_id %d\n",
	tag, CBTF_MRNet_stream->id,CBTF_MRNet_stream->sync_filter_id,
	CBTF_MRNet_stream->us_filter_id, CBTF_MRNet_stream->ds_filter_id);
    }
#endif

    /*  wait for FE to notify netowrk is ready 
     *  Use the SpecifyFilter tag for this since once all commnodes
     *  have loaded a filter, this message is passed down to the BE.
     *  This holds the collectors from continuing until any needed
     *  filter is loaded above.
     */
    if (CBTF_MRNet_netPtr) {
	/* get our stream */
	Stream_t* stream = Network_get_Stream(CBTF_MRNet_netPtr,stream_id);
	int readytag = 0;
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
	    fprintf(stderr, "BE: waiting for specify filter tag from FE\n");
	}
#endif
	do {
	    if( CBTF_MRNet_netPtr && Network_recv(CBTF_MRNet_netPtr, &readytag, p, &stream) != 1 ) {
		fprintf(stderr, "BE: receive failure\n");
		break;
	    }
	} while ( readytag != 105 );
#ifndef NDEBUG
	if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
	    fprintf(stderr, "BE: GOT TAG for filter tag %d from FE\n",readytag);
	}
#endif
     }

    mrnet_connected = 1;

    if (p != NULL) {
        free(p);
    } 
}

static void CBTF_MRNet_LW_sendToFrontend(const int tag, const int size, void *data)
{
    const char* fmt_str = "%auc";

#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
	fprintf(stderr,"CBTF_MRNet_LW_sendToFrontend: sends message with tag %d\n",tag);
    }
#endif

    Stream_t* CBTF_MRNet_stream = Network_get_Stream(CBTF_MRNet_netPtr,stream_id);
    if ( (Stream_send(CBTF_MRNet_stream, tag, fmt_str, data, size) == -1) ||
          Stream_flush(CBTF_MRNet_stream) == -1 ) {
        fprintf(stderr, "BE: stream::send() failure\n");
    }


    fflush(stdout);
    fflush(stderr);
}

void CBTF_MRNet_Send(const int tag,
                 const xdrproc_t xdrproc, const void* data)
{
#ifndef NDEBUG
    if (getenv("CBTF_DEBUG_LW_MRNET") != NULL) {
	fprintf(stderr,"CBTF_MRNet_Send: sends message with tag %d\n",tag);
    }
#endif
    unsigned size,dm_size;
    char* dm_contents = NULL;

    for(size = 1024; dm_contents == NULL; size *= 2) {
        char* buffer = alloca(size);
        XDR xdrs;
        xdrmem_create(&xdrs, buffer, size, XDR_ENCODE);


        if ((*xdrproc)(&xdrs, (void*)data) == TRUE) {
            dm_size = xdr_getpos(&xdrs);
            dm_contents = buffer;
        }
        xdr_destroy(&xdrs);
    }

    CBTF_MRNet_LW_sendToFrontend(tag ,dm_size , (void *) dm_contents);
}

void CBTF_MRNet_Send_PerfData(const CBTF_DataHeader* header,
                 const xdrproc_t xdrproc, const void* data)
{
    const size_t EncodingBufferSize = (CBTF_BlobSizeFactor * 15 * 1024);
    unsigned size;
    char* buffer = NULL;
    XDR xdrs;

    /* Check preconditions */
    Assert(header != NULL);
    Assert(xdrproc != NULL);
    Assert(data != NULL);

    buffer = alloca(EncodingBufferSize);

    xdrmem_create(&xdrs, buffer, EncodingBufferSize, XDR_ENCODE);
    Assert(xdr_CBTF_DataHeader(&xdrs, (void*)header) == TRUE);
    Assert((*xdrproc)(&xdrs, (void*)data) == TRUE);
    size = xdr_getpos(&xdrs);
    xdr_destroy(&xdrs);

    /* send it as a blob */
    CBTF_Protocol_Blob blob;
    blob.data.data_val = buffer;
    blob.data.data_len = size;

    CBTF_MRNet_Send(CBTF_PROTOCOL_TAG_PERFORMANCE_DATA,
                 (xdrproc_t) xdr_CBTF_Protocol_Blob, &blob);
}

void CBTF_Waitfor_MRNet_Shutdown() {

    Packet_t * p;
    p = (Packet_t *)malloc(sizeof(Packet_t));
    Assert(p);

    if (CBTF_MRNet_netPtr && Network_is_ShutDown(CBTF_MRNet_netPtr)) {
	//fprintf(stderr,"CBTF_Waitfor_MRNet_Shutdown -- already SHUTDOWN %d\n",getpid());
	return;
    }


    if (CBTF_MRNet_netPtr) {
	/* get our stream */
	Stream_t* stream = Network_get_Stream(CBTF_MRNet_netPtr,stream_id);
	int tag = 0;

	/* wait for FE to request the shutdown */
	do {
	    //fprintf(stderr,"CBTF_Waitfor_MRNet_Shutdown WAIT FOR FE request of shutdown %d\n",getpid());
	    if( CBTF_MRNet_netPtr && Network_recv(CBTF_MRNet_netPtr, &tag, p, &stream) != 1 ) {
		fprintf(stderr, "BE: receive failure\n");
		break;
	    }
	} while ( tag != 101 );


	/* send the FE the acknowledgement of shutdown */
	//fprintf(stderr,"CBTF_Waitfor_MRNet_Shutdown SENDS FE acknowledgement of shutdown %d\n",getpid());
	if ( (Stream_send(stream, /*tag*/ 102, "%d", 102) == -1) ||
	      Stream_flush(stream) == -1 ) {
	    fprintf(stderr, "CBTF_Waitfor_MRNet_Shutdown BE: stream::send() failure\n");
	}

	/* Now wait for mrnet to shutdown */
	if (CBTF_MRNet_netPtr && !Network_is_ShutDown(CBTF_MRNet_netPtr)) {
	    //fprintf(stderr,"CBTF_Waitfor_MRNet_Shutdown Network_waitfor_ShutDown %d\n",getpid());
	    Network_waitfor_ShutDown(CBTF_MRNet_netPtr);
        }

    }

    /* delete out network pointer */
    if (CBTF_MRNet_netPtr != NULL) {
	//fprintf(stderr,"CBTF_Waitfor_MRNet_Shutdown delete_Network_t CBTF_MRNet_netPtr %d\n",getpid());
        delete_Network_t(CBTF_MRNet_netPtr);
    }

    if (p != NULL)
	free(p);
    //fprintf(stderr,"EXIT CBTF_Waitfor_MRNet_Shutdown %d\n",getpid());
}

