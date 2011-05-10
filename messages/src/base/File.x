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
 * Specification of the File communication protocol.
 *
 */



/**
 * File name.
 *
 * Names a single file by providing the full path name of that file. A checksum
 * is also provided, which allows the frontend to verify the availability of the
 * correct file.
 *
 * @note    Currently the exact type of checksum to be used here is left
 *          undefined. It is expected this checksum will be a CRC-64-ISO
 *          or similar.
 */
struct CBTF_Protocol_FileName
{
    /** Full path name of the file. */
    string path<>;

    /** Checksum calculated on this file. */
    uint64_t checksum;
};
