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
 * Specification of the Symbol communication protocol.
 *
 */

%#include "Address.h"
%#include "Blob.h"
%#include "File.h"



/**
 * Address bitmap.
 *
 * A bitmap containing one bit per address within an address range. Used to
 * describe which addresses within an address range are actually attributable
 * to a function or source statement.
 */
struct CBTF_Protocol_AddressBitmap
{
    /** Address range covered by this bitmap. */
    CBTF_Protocol_AddressRange range;

    /** Actual bitmap. */
    CBTF_Protocol_Blob bitmap;
};



/**
 * Function entry.
 *
 * Describes a single function in a symbol table. Contains the name of the
 * function and a description of the address ranges occupied by the function.
 */
struct CBTF_Protocol_FunctionEntry
{
    /** Name of this function. */
    string name<>;
    
    /** Address bitmaps describing the occupied address ranges. */
    CBTF_Protocol_AddressBitmap bitmaps<>;
};



/**
 * Statement entry.
 *
 * Describes a single source statement in a symbol table. Contains the name
 * of the source file, line number, and column number of the statement. Also
 * contains a description of the address ranges occupied by the statement.
 */
struct CBTF_Protocol_StatementEntry
{
    /** Name of this statement's source file. */
    CBTF_Protocol_FileName path;

    /** Line number of this statement. */
    int line;

    /** Column number of this statement. */
    int column;

    /** Address bitmaps describing the occupied address ranges. */
    CBTF_Protocol_AddressBitmap bitmaps<>;
};



/**
 * Symbol table.
 *
 * Issued by a backend to provide the frontend with the symbol table for a
 * single linked object.
 *
 * @note    All addresses in the symbol table are relative to the base load
 *          address of the linked object, allowing this data to be easily
 *          reused for multiple threads which may have loaded the linked
 *          object at differing addresses.
 */
struct CBTF_Protocol_SymbolTable
{
    /** Name of the linked object's file. */
    CBTF_Protocol_FileName linked_object;

    /** Functions contained in this linked object. */
    CBTF_Protocol_FunctionEntry functions<>;

    /** Statements contained in this linked object. */
    CBTF_Protocol_StatementEntry statements<>;
};
