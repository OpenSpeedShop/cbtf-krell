////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 The Krell Institute. All Rights Reserved.
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
 * Declaration of the SymtabAPISymbols class.
 *
 */

#include "KrellInstitute/Core/SymtabAPISymbols.hpp"
#include "KrellInstitute/Core/Address.hpp"
#include "KrellInstitute/Core/AddressRange.hpp"
#include "Symtab.h"
#include "LineInformation.h"
#include "Function.h"
#include <sstream>

using namespace KrellInstitute::Core;
using namespace Dyninst;
using namespace Dyninst::SymtabAPI;

#ifndef NDEBUG
/** Flag indicating if debuging is enabled. */
bool SymtabAPISymbols::is_debug_symtabapi_symbols_enabled =
    (getenv("CBTF_DEBUG_SYMTABAPI_SYMBOLS") != NULL);
bool SymtabAPISymbols::is_debug_symtabapi_symbols_detailed_enabled =
    (getenv("CBTF_DEBUG_SYMTABAPI_SYMBOLS_DETAILS") != NULL);
#endif


// get all function symbols and statements for this linked object
// that match an address in the passed AddressBuffer.
void
SymtabAPISymbols::getSymbols(const AddressBuffer& abuffer,
			     const LinkedObjectEntry& linkedobject,
			     SymbolTable& st)
{
    std::set<Address> addresses;
    AddressCounts ac = abuffer.addresscounts;
    AddressCounts::const_iterator aci;

    std::set<Address>::iterator ai;
    for (aci = ac.begin(); aci != ac.end(); ++aci) {
	addresses.insert(aci->first);
    }

    std::string objname = linkedobject.getPath();
    AddressRange lorange = linkedobject.getAddressRange();
    std::set<AddressRange>::iterator si;


// DEBUG
#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_enabled) {
	std::cerr << "SymtabAPISymbols::getSymbols: Processing linked object "
	    << objname << " with address range " << lorange
	    << " addresses is " << addresses.size()
	    << std::endl;
    }
#endif



    Symtab *symtab;
    bool err = Symtab::openFile(symtab, objname);


    KrellInstitute::Core::Address image_offset(symtab->imageOffset());
    KrellInstitute::Core::Address image_length(symtab->imageLength());
    AddressRange image_range(image_offset,image_offset+image_length);

// DEBUG
#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_enabled) {
        std::cerr << "SymtabAPISymbols::getSymbols: Image range for" << objname << " " << image_range << std::endl;
    }
#endif

    KrellInstitute::Core::Address base(0);
    if ( (image_range.getBegin() - lorange.getBegin()) < 0 ) {
	base = lorange.getBegin();
    }

    std::vector <Dyninst::SymtabAPI::Function *>fsyms;

    // Make sure we get the full filename
    if(symtab) {
       symtab->setTruncateLinePaths(false);
    }


    if(symtab && !symtab->getAllFunctions(fsyms)) {
#ifndef NDEBUG
	if(is_debug_symtabapi_symbols_enabled) {
	    std::cerr << "Dyninst::SymtabAPI::Symbol::getAllFunctions unable to get functions\n`"
	    << "from " << objname << " range: " << image_range << "\n"
	    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
	    << std::endl;
	}
#endif

    }

    std::set<KrellInstitute::Core::Address> function_begin_addresses;
    std::vector <Function *>::iterator fsit;

#ifndef NDEBUG
    std::stringstream output;
#endif

    for(fsit = fsyms.begin(); fsit != fsyms.end(); ++fsit) {
	int sym_size = (*fsit)->getSize();
	KrellInstitute::Core::Address begin((*fsit)->getOffset());
	KrellInstitute::Core::Address end(begin + sym_size);

	// don't let an invalid range through...
	if (begin >= end) continue;

	AddressRange frange(begin,end);

	for (ai=addresses.equal_range(lorange.getBegin()).first;
     		ai!=addresses.equal_range(lorange.getEnd()).second;ai++) {
	    // normalize address for testing range from symtabapi.
	    KrellInstitute::Core::Address theAddr(*ai - base.getValue()) ; 
	    if (frange.doesContain( theAddr )) {

		std::string fname =
			(*fsit)->getFirstSymbol()->getPrettyName();
// DEBUG
#ifndef NDEBUG
		if(is_debug_symtabapi_symbols_detailed_enabled) {
	            output << "SymtabAPISymbols::getSymbols: ADDING FUNCTION " << fname
		    << " " << frange
		    << " pc:" << *ai
		    << " adjusted pc:" << theAddr
		    << std::endl;
		}
#endif
		st.addFunction(begin+base,end+base,fname);

		// Record the function begin addresses, This allows the
		// cli and gui to focus on or display the first
		// statement of a function.
		// The function begin addresses will be processed later
		// for statement info and added to our statements.
		function_begin_addresses.insert(begin);
		break;
	    }
	}
    }

#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_detailed_enabled) {
	std::cerr << output.str();
        output.clear();
    }
#endif


    std::vector <Module *>mods;
    AddressRange module_range;
    std::string module_name;
    if(symtab && !symtab->getAllModules(mods)) {
	std::cerr << "SymtabAPISymbols::getSymbols: getAllModules unable to get all modules  "
	    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
	    << std::endl;
    } else {
// DEBUG
#ifndef NDEBUG
        if(is_debug_symtabapi_symbols_detailed_enabled) {
	    for(unsigned i = 0; i< mods.size();i++){
	        module_range =
		   AddressRange(mods[i]->addr(),
				mods[i]->addr() + symtab->imageLength());
	        module_name = mods[i]->fullName();
	        std::cerr << "SymtabAPISymbols::getSymbols: getAllModules for " << mods[i]->fullName()
		    << " Range " << module_range
		    << std::endl;
	    }
	}
#endif
    }

// DEBUG
#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_enabled) {

        std::cerr << "SymtabAPISymbols::getSymbols: image_offset:" << image_offset
	<< " image_length:" << image_length
	<< " image_range:" << image_range << std::endl;

	std::cerr << "USING BASE OFFSET " << base << std::endl;
    }
#endif

    for (ai=addresses.equal_range(lorange.getBegin()).first;
    	    ai!=addresses.equal_range(lorange.getEnd()).second;ai++) {
	// normalize address for testing range from symtabapi.
	KrellInstitute::Core::Address theAddr(*ai - base.getValue()) ; 
	Offset myoffset = theAddr.getValue();

	for(unsigned i = 0; i< mods.size();i++) {
	    std::vector< Dyninst::SymtabAPI::Statement *> mylines;
	    mods[i]->getSourceLines(mylines,myoffset);
	    if (mylines.size() > 0) {
		for (std::vector<Dyninst::SymtabAPI::Statement *>::iterator si = mylines.begin();
		     si != mylines.end(); si++) {
// DEBUG
#ifndef NDEBUG
		    if(is_debug_symtabapi_symbols_detailed_enabled) {
			output << "SymtabAPISymbols::getSymbols: SAMPLE Address:" << theAddr + base
			<< " " << (*si)->getFile()
			<< ":" << (*si)->getLine()
			<< ":" << (int) (*si)->getColumn()
			<< ":" << KrellInstitute::Core::Address((*si)->startAddr()) +base
			<< ":" << KrellInstitute::Core::Address((*si)->endAddr()) +base
			<< std::endl;
		    }
#endif
		    // add the base offset back when recording statement.
		    st.addStatement(KrellInstitute::Core::Address((*si)->startAddr()) +base,
				KrellInstitute::Core::Address((*si)->endAddr()) +base,
				(*si)->getFile(),
				(*si)->getLine(),
				(int) (*si)->getColumn()
				);
		}
	    }
	} // mods loop
    } // sampled addresses loop

#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_detailed_enabled) {
	std::cerr << output.str();
        output.clear();
    }
#endif

    // Find any statements for the beginning of a function that
    // contained a valid sample address.
    for(std::set<KrellInstitute::Core::Address>::const_iterator fi = function_begin_addresses.begin();
			                                      fi != function_begin_addresses.end();
			                                      ++fi) {
	// Lookup address by subtracting base offset.
	KrellInstitute::Core::Address theAddr(*fi - base.getValue()) ; 
	Offset myoffset = theAddr.getValue();
        for(unsigned i = 0; i< mods.size();i++) {
	    std::vector< Dyninst::SymtabAPI::Statement *> mylines;
	    mods[i]->getSourceLines(mylines,myoffset);
	    if (mylines.size() > 0) {
		for(std::vector<Dyninst::SymtabAPI::Statement *>::iterator si = mylines.begin();
			si != mylines.end(); si++) {

// DEBUG
#ifndef NDEBUG
	 	    if(is_debug_symtabapi_symbols_detailed_enabled) {
			output << "SymtabAPISymbols::getSymbols: FUNCTION BEGIN"
				  << " " << (*si)->getFile()
				  << ":" << (*si)->getLine()
				  << ":" << (int) (*si)->getColumn()
				  << ":" << KrellInstitute::Core::Address((*si)->startAddr()) +base
				  << ":" << KrellInstitute::Core::Address((*si)->endAddr()) +base
				  << std::endl;
		    }
#endif

		    // add the base offset back when recording statement.
		    st.addStatement(KrellInstitute::Core::Address((*si)->startAddr()) +base,
				KrellInstitute::Core::Address((*si)->endAddr()) +base,
				(*si)->getFile(),
				(*si)->getLine(),
				(int) (*si)->getColumn()
				);
		}
	    }
	} // mods loop
    } // function begin statement loop
#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_detailed_enabled) {
	std::cerr << output.str();
        output.clear();
    }
#endif
}


void
SymtabAPISymbols::getSymbols(const std::set<Address>& addresses,
			     const LinkedObjectEntry& linkedobject,
			     SymbolTableMap& stm)
{
}

// get all function symbols and statements for this linked object.
void
SymtabAPISymbols::getAllSymbols(const LinkedObjectEntry& linkedobject,
			     SymbolTable& st)
{

    std::string objname = linkedobject.getPath();
    AddressRange lrange = linkedobject.getAddressRange();

// DEBUG
#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_enabled) {
	    std::cerr << "SymtabAPISymbols::getAllSymbols: Processing linked object "
	    << objname << " with address range " << lrange << std::endl;
    }
#endif

    Symtab *symtab;
    bool err = Symtab::openFile(symtab, objname);

    KrellInstitute::Core::Address image_offset(symtab->imageOffset());
    KrellInstitute::Core::Address image_length(symtab->imageLength());
    AddressRange image_range(image_offset,image_offset+image_length);

    KrellInstitute::Core::Address base(0);
    if ( (image_range.getBegin() - lrange.getBegin()) < 0 ) {
	base = lrange.getBegin();
    }

    std::vector <SymtabAPI::Function *>fsyms;

    if(symtab && !symtab->getAllFunctions(fsyms) ) {
	std::cerr << "Symtab getAllFunctions unable to get all Functions "
	    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
	    << std::endl;
    }

    std::vector<SymtabAPI::Function *>::iterator fsit;

#ifndef NDEBUG
    std::stringstream output;
#endif

    for(fsit = fsyms.begin(); fsit != fsyms.end(); ++fsit) {
	int sym_size = (*fsit)->getSize();
	KrellInstitute::Core::Address begin((*fsit)->getOffset());
	KrellInstitute::Core::Address end(begin + sym_size);

	// don't let an invalid range through...
	if (begin >= end) continue;

// DEBUG
#ifndef NDEBUG
	if(is_debug_symtabapi_symbols_enabled) {
	    output << "SymtabAPISymbols::getAllSymbols: ADDING FUNCTION "
		<< (*fsit)->getFirstSymbol()->getPrettyName()
		<< " RANGE " << begin << "," << end << std::endl;
	}
#endif
	st.addFunction(begin + base, end + base,(*fsit)->getFirstSymbol()->getPrettyName());
    }

#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_enabled) {
	std::cerr << output.str();
	output.clear();
    }
#endif

    std::vector <Module *>mods;
    AddressRange module_range;
    std::string module_name;
    if(symtab && !symtab->getAllModules(mods)) {
	std::cerr << "Symtab getAllModules unable to get all modules  "
	    << Symtab::printError(Symtab::getLastSymtabError()).c_str()
	    << std::endl;
    } else {
// DEBUG
#ifndef NDEBUG
	if(is_debug_symtabapi_symbols_enabled) {
	    for(unsigned i = 0; i< mods.size();i++){
	        module_range =
		   AddressRange(mods[i]->addr(), mods[i]->addr() + symtab->imageLength());
	        module_name = mods[i]->fullName();
	        std::cerr << "Symtab getAllModules MNAME " << mods[i]->fullName()
		    << " Range " << module_range << std::endl;
	    }
	}
#endif
    }

// DEBUG
#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_enabled) {

        std::cerr << "SymtabAPISymbols::getAllSymbols: image_offset " << image_offset
	<< " image_length " << image_length
	<< " image_range " << image_range << std::endl;

	std::cerr << "USING BASE OFFSET " << base << std::endl;
    }
#endif


    for(unsigned i = 0; i< mods.size();i++) {

	LineInformation *lineInformation = mods[i]->getLineInformation();
	if(lineInformation) {
	    LineInformation::const_iterator iter = lineInformation->begin();
	    for(;iter!=lineInformation->end();iter++) {
		const std::pair<Offset, Offset> range = iter->first;
		LineNoTuple line = iter->second;
		KrellInstitute::Core::Address b(range.first);
		KrellInstitute::Core::Address e(range.second);
// DEBUG
#ifndef NDEBUG
		if(is_debug_symtabapi_symbols_enabled) {
		    output << "SymtabAPISymbols::getAllSymbols: ADDING STATEMENT " << b << ":" << e
		    <<" " << line.first << ":" << line.second  << std::endl;
		}
#endif
		st.addStatement(b,e,line.first,line.second,line.column);
	    }
	}
    }
#ifndef NDEBUG
    if(is_debug_symtabapi_symbols_enabled) {
	std::cerr << output.str();
	output.clear();
    }
#endif
}

void
SymtabAPISymbols::getDepenentLibs(const std::string& objname,
	   std::vector<std::string>& dependencies)
{
    Symtab *symtab;
    bool err = Symtab::openFile(symtab, objname);
    if (symtab) {
	dependencies = symtab->getDependencies();
    }
}

bool SymtabAPISymbols::foundLibrary(const std::string& exename, const std::string& libname)
{

    std::vector<std::string> depends;
    getDepenentLibs(exename,depends);
    for (std::vector<std::string>::iterator curDep = depends.begin(); curDep != depends.end(); curDep++) {
      if (curDep->find(libname) != std::string::npos) {
        return true; 
      }
    }
   return false;
}

