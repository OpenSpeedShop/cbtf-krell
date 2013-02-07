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

/** @file 
 * Declaration and definition of the performance data pack/unpack functions.
 */

#pragma once

#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdlib>
#include <KrellInstitute/Messages/DataHeader.h>
#include <KrellInstitute/Messages/Blob.h>
#include <rpc/rpc.h>
#include <stdexcept>
#include <typeinfo>
#include <utility>

namespace KrellInstitute { namespace Messages {

    namespace Impl {

        /**
         * Custom deleter function for shared pointers to XDR types.
         *
         * @tparam T     XDR type being deleted.
         *
         * @param ptr         Pointer to the XDR type being deleted.
         * @param xdr_proc    XDR procedure for the specified XDR type.
         */
        template <typename T>
        static void xdr_deleter(T* ptr, const xdrproc_t xdr_proc)
        {
            BOOST_ASSERT((ptr != NULL) && (xdr_proc != NULL));
            xdr_free(xdr_proc, reinterpret_cast<char*>(ptr));
            delete ptr;
        }

    }

    /**
     * Pack the specified performance data header and performance data into
     * a performance data blob.
     *
     * @tparam T    Type of the performance data.
     *
     * @param in          Pair containing the performance data header
     *                    and performance data.
     * @param xdr_proc    XDR procedure for the performance data type.
     * @return            Performance data blob.
     *
     * @note    The performance data blob produced by this function is of
     *          the same format as Open|SpeedShop's performance data blobs.
     */
    template <typename T>
    boost::shared_ptr<CBTF_Protocol_Blob> pack(
        const std::pair<
            boost::shared_ptr<CBTF_DataHeader>, boost::shared_ptr<T>
            >& in,
        const xdrproc_t& xdr_proc
        )
    {
        if (!in.first)
        {
            throw std::runtime_error(boost::str(
                boost::format(
                    "The performance data header for type \"%1%\" was null."
                    ) % typeid(T).name()
                ));
        }

        if (!in.second)
        {
            throw std::runtime_error(boost::str(
                boost::format(
                    "The performance data for type \"%1%\" was null."
                    ) % typeid(T).name()
                ));
        }

        if (xdr_proc == NULL)
        {
            throw std::runtime_error(boost::str(
                boost::format(
                    "The XDR procedure for type \"%1%\" was null."
                    ) % typeid(T).name()
                ));
        }

        // Allocate the performance data blob
        boost::shared_ptr<CBTF_Protocol_Blob> out(
            new CBTF_Protocol_Blob(),
            boost::bind(&Impl::xdr_deleter<CBTF_Protocol_Blob>, _1, 
                        reinterpret_cast<xdrproc_t>(xdr_CBTF_Protocol_Blob))
            );
        
        // Iteratively increase the encoding buffer size
        for (unsigned int size = 64 * 1024 /* 64 KB */; size > 0; size *= 2)
        {
            // Allocate an encoding buffer
            char* buffer = reinterpret_cast<char*>(malloc(size));
            
            // Create an XDR stream using this encoding buffer
            XDR xdrs;
            xdrmem_create(&xdrs, buffer, size, XDR_ENCODE);
            
            // Attempt to encode using this XDR stream
            bool_t ok = TRUE;
            ok &= xdr_CBTF_DataHeader(
                &xdrs, const_cast<CBTF_DataHeader*>(in.first.get())
                );
            ok &= (*xdr_proc)(
                &xdrs,
                const_cast<void*>(
                    reinterpret_cast<const void*>(in.second.get())
                    )
                );

            // Keep this encoding if it succeeded
            if (ok)
            {                
                out->data.data_len = static_cast<u_int>(xdr_getpos(&xdrs));
                out->data.data_val = reinterpret_cast<uint8_t*>(buffer);
                size = 0;
            }
            
            // Close this XDR stream
            xdr_destroy(&xdrs);
            
            // Destroy the encoding buffer if encoding wasn't successful
            if (size > 0)
            {
                free(buffer);
            }
        }

        // Return the performance data blob to the caller
        return out;
    }

    /**
     * Unpack the specified performance data blob into a performance data
     * header and performance data.
     *
     * @tparam T    Type of the performance data.
     *
     * @param in          Performance data blob.
     * @param xdr_proc    XDR procedure for the performance data type.
     * @return            Pair containing the performance data header
     *                    and performance data.
     *
     * @note    The performance data blob consumed by this function is of
     *          the same format as Open|SpeedShop's performance data blobs.
     */
    template <typename T>
    std::pair<boost::shared_ptr<CBTF_DataHeader>, boost::shared_ptr<T> > unpack(
        const boost::shared_ptr<CBTF_Protocol_Blob>& in,
        const xdrproc_t& xdr_proc
        )
    {
        if (!in)
        {
            throw std::runtime_error(boost::str(
                boost::format(
                    "The performance data blob for type \"%1%\" was null."
                    ) % typeid(T).name()
                ));
        }
        
        if (xdr_proc == NULL)
        {
            throw std::runtime_error(boost::str(
                boost::format(
                    "The XDR procedure for type \"%1%\" was null."
                    ) % typeid(T).name()
                ));
        }

        // Allocate the performance data header and data
        std::pair<
            boost::shared_ptr<CBTF_DataHeader>, boost::shared_ptr<T>
            > out(
                boost::shared_ptr<CBTF_DataHeader>(
                    new CBTF_DataHeader(),
                    boost::bind(&Impl::xdr_deleter<CBTF_DataHeader>, _1,
                                reinterpret_cast<xdrproc_t>(
                                    xdr_CBTF_DataHeader
                                    ))
                    ),
                boost::shared_ptr<T>(
                    new T(),
                    boost::bind(&Impl::xdr_deleter<T>, _1, xdr_proc)
                    )
                );

        // Create an XDR stream using the performance data blob's contents
        XDR xdrs;
        xdrmem_create(&xdrs,
                      reinterpret_cast<char*>(in->data.data_val),
                      static_cast<unsigned int>(in->data.data_len),
                      XDR_DECODE);

        // Attempt to decode using this XDR stream
        bool_t ok = TRUE;
        ok &= xdr_CBTF_DataHeader(&xdrs, out.first.get());
        ok &= (*xdr_proc)(&xdrs, reinterpret_cast<void*>(out.second.get()));

        // Decoding only fails if the performance data blob is ill-formed
        if (!ok)
        {
            throw std::runtime_error(boost::str(
                boost::format(
                    "The performance data blob for type "
                    "\"%1%\" could not be unpacked."
                    ) % typeid(T).name()
                ));
        }
        BOOST_ASSERT(ok);
        
        // Close this XDR stream
        xdr_destroy(&xdrs);

        // Return the performance data header and data to the caller
        return out;
    }
    
} } // namespace KrellInstitute::Messages
