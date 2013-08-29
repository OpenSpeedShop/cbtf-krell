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

/** @file Declaration of the FileName class. */

#pragma once

#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/operators.hpp>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <KrellInstitute/Messages/File.h>

namespace KrellInstitute { namespace Base {

    /**
     * Unique name for a file that includes a checksum of the named file's
     * contents in addition to its full path.
     *
     * @note    The exact algorithm used to calculate the checksum is left
     *          unspecified, but can be expected to be something similar to
     *          CRC-64-ISO. This checksum is either calculated automagically
     *          upon the construction of a new FileName, or extracted from
     *          the CBTF_Protocol_FileName, as appropriate.
     */
    class FileName :
        public boost::totally_ordered<FileName>
    {
	
    public:
        
        /** Construct a file name from the file's full path. */
        FileName(const boost::filesystem::path& path) :
            dm_path(path),
            dm_checksum(0)
        {
            if (boost::filesystem::is_regular_file(path))
            {
                dm_checksum = computeChecksum(path);
            }
        }
        
        /** Construct a file name from a CBTF_Protocol_FileName. */
        FileName(const CBTF_Protocol_FileName& message) :
            dm_path(message.path),
            dm_checksum(message.checksum)
        {
        }

        /** Type conversion to a boost::filesystem::path. */
        operator boost::filesystem::path() const
        {
            return dm_path;
        }

        /** Type conversion to a CBTF_Protocol_FileName. */
        operator CBTF_Protocol_FileName() const
        {
            CBTF_Protocol_FileName message;
            message.path = strdup(dm_path.c_str());
            message.checksum = dm_checksum;
            return message;
        }

        /** Type conversion to a string. */
        operator std::string() const
        {
            std::ostringstream stream;
            stream << *this;
            return stream.str();
        }

        /** Is this file name less than another one? */
        bool operator<(const FileName& other) const
        {
            if (dm_path < other.dm_path)
            {
                return true;
            }
            else if ((dm_path == other.dm_path) &&
                     (dm_checksum != 0) && (other.dm_checksum != 0) &&
                     (dm_checksum < other.dm_checksum))
            {
                return true;
            }
            return false;
        }

        /** Is this file name equal to another one? */
        bool operator==(const FileName& other) const
        {
            if (dm_path != other.dm_path)
            {
                return false;
            }
            else if ((dm_checksum != 0) && (other.dm_checksum != 0) &&
                     (dm_checksum != other.dm_checksum))
            {
                return false;
            }
            return true;
        }

        /** Get the full path of the named file. */
        const boost::filesystem::path& path() const
        {
            return dm_path;
        }

        /** Get the checksum of the named file's contents. */
        boost::uint64_t checksum() const
        {
            return dm_checksum;
        }
        
        /** Redirection to an output stream. */
        friend std::ostream& operator<<(std::ostream& stream,
                                        const FileName& name)
        {
            stream << boost::str(boost::format("0x%016X") % name.dm_checksum)
                   << ": " << name.dm_path;
            return stream;
        }
        
    private:

        /** Compute the checksum of the specified file's contents. */
        static boost::uint64_t computeChecksum(
            const boost::filesystem::path& path
            );
        
        /** Full path of the named file. */
        boost::filesystem::path dm_path;
        
        /** Checksum of the named file's contents. */
        boost::uint64_t dm_checksum;
        
    }; // class FileName
            
} } // namespace KrellInstitute::Base
