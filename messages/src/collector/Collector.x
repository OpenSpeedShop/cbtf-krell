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
 * Specification of the Collector communication protocol.
 *
 */



/**
 * Collector identifier.
 *
 * Names a specific instance of a collector. To uniquely identify a collector,
 * the experiment and collector's unique identifiers must be specified.
 */
struct CBTF_Protocol_Collector
{
    /** Unique identifier for the experiment containing this collector. */
    int experiment;

    /** Identifer for this collector within that experiment. */
    int collector;
};
