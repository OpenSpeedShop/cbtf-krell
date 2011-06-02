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
 * Declaration of the Exception class.
 *
 */

#ifndef _KrellInstitute_Core_Exception_
#define _KrellInstitute_Core_Exception_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sstream>
#include <string>
#include <vector>



namespace KrellInstitute { namespace Core {
    
    /**
     * Runtime exception.
     *
     * An object of this class is thrown when an exception occurs outside the
     * scope of the framework. That is, an exception that either was caused by
     * input from some other external entity (e.g. a tool), or cannot be easily 
     * predicted and can only be caught as the software executes (e.g. the user
     * specifies a database file name that doesn't exist). Each exception has a
     * code that indicates the specific problem that occured. It also provides
     * a text description of the exception.
     *
     * @ingroup Utility
     */
    class Exception
    {
	
    public:

	template <typename T>
	static std::string toString(const T&);

	/**
	 * Exception code enumeration.
	 *
	 * Enumeration defining all the possible exception codes that might be
	 * thrown. No specific rhyme or reason is used to determine how coarse
	 * or fine grained these codes are. It is dictated purely by the needs
	 * of the tools.
	 */
	enum Code {
	    Unknown                 /**< Catch-all for generic exceptions. */
	};

	Exception(const Code&);
	Exception(const Code&, const std::string&);
	Exception(const Code&, const std::string&, const std::string&);
	Exception(const Code&, const std::string&, 
		  const std::string&, const std::string&);
	
	void appendArgument(const std::string&);

	/** Read-only data member accessor function. */
        const Code& getCode() const
	{
	    return dm_code;
	}

	std::string getDescription() const;
	
    private:
	
	/** Code describing the exception. */
	Code dm_code;

	/** Arguments to the exception's text description. */
	std::vector<std::string> dm_arguments;
	
    };



    /**
     * Convert type to string.
     *
     * Converts a type to a string for the purposes of using it as an argument
     * to an exception. The only requirement of the type being converted is that
     * it be redirectable to an output stream.
     *
     * @param value    Value to convert.
     * @return         String conversion of that value.
     */
    template <typename T>
    std::string Exception::toString(const T& value)
    {
	std::ostringstream stream;
	stream << value;
	return stream.str();
    }
    
    
    
} }



#endif
