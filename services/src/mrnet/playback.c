/*******************************************************************************
** Copyright (c) 2017 Argo Navis Technologies. All Rights Reserved.
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

/** @file Definition of the playback globals and functions. */

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Time.h"

#include "playback.h"

void CBTF_MRNet_LW_sendToFrontend(const int, const int, void*);



/** Is the recording of a playback enabled? */
static bool do_record_playback = false;

/** Is the replay of previous recorded playback enabled? */
static bool do_replay_playback = false;

/** Should the recorded delay between each message be ignored? */
static bool no_playback_delay = false;

/** Path of the playback file. */
static char playback_file[PATH_MAX] = "";



/**
 * Configure the playback mechanism.
 *
 * Determine if the sent messages should be recorded for future playback, or
 * replayed from an existing playback. And if so perform any necessary setup.
 *
 * @param rank    MRNet rank of this process.
 */
void playback_configure(uint32_t rank)
{
    do_record_playback = (getenv("CBTF_MRNET_RECORD_PLAYBACK") != NULL);
    do_replay_playback = (getenv("CBTF_MRNET_REPLAY_PLAYBACK") != NULL);
    no_playback_delay = (getenv("CBTF_MRNET_NO_PLAYBACK_DELAY") != NULL);
    
    if (do_record_playback && do_replay_playback)
    {
        do_record_playback = false;
        do_replay_playback = false;

        fprintf(stderr, "[CBTF/MRNet Playback] "
                "Cannot both record and replay a playback concurrently!\n");
        fflush(stderr);
    }
    else if (do_record_playback || do_replay_playback)
    {
        char* directory = do_record_playback ?
            getenv("CBTF_MRNET_RECORD_PLAYBACK") :
            getenv("CBTF_MRNET_REPLAY_PLAYBACK");
        
        struct stat status;
        int retval = stat(directory, &status);

        if ((retval == -1) || (!S_ISDIR(status.st_mode)))
        {
            do_record_playback = false;
            do_replay_playback = false;

            fprintf(stderr, "[CBTF/MRNet Playback] "
                    "Playback directory \"%s\" couldn't be accessed!\n",
                    directory);
            fflush(stderr);
        }
        else
        {
            sprintf(playback_file, "%s/cbtf-mrnet-playback-%u",
                    directory, rank);

            retval = stat(playback_file, &status);

            if (do_record_playback &&
                ((retval == 0) || (errno != ENOENT)))
            {
                do_record_playback = false;

                fprintf(stderr, "[CBTF/MRNet Playback] "
                        "Playback file \"%s\" already exists!\n",
                        playback_file);
                fflush(stderr);
            }
            
            if (do_replay_playback &&
                ((retval == -1) || !S_ISREG(status.st_mode)))
            {
                do_replay_playback = false;
                
                fprintf(stderr, "[CBTF/MRNet Playback] "
                        "Playback file \"%s\" couldn't be opened!\n",
                        playback_file);
                fflush(stderr);
            }
        }
    }
}



/**
 * Record the specified message to the previously configured playback file.
 *
 * @param tag     Tag for the message.
 * @param size    Size (in bytes) of the message.
 * @param data    Pointer to the message contents.
 */
static void record_to_playback(int32_t tag, uint32_t size, void* data)
{
    uint64_t time = CBTF_GetTime();
    FILE* fp = fopen(playback_file, "a+");

    Assert(fp != NULL);

    Assert(fwrite(&time, sizeof(uint64_t), 1, fp) == 1);
    Assert(fwrite(&tag, sizeof(int32_t), 1, fp) == 1);
    Assert(fwrite(&size, sizeof(uint32_t), 1, fp) == 1);
    Assert(fwrite(data, size, 1, fp) == 1);
    
    Assert(fclose(fp) == 0);
}



/**
 * Replay all messages from the previously configured playback file.
 */
static void replay_from_playback()
{
    bool is_first = true;
    uint64_t previous_time = 0;
    FILE* fp = fopen(playback_file, "r");

    Assert(fp != NULL);

    while (true)
    {
        uint64_t time = 0;
        int32_t tag = 0;
        uint32_t size = 0;

        if (fread(&time, sizeof(uint64_t), 1, fp) < 1)
        {
            break;
        }

        if (fread(&tag, sizeof(int32_t), 1, fp) < 1)
        {
            break;
        }

        if (fread(&size, sizeof(uint32_t), 1, fp) < 1)
        {
            break;
        }

        void* data = malloc(size);

        if (fread(data, size, 1, fp) < 1)
        {
            free(data);
            break;
        }

        if (!is_first && !no_playback_delay)
        {
            usleep((time - previous_time) / 1000 /* us/ns */);
        }

        is_first = false;
        previous_time = time;

        CBTF_MRNet_LW_sendToFrontend(tag, (int)size, data);
        free(data);
    }
        
    Assert(fclose(fp) == 0);
}



/**
 * Inform the playback mechanism of a message to be sent.
 *
 * Called prior to sending each message. May record the message, replay from
 * an existing playback, or nothing, depending on how playback was configured.
 *
 * @param tag     Tag for the message.
 * @param size    Size (in bytes) of the message.
 * @param data    Pointer to the message contents.
 *
 * @return        Boolean flag indicating if the actual sending of this
 *                message should be squelched because replay is enabled.
 */
bool playback_intercept(int32_t tag, uint32_t size, void* data)
{
    /** Is this the first message to be intercepted? */
    static bool is_first_message = true;

    /** Should sends now be squelched? */
    static bool do_squelch_message = false;

    /* Record this message to a playback file if requested. */
    if (do_record_playback)
    {
        record_to_playback(tag, (uint32_t)size, data);

        return false; /* Don't squelch the actual sending of this message. */
    }

    /*
     * Squelch the actual sending of this message if replay of a playback file
     * has been requested. But if this is the first message, do the replay too.
     * Things get slightly complicated because replay_from_playback() must use
     * CBTF_MRNet_LW_sendToFrontend() to send its messages, and that function 
     * is going to recursively call into here again...
     */
    if (do_replay_playback)
    {
        if (is_first_message)
        {
            is_first_message = false;
            replay_from_playback();
            do_squelch_message = true;
        }
        
        if (do_squelch_message)
        {
            return true; /* Squelch the actual sending of this message. */
        }
    }
    
    return false; /* Don't squelch the actual sending of this message. */
}
