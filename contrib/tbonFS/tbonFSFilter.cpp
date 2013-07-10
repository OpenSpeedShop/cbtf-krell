////////////////////////////////////////////////////////////////////////////////
// tbonFSFilter.cpp defines the filter components for the tbonFS tool
// LACC #:  LA-CC-13-051
// Copyright (c) 2013 Michael Mason; HPC-3, LANL
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
////////////////////////////////////////////////////////////////////////////////


#include <boost/bind.hpp>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sys/param.h>
#include <string>
#include <vector>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

using namespace KrellInstitute::CBTF;

/**
 *  * catVec filter
 *   */
class __attribute__ ((visibility ("hidden"))) tbonFScatVec :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new tbonFScatVec())
        );
    }

private:
    // variables
    int runNum;
    int nodes;
    std::vector<std::string> output;

    /** Default constructor. */
    tbonFScatVec() :
        Component(Type(typeid(tbonFScatVec)), Version(1, 0, 0))
    {
        declareInput<std::vector<std::string> >(
            "in", boost::bind(&tbonFScatVec::inHandler, this, _1)
            );
        declareOutput<std::vector<std::string> >("out");

        // variables
        runNum = 0;
        nodes = 2;
        output.clear();
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::vector<std::string>& in)
    { 
      //reset the output on the first run
      //can't reset it after you emitoutput because because it is bound to it
      if(runNum == 0) {
        output.clear();
      }
      // only emit output when all nodes have replied.
      runNum++;
      
      // concat the in to the output
      for(std::vector<std::string>::const_iterator str = in.begin(); 
	   str != in.end(); ++str)
      {
        output.push_back(*str);
      }

      // once all nodes have responded emitOutput
      if(runNum >= nodes) {
        emitOutput<std::vector<std::string> >("out", output ); 
        //reset the counter
        runNum = 0;
      }
    }
}; // end class tbonFScatVec

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(tbonFScatVec)


