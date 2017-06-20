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

/** @file Declaration of the playback globals and functions. */

#pragma once

#include <stdint.h>

/*
 * Configure the playback mechanism.
 *
 * Determine if the sent messages should be recorded for future playback, or
 * replayed from an existing playback. And if so perform any necessary setup.
 *
 * @param rank    MRNet rank of this process.
 */
void playback_configure(uint32_t rank);

/*
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
bool playback_intercept(int32_t tag, uint32_t size, void* data);
