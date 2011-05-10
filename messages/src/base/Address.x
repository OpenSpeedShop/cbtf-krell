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
 * Specification of the Address communication protocol.
 *
 */



/**
 * Memory address.
 *
 * All memory addresses are represented using a 64-bit unsigned integer.
 * This allows for a unified representation of both 32-bit and 64-bit
 * address spaces by sacrificing storage space when 32-bit addresses are
 * processed. Various overflow and underflow conditions are checked when
 * arithmetic operations are performed on these addresses.
 */
typedef uint64_t CBTF_Protocol_Address;



/**
 * Range of memory addresses.
 *
 * A single, closed-ended, range of memory addresses: [begin, end]. Used in
 * several different places for representing a single contiguous portion of
 * an address space, as occupied by a DSO, compilation unit, function, etc.
 */
struct CBTF_Protocol_AddressRange
{
    CBTF_Protocol_Address begin;  /**< Beginning of the address range. */
    CBTF_Protocol_Address end;    /**< End of the address range. */
};
