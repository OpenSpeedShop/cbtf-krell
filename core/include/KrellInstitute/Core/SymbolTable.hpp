////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2006-2013 The Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Declaration of the SymbolTable class.
 *
 */

#ifndef _KrellInstitute_Core_SymbolTable_
#define _KrellInstitute_Core_SymbolTable_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//#include "KrellInstitute/Core/AddressBitmap.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"
#include "KrellInstitute/Messages/File.h"
#include "KrellInstitute/Messages/Symbol.h"

#include <map>
#include <set>
#include <string>
#include <vector>



namespace KrellInstitute { namespace Core {

    class Address;
    class AddressBitmap;
    class LinkedObjectEntry;
    class Path;

    /**
     * Symbol table.
     * 
     * The symbol table information for linked objects must be processed a fair
     * amount before it is stored into the database: overlapping and redundant
     * entries must be removed; source statement information is scattered across
     * the linked object and must be condensed; and finally the formatting of
     * the information in the database is signficantly different from the form
     * in which it is typically found. This class encapsulates these various
     * processing steps.
     *
     * @ingroup Implementation
     */
    class SymbolTable
    {
	
    public:
	
	SymbolTable(const AddressRange&);
	
	void addFunction(const Address&, const Address&, const std::string&);
	void addFunction(const Address&, const Address&, const Address&, const std::string&);
	void addStatement(const Address&, const Address&,
			  const Path&, const int&, const int&);
	
	void processAndStore(const LinkedObjectEntry&);

	AddressRange getAddressRange() { return dm_range; };
	
	std::map<AddressRange, std::string> getFunctions()
		{ return dm_functions;};

	operator CBTF_Protocol_SymbolTable() const;

	
    private:
        static std::vector<KrellInstitute::Core::AddressBitmap>
	partitionAddressRanges(const std::vector<AddressRange>&);

	static std::vector<std::set<Address> >
	partitionAddressSet(const std::set<Address>&);
	static void convert(const std::vector<KrellInstitute::Core::AddressBitmap>&,
                            u_int&, CBTF_Protocol_AddressBitmap*&);
	
	/** Address range occupied by this symbol table. */
	AddressRange dm_range;

        /** Table mapping function names to their address ranges. */
        typedef std::map<std::string, std::vector<AddressRange> > FunctionTable;

        /** Functions in this symbol table. */
        FunctionTable table_functions;

	/** Functions in this symbol table. */
	std::map<AddressRange, std::string> dm_functions;
	
	/**
	 * Statement entry.
	 *
	 * Structure for a source statement's entry in the symbol table's
	 * internal tables. Contains the full path name, line number, and column
	 * number of the source statement.
	 */
	struct StatementEntry
	{
	    
	    Path dm_path;   /**< Path name of this statement's source file. */
	    int dm_line;    /**< Line number of this statement. */
	    int dm_column;  /**< Column number of this statement. */
	    
	    /** Constructor from fields. */
	    StatementEntry(const Path& path, 
			   const int& line, 
			   const int& column) :
		dm_path(path),
		dm_line(line),
		dm_column(column)
	    {
	    }

	    /** Operator "<" defined for two StatementEntry objects. */
	    bool operator<(const StatementEntry& other) const
	    {
		if(dm_path < other.dm_path)
		    return true;
		if(dm_path > other.dm_path)
		    return false;
		if(dm_line < other.dm_line)
		    return true;
		if(dm_line > other.dm_line)
		    return false;
		return dm_column < other.dm_column;		
	    }

	};

        /** Table mapping statments to their address ranges. */
        typedef std::map<StatementEntry,
                         std::vector<AddressRange> > StatementTable;

        /** Statements in this symbol table. */
        StatementTable table_statements;
	
	/** Statements in this symbol table. */
	std::map<StatementEntry, std::vector<AddressRange> > dm_statements;

	public:

	typedef std::map<StatementEntry, std::vector<AddressRange> >
	StatementMap;

	StatementMap getStatements() { return dm_statements; };
	
    };

        typedef std::map<AddressRange,
        std::pair<SymbolTable, LinkedObjectEntry > >
        SymbolTableMap;

	typedef std::map<AddressRange, std::string> FunctionMap;
} }



#endif
