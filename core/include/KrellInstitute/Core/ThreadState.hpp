////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2007 William Hachfeld. All Rights Reserved.
// Copyright (c) 2006-2013 The Krell Institute All Rights Reserved.
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
 * Declaration of the ThreadState enum.
 *
 */

#ifndef _KrellInstitute_Core_ThreadState_
#define _KrellInstitute_Core_ThreadState_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "KrellInstitute/Core/ThreadName.hpp"

namespace KrellInstitute { namespace Core {

    enum ThreadState {
            Disconnected,  /**< Thread isn't connected (may not even exist). */
            Connecting,    /**< Thread is being connected. */
            Nonexistent,   /**< Thread doesn't exist. */
            Running,       /**< Thread is active and running. */
            Suspended,     /**< Thread has been temporarily suspended. */
            Terminated     /**< Thread has terminated. */
    };

    typedef std::vector< std::pair<ThreadName,ThreadState> > ThreadNameStateVec;
} }



#endif
