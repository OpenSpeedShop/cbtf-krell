/*******************************************************************************
** Copyright (c) 2011 The Krell Institue. All Rights Reserved.
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
 * Specification of the offline specific blobs.
 *
 */

/** Structure of the blob containing our file objects. */
struct CBTF_Protocol_Offline_LinkedObject {
    string   objname<>;   /** < Name of the object. */
    uint64_t addr_begin;  /** < begin address of object */
    uint64_t addr_end;    /** < end address of object */
    uint64_t time_begin;  /** < load time of object */
    uint64_t time_end;    /** < close time of object */
    uint8_t  is_open;     /** < flag to indicate dlopen or dlclose */
};

struct CBTF_Protocol_Offline_LinkedObjectGroup {
    CBTF_Protocol_Offline_LinkedObject linkedobjects<>;
};

/** Structure of the blob containing our process info. */
struct CBTF_Protocol_Offline_Parameters {
    string   collector<>;  /** < Name of the collector. */
    string   exename<>;    /** < Name of the executable. */
    string   traced<>;     /** < list of colon separated traceable functions.*/
    string   event<>;      /** < list of colon separated traceable events. */
    uint32_t rate;	   /** < rate or threshold parameter for this object */
};
