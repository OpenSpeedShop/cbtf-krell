////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file Definition of the SymtabAPIResolver class. */

#include <KrellInstitute/Base/Raise.hpp>
#include <stdexcept>

#include "SymtabAPIResolver.hpp"

using namespace Dyninst::SymtabAPI;

using namespace KrellInstitute::Base;
using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



/** Anonymous namespace hiding implementation details. */
namespace {

    // ...

} // namespace <anonymous>



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
SymtabAPIResolver::SymtabAPIResolver(AddressSpaces& spaces) :
    dm_spaces(spaces),
    dm_symtabs()
{
}



//------------------------------------------------------------------------------
// Close all of the symbol tables opened by this resolver. SymtabAPI keeps a
// reference count for each symbol table internally, and will free the actual
// pointer when appropriate.
//------------------------------------------------------------------------------
SymtabAPIResolver::~SymtabAPIResolver()
{
    for (std::map<FileName, Symtab*>::iterator
             i = dm_symtabs.begin(), iEnd = dm_symtabs.end(); i != iEnd; ++i)
    {
        Symtab::closeSymtab(i->second);
    }
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymtabAPIResolver::operator()(const LinkedObject& linked_object)
{
    // ...
}



//------------------------------------------------------------------------------
// ...
//------------------------------------------------------------------------------
void SymtabAPIResolver::operator()(const ThreadName& thread,
                                   const AddressRange& range,
                                   const TimeInterval& interval)
{
    // ...
}



//------------------------------------------------------------------------------
// First search our own indexed list of symbol tables for the name of the given
// linked object's file. If it isn't found there check whether SymtabAPI has it
// cached. Finally, if not, open the file using SymtabAPI after first verifying
// that the file hasn't been modified.
//------------------------------------------------------------------------------
Symtab* SymtabAPIResolver::open(const LinkedObject& linked_object)
{
    FileName name = linked_object.getFile();

    std::map<FileName, Symtab*>::iterator i = dm_symtabs.find(name);
    
    if (i == dm_symtabs.end())
    {
        if (name != FileName(name.path()))
        {            
            raise<std::runtime_error>(
                "The given linked object (%1%) has changed recently and "
                "using it to resolve symbols may result in the reporting "
                "of inaccurate performance data.", name.path()
                );
        }
        
        Symtab* symtab = Symtab::findOpenSymtab(name.path().string());
        
        if (symtab == NULL)
        {
            Symtab::openFile(symtab, name.path().string());
            
            if (symtab == NULL)
            {
                raise<std::runtime_error>(
                    "The given linked object (%1%) could not "
                    "be opened by SymtabAPI. ", name.path()
                    );
            }
            
            symtab->setTruncateLinePaths(false);
        }
        
        i = dm_symtabs.insert(std::make_pair(name, symtab)).first;
    }
    
    return i->second;
}
