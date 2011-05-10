/*******************************************************************************
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2011 Krell Institute. All Rights Reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 59 Temple
** Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Specification of the blob communication protocol.
 *
 */



/**
 * Binary large object.
 *
 * Encapsulates a buffer of raw, untyped, binary data. Used to store arguments
 * to functions called by instrumentation.
 *
 * @sa    http://www.hyperdictionary.com/computing/binary+large+object
 */
struct CBTF_Protocol_Blob
{
    /** Binary data in the blob. */
    uint8_t data<>;
};
