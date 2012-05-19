////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 Krell Institute. All Rights Reserved.
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

/** @file Plugin components. */

#include <boost/bind.hpp>
#include <boost/operators.hpp>
#include <mrnet/MRNet.h>
#include <typeinfo>
#include <algorithm>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>
#if 0
#include <KrellInstitute/CBTF/Impl/MRNet.hpp>
#endif

#include "KrellInstitute/Core/Blob.hpp"
#include "KrellInstitute/Core/PCData.hpp"
#include "KrellInstitute/Messages/Blob.h"

using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

namespace {
}

/**
 * Component that aggregates address values and their counts.
 */
class __attribute__ ((visibility ("hidden"))) BlobComponent :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new BlobComponent())
            );
    }

private:

    /** Default constructor. */
    BlobComponent() :
        Component(Type(typeid(BlobComponent)), Version(0, 0, 1))
    {
        declareInput<Blob>(
            "blob", boost::bind(&BlobComponent::blobHandler, this, _1)
            );
        declareInput<boost::shared_ptr<CBTF_Protocol_Blob> >(
            "cbtf_protocol_blob_in",
            boost::bind(
                &BlobComponent::cbtf_protocol_blob_Handler, this, _1
                )
            );
	declareOutput<Blob>("blob");
	declareOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("blob_xdr_out");
    }

    /** Handler for the "CBTF_Protocol_Blob" input.*/
    void cbtf_protocol_blob_Handler(const boost::shared_ptr<CBTF_Protocol_Blob>& in)
    {
#if 0
std::cerr << "cbtf_protocol_blob_Handler TheTopologyInfo:"
<< "\nRank " << KrellInstitute::CBTF::Impl::TheTopologyInfo.Rank
<< "\nNumChildren " << KrellInstitute::CBTF::Impl::TheTopologyInfo.NumChildren
<< "\nNumSiblings " << KrellInstitute::CBTF::Impl::TheTopologyInfo.NumSiblings
<< "\nNumDescendants " << KrellInstitute::CBTF::Impl::TheTopologyInfo.NumDescendants
<< "\nNumLeafDescendants " << KrellInstitute::CBTF::Impl::TheTopologyInfo.NumLeafDescendants
<< "\nRootDistance " << KrellInstitute::CBTF::Impl::TheTopologyInfo.RootDistance
<< "\nMaxLeafDistance " << KrellInstitute::CBTF::Impl::TheTopologyInfo.MaxLeafDistance
<< std::endl;
#endif

	std::cerr << "ENTER BlobComponent::cbtf_protocol_blob_Handler" << std::endl;
        CBTF_Protocol_Blob *B = in.get();
	Blob myblob(B->data.data_len, B->data.data_val);

	if (in->data.data_len == 0 ) {
	    std::cerr << "EXIT BlobComponent::cbtf_protocol_blob_Handler: data length 0" << std::endl;
	    abort();
	}

	std::cerr << "BlobComponent::cbtf_protocol_blob_Handler: emit output on blob_xdr_out" << std::endl;
	emitOutput<boost::shared_ptr<CBTF_Protocol_Blob> >("blob_xdr_out",in);
    }

    /** Handler for the "in3" input.*/
    void blobHandler(const Blob& in)
    {
        emitOutput<Blob>("blob",  in);
    }

}; // class BlobComponent

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(BlobComponent)
