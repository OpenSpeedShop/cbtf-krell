////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014 The Krell Institute. All Rights Reserved.
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
 * Definition of the LinkedObject class.
 *
 */

#include "KrellInstitute/Core/Assert.hpp"
#include "KrellInstitute/Core/LinkedObject.hpp"
#include "KrellInstitute/Core/Path.hpp"

using namespace KrellInstitute::Core;

LinkedObject::LinkedObject()
{
}

/**
 * Get our path.
 *
 * Returns the full path name of this linked object.
 *
 * @return    Full path name of this linked object.
 */
Path LinkedObject::getPath() const
{
    // Return the full path name to the caller
    return path;
}


/**
 * Test if we are an executable.
 *
 * Returns a boolean value indicating if this linked object is an executable or
 * not (i.e. a shared library). This does not necessarily correlate one-to-one
 * with the "executable" bit in the file system. It means, instead, that this
 * linked object was the "a.out" for one of the processes in the experiment.
 *
 * @return    Boolean "true" if this linked object is an executable,
 *            "false" otherwise.
 */
bool LinkedObject::isExecutable() const
{
    // Return the flag to the caller
    return is_executable;
}

// the addressrange of a linked object.
AddressRange
LinkedObject::getAddressRange() const
{

    return range;
}

// the time interval of a linked object.
TimeInterval
LinkedObject::getTimeInterval() const
{

    return time;
}

bool LinkedObject::operator==(const LinkedObject& other) const
{

    if(path == other.path &&
       time == other.time &&
       range == other.range)
	return true;

    return false;
}
