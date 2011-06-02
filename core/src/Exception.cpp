////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
// Copyright (c) 2011 The Krell Institute. All Rights Reserved.
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
 * Definition of the Exception class.
 *
 */

#include "KrellInstitute/Core/Exception.hpp"

#include <sstream>

using namespace KrellInstitute::Core;



namespace {

    /**
     * Description table.
     *
     * Table containing the description string corresponding to each of the
     * possible exception codes. This strings are centralized in one place to
     * facilitate internationalization in the future.
     */
    const struct {
	Exception::Code dm_code;     /**< Exception code. */
	std::string dm_description;  /**< Description of that exception code. */
    } DescriptionTable[] = {
	
	{ Exception::Unknown,
	  "A generic, unknown, exception has occured. " },
	
	// End Of Table Entry
	{ Exception::Unknown, "" }
	
    };
    
}



/**
 * Constructor from code and no arguments.
 *
 * Constructs an Exception from the specified code. No arguments are specified
 * for the exception's text description, but arguments can be specified after
 * object construction using appendArgument().
 *
 * @param code    Code describing the exception.
 */
Exception::Exception(const Code& code) :
    dm_code(code),
    dm_arguments()
{
}



/**
 * Constructor from code and one argument.
 *
 * Constructs an Exception from the specified code. A single argument is
 * specified for the exception's text description, and additional arguments
 * can be specified after object construction using appendArgument().
 *
 * @param code        Code describing the exception.
 * @param argument    Argument to the exception's text description.
 */
Exception::Exception(const Code& code, const std::string& argument) :
    dm_code(code),
    dm_arguments()
{
    dm_arguments.push_back(argument);
}



/**
 * Constructor from code and two arguments.
 *
 * Constructs an Exception from the specified code. Two arguments are
 * specified for the exception's text description, and additional arguments
 * can be specified after object construction using appendArgument().
 *
 * @param code         Code describing the exception.
 * @param argument1    First argument to the exception's text description.
 * @param argument2    Second argument to the exception's text description.
 */
Exception::Exception(const Code& code,
		     const std::string& argument1,
		     const std::string& argument2) :
    dm_code(code),
    dm_arguments()
{
    dm_arguments.push_back(argument1);
    dm_arguments.push_back(argument2);
}



/**
 * Constructor from code and three arguments.
 *
 * Constructs an Exception from the specified code. Three arguments are
 * specified for the exception's text description, and additional arguments
 * can be specified after object construction using appendArgument().
 *
 * @param code         Code describing the exception.
 * @param argument1    First argument to the exception's text description.
 * @param argument2    Second argument to the exception's text description.
 * @param argument3    Third argument to the exception's text description.
 */
Exception::Exception(const Code& code,
		     const std::string& argument1,
		     const std::string& argument2,
		     const std::string& argument3) :
    dm_code(code),
    dm_arguments()
{
    dm_arguments.push_back(argument1);
    dm_arguments.push_back(argument2);
    dm_arguments.push_back(argument3);
}



/**
 * Append an argument.
 *
 * Appends the specified argument to the argument list for this exception's
 * text description.
 *
 * @param argument    Argument to the exception's text description.
 */
void Exception::appendArgument(const std::string& argument)
{
    dm_arguments.push_back(argument);
}



/**
 * Get our description.
 *
 * Returns the text description for this exception. Any arguments specified are
 * substituted into the description before it is returned.
 *
 * @return    Text description of this exception.
 */
std::string Exception::getDescription() const
{
    std::string description = "";
    
    // Iterate over each entry in the description table
    for(int i = 0; !DescriptionTable[i].dm_description.empty(); ++i)
	if(dm_code == DescriptionTable[i].dm_code) {	    
	    
	    // Make a copy of this exception's description
	    description = DescriptionTable[i].dm_description;
	    
	    // Iterate over each of this exception's arguments
	    for(int j = 0; j < dm_arguments.size(); ++j) {

		// Form the argument search term for this argument
		std::ostringstream search;
		search << "%" << (j + 1);

		// Iterate over each search term occurence in the description
		for(std::string::size_type
			pos = description.find(search.str(), 0);
		    pos != std::string::npos;
		    pos = description.find(search.str(), pos)) {
		    
		    // Replace the search term with the argument string
		    description = 
			description.replace(pos, search.str().size(),
					    dm_arguments[j]);
		    
		    // Advance past the argument string
		    pos += dm_arguments[j].size();
		    
		}

	    }
	    
	}
    
    // Return the description to the caller
    return description;
}
