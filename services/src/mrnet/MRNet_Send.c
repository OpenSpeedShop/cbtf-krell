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
 * Definition of the MRNET LW send functions.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Messages/DataHeader.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"

#include <rpc/rpc.h>
#include "mrnet_lightweight/MRNet.h"


Network_t* CBTF_MRNet_netPtr;
Stream_t* CBTF_MRNet_upstream;
Packet_t *CBTF_MRNet_packet;

static int mrnet_connected = 0;

int CBTF_MRNet_getParentInfo(const char* file, int rank, char* phost, char* pport, char* prank)
{
    fprintf(stderr,"ENTER CBTF_MRNet_getParentInfo with rank %d\n",rank);

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

            fprintf(stderr,"CBTF_MRNet_getParentInfo found match: %s, %d, %d, %d\n",pname, tpport, tprank, trank);

            if( trank == rank ) {
                sprintf(phost, "%s", pname);
                sprintf(pport, "%d", tpport);
                sprintf(prank, "%d", tprank);
                fclose(cfile);;

                fprintf(stderr,"EXIT CBTF_MRNet_getParentInfo with rank %d, phost %s, pport %s, prank %s\n",
                        rank,phost,pport,prank);
                return 0;
            }

        }
        fclose(cfile);;
    }

    return 1;
}

int CBTF_MRNet_LW_connect (int con_rank)
{
    if (mrnet_connected) {
        return;
    }

    const char* connfile = getenv("CBTF_MRNETBE_CONNECTIONS");
    if (connfile == NULL) {
        connfile = "/home/dpm/.openspeedshop/attachBE_connections";
    }

    char parHostname[64], myHostname[64], parPort[10], parRank[10], myRank[10];
    int wRank = 0;
    if (con_rank > 0) {
	wRank = con_rank;
    }

    Rank mRank = 10000 + wRank;
    sprintf(myRank, "%d", mRank);

    fprintf(stderr, "CBTF_MRNet_LW_connect: myRank = %s, mRank = %d\n",myRank,mRank);

    if( CBTF_MRNet_getParentInfo(connfile, wRank, parHostname, parPort, parRank) != 0 ) {
        fprintf(stderr, "CBTF_MRNet_LW_connect: Failed to parse connections file\n");
        return -1;
    }

    while( gethostname(myHostname, 64) == -1 ) {}
    myHostname[63] = '\0';

    fprintf(stderr, "CBTF_MRNet_LW_connect: WE ARE ON HOSTNAME %s\n",myHostname);
    int BE_argc = 6;
    char* BE_argv[6];

    BE_argv[0] = (char *)malloc(strlen("")*sizeof(char));
    BE_argv[0] = strcpy(BE_argv[0],"");
    BE_argv[1] = parHostname;
    BE_argv[2] = parPort;
    BE_argv[3] = parRank;
    BE_argv[4] = myHostname;
    BE_argv[5] = myRank;

    fprintf(stderr,"CBTF_MRNet_LW_connect: argv 0=%s, 1=%s, 2=%d, 3=%d, 4=%s, 5=%d\n",
        BE_argv[0], BE_argv[1], strtoul( BE_argv[2], NULL, 10 ),
        strtoul( BE_argv[3], NULL, 10 ), BE_argv[4], strtoul( BE_argv[5], NULL, 10 ));

    fprintf( stderr, "CBTF_MRNet_LW_connect: Backend %s[%s] connecting to %s:%s[%s]\n",
        myHostname,myRank,parHostname,parPort,myRank);

    CBTF_MRNet_netPtr = Network_CreateNetworkBE(BE_argc, BE_argv);
    Assert(CBTF_MRNet_netPtr);

    uint64_t recv_int=0;
    int tag;
    const char* fmt_str = "%d";

    CBTF_MRNet_packet = (Packet_t *)malloc(sizeof(Packet_t));

    if (Network_recv(CBTF_MRNet_netPtr, &tag, CBTF_MRNet_packet, &CBTF_MRNet_upstream) != 1) {
        fprintf(stderr, "CBTF_MRNet_LW_connect: BE receive failure\n");
	abort();
    }

    fprintf(stderr,"CBTF_MRNet_LW_connect: CONNECTED TO MRNET network\n");
    mrnet_connected = 1;
}

void CBTF_MRNet_LW_sendToFrontend(const int tag, const int size, void *data)
{
    const char* fmt_str = "%auc";

    fprintf(stderr, "CBTF_MRNet_LW_sendToFrontend BE: tag is %d\n", tag);
    if ( (Stream_send(CBTF_MRNet_upstream, tag, fmt_str, data, size) == -1) ||
          Stream_flush(CBTF_MRNet_upstream) == -1 ) {
        fprintf(stderr, "BE: stream::send() failure\n");
    }

    fflush(stdout);
    fflush(stderr);
}

void CBTF_MRNet_Send(const int tag,
                 const xdrproc_t xdrproc, const void* data)
{
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
        if(dm_contents == NULL)
            free(buffer);
    }

    fprintf(stderr,"CBTF_MRNet_Send calls CBTF_MRNet_LW_sendToFrontend: with tag %d\n",tag);
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

    fprintf(stderr,"CBTF_MRNet_Send_PerfData calls CBTF_MRNet_LW_sendToFrontend\n");
    CBTF_MRNet_LW_sendToFrontend(CBTF_PROTOCOL_TAG_PERFORMANCE_DATA ,size , (void *) buffer);
}
