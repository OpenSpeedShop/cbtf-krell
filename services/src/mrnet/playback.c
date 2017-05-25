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
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <rpc/xdr.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "KrellInstitute/Messages/LinkedObjectEvents.h"
#include "KrellInstitute/Messages/ToolMessageTags.h"
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

/** Mutex insuring only one thread enters playback_intercept() at a time. */
static pthread_mutex_t playback_mutex;
static pthread_mutexattr_t playback_mutexattr;

/** Path of the playback directory. */
static char playback_directory[PATH_MAX] = "";

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
        strcpy(playback_directory, do_record_playback ?
               getenv("CBTF_MRNET_RECORD_PLAYBACK") :
               getenv("CBTF_MRNET_REPLAY_PLAYBACK"));
        
        struct stat status;
        int retval = stat(playback_directory, &status);

        if ((retval == -1) || (!S_ISDIR(status.st_mode)))
        {
            do_record_playback = false;
            do_replay_playback = false;

            fprintf(stderr, "[CBTF/MRNet Playback] "
                    "Playback directory \"%s\" couldn't be accessed!\n",
                    playback_directory);
            fflush(stderr);
        }
        else
        {
            sprintf(playback_file, "%s/cbtf-mrnet-playback-%u",
                    playback_directory, rank);

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

    pthread_mutexattr_init(&playback_mutexattr);
    pthread_mutexattr_settype(&playback_mutexattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&playback_mutex, &playback_mutexattr);
}



/**
 * Equivalent of "mkdir -p", but ignoring the last path component, which is
 * assumed to be a filename. Taken (with modifications) from the link below.
 *
 * @param path    Path of the file for whom all parent directories are created.
 * @return        Integer indicating success (0) or failure (-1).
 * 
 * @sa https://gist.github.com/JonathonReinhart/8c0d90191c38af2dcadb102c4e202950
 */
static int mkpath(const char* path)
{
    errno = 0;

    const size_t length = strlen(path);
    char temp[PATH_MAX];

    if (length > (sizeof(temp) - 1))
    {
        errno = ENAMETOOLONG;
        return -1;
    }
    
    strcpy(temp, path);

    char *ptr;
    for (ptr = temp + 1; *ptr != '\0'; ptr++)
    {
        if (*ptr == '/')
        {
            *ptr = '\0';

            if (mkdir(temp, S_IRWXU) != 0)
            {
                if (errno != EEXIST)
                {
                    return -1;
                }
            }

            *ptr = '/';
        }
    }

    return 0;
}



/**
 * Copy the specified file to a location within the playback directory that
 * includes the original file's full path. This is done atomically such that
 * multiple processes may copy the same file and only one, consistent, copy
 * will be kept.
 *
 * @param source    Path of the file to be copied.
 */
void copyfile(const char* source)
{
    int src = open(source, O_RDONLY);

    Assert(src != -1);
    
    char temp[PATH_MAX] = "";
    sprintf(temp, "%s/XXXXXX", playback_directory);
    int dst = mkstemp(temp);

    Assert(dst != -1);

    while (true)
    {
        static char buffer[8192];
        
        ssize_t n = read(src, buffer, sizeof(buffer));

        if (n == 0)
        {
            break;
        }

        Assert(write(dst, buffer, n) == n);
    }

    Assert(close(src) == 0);
    Assert(close(dst) == 0);

    char destination[PATH_MAX] = "";
    sprintf(destination, "%s%s", playback_directory, source);
    
    Assert(mkpath(destination) == 0);
    Assert(rename(temp, destination) == 0);
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

    if (tag == CBTF_PROTOCOL_TAG_LINKED_OBJECT_GROUP)
    {
        XDR xdr;
        CBTF_Protocol_LinkedObjectGroup group;

        xdrmem_create(&xdr, data, size, XDR_DECODE);
        memset(&group, 0, sizeof(CBTF_Protocol_LinkedObjectGroup));
        xdr_CBTF_Protocol_LinkedObjectGroup(&xdr, &group);
        xdr_destroy(&xdr);

        int i;
        for (i = 0; i < group.linkedobjects.linkedobjects_len; ++i)
        {
            copyfile(
                group.linkedobjects.linkedobjects_val[i].linked_object.path
                );
        }

        xdr_free((xdrproc_t)xdr_CBTF_Protocol_LinkedObjectGroup,
                 (char*)&group);
    }
    
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

        if (tag == CBTF_PROTOCOL_TAG_LINKED_OBJECT_GROUP)
        {
            XDR xdr;
            CBTF_Protocol_LinkedObjectGroup group;
            
            xdrmem_create(&xdr, data, size, XDR_DECODE);
            memset(&group, 0, sizeof(CBTF_Protocol_LinkedObjectGroup));
            xdr_CBTF_Protocol_LinkedObjectGroup(&xdr, &group);
            xdr_destroy(&xdr);

            int i;
            for (i = 0; i < group.linkedobjects.linkedobjects_len; ++i)
            {
                char** original = &(
                    group.linkedobjects.linkedobjects_val[i].linked_object.path
                    );
                
                char rehomed[PATH_MAX] = "";
                sprintf(rehomed, "%s%s", playback_directory, *original);

                free(*original);
                *original = strdup(rehomed);
            }

            free(data);
            size *= 2; /* Should be more than enough for re-encoding */
            data = malloc(size);
            
            xdrmem_create(&xdr, data, size, XDR_ENCODE);
            xdr_CBTF_Protocol_LinkedObjectGroup(&xdr, &group);
            size = xdr_getpos(&xdr);
            xdr_destroy(&xdr);

            xdr_free((xdrproc_t)xdr_CBTF_Protocol_LinkedObjectGroup,
                     (char*)&group);
        }
        
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
    static bool is_first_message = true;    /* First message? */
    static bool do_squelch_message = false; /* Squelch sends now? */

    Assert(pthread_mutex_lock(&playback_mutex) == 0);
        
    /* Record this message to a playback file if requested. */
    if (do_record_playback)
    {
        record_to_playback(tag, (uint32_t)size, data);
        Assert(pthread_mutex_unlock(&playback_mutex) == 0);
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
            Assert(pthread_mutex_unlock(&playback_mutex) == 0);
            return true; /* Squelch the actual sending of this message. */
        }
    }
    
    Assert(pthread_mutex_unlock(&playback_mutex) == 0);
    return false; /* Don't squelch the actual sending of this message. */
}
