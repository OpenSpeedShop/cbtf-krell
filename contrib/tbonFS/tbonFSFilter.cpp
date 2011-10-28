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


