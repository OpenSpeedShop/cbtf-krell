/*******************************************************************************
 ** Copyright (c) 2007-2013 The Krell Institue. All Rights Reserved.
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
 * Definition of the CBTF Core BFD symbol support functions.
 * This code is based on examples (directly copied) from binutils
 * tools like objdump, addr2line, nm.  We use the bfd and
 * code examples from these tools to match addresses gathered
 * from offline collectors to symbol information.
 *
 */

#include <bfd.h>
#include <stdint.h>
#include <vector>
//#include "OfflineExperiment.hxx"
//#include "LinkedObject.hxx"
#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressBuffer.hpp"
#include "KrellInstitute/Core/LinkedObjectEntry.hpp"
#include "KrellInstitute/Core/SymbolTable.hpp"

namespace KrellInstitute { namespace Core {

class BFDFunction {
    std::string func_name;
    Address func_begin;
    Address func_end;

    public:
    BFDFunction (const std::string& name,
		 const uint64_t& begin,
		 const uint64_t& end ) :
	func_name(name),
	func_begin(begin),
	func_end(end)
    {
    };

    BFDFunction& operator=(const BFDFunction& b)
    {
	func_name = b.func_name;
	func_begin = b.func_begin;
	func_end = b.func_end;
	return *this;
    }

    std::string getFuncName() { return func_name; };
    Address getFuncBegin() { return func_begin; };
    Address getFuncEnd() { return func_end; };
};

struct BFDStatement
{
    uint64_t pc;
    std::string file_name;
    unsigned int lineno;

    BFDStatement (const uint64_t& pc,
	          const std::string& filename,
	          const unsigned int& line) :
	pc(pc),
	file_name(filename),
	lineno(line)
   {
   }
};


typedef std::vector<BFDFunction>  FunctionsVec;
typedef std::vector<BFDStatement> StatementsVec;

class BFDSymbols {

    public:

#if 0
    int		getBFDFunctionStatements(AddressBuffer*, const LinkedObject&,
					 SymbolTableMap&);
#else
    int		getBFDFunctionStatements(const AddressBuffer&, const LinkedObjectEntry&,
					 SymbolTable&);
#endif
    int		getFunctionSyms(const AddressBuffer&, bfd_vma, bfd_vma);
    int		initBFD(std::string);
    Path	getObjectFile(Path);
    //void	getSymbols(AddressBuffer*, const LinkedObject&, SymbolTableMap&);
    void	getSymbols(const AddressBuffer&, const LinkedObjectEntry&, SymbolTable&);

    private:

    void	slurp_symtab (bfd *);
    int		dummyprint () { return 0; };
    long	remove_useless_symbols (asymbol **symbols, long count);

    int init_done;

#ifndef NDEBUG
    static bool is_debug_bfd_symbols_enabled;
    static bool is_debug_bfd_symbols_details_enabled;
    static bool is_debug_offline_enabled;
#endif

};

} };
