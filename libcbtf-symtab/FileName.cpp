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

/** @file Definition of the FileName class. */

#include <boost/crc.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <KrellInstitute/SymbolTable/FileName.hpp>

using namespace KrellInstitute::SymbolTable;



//------------------------------------------------------------------------------
// Compute the checksum of the specified file's contents using "CRC-64" from:
//
//     http://reveng.sourceforge.net/crc-catalogue/17plus.htm#crc.cat-bits.64
//
// Comments below note the names of the template parameters to the crc_optimized
// class as listed in Boost documentation, and their correspondence to fields in
// the above-mentioned document (in parentheses).
//------------------------------------------------------------------------------
boost::uint64_t FileName::computeChecksum(const boost::filesystem::path& path)
{
    char buffer[1 * 1024 * 1024 /* 1 MB */];

    boost::crc_optimal<
        64,                 // Bits (width)
        0x42f0e1eba9ea3693, // TruncPoly (poly)
        0x0000000000000000, // InitRem (init)
        0x0000000000000000, // FinalXor (xorout)
        false,              // ReflectIn (refin)
        false               // ReflectRem (refout)
        > crc;
    
    boost::filesystem::ifstream stream(path, std::ios::binary);

    for (std::streamsize n = stream.readsome(buffer, sizeof(buffer));
         n > 0;
         n = stream.readsome(buffer, sizeof(buffer)))
    {
        crc.process_bytes(buffer, n);
    }
    
    stream.close();
    
    return static_cast<boost::uint64_t>(crc.checksum());
}
