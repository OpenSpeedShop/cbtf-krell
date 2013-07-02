////////////////////////////////////////////////////////////////////////////////
// psPlugin.cpp defines the components for the psTool
// LACC #:  LA-CC-13-051
// Copyright (C) 2013 Michael Mason; HPC-3, LANL
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
#include <stdio.h>
#include <sys/param.h>
#include <string>
#include <vector>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

typedef std::vector<std::string> sameVec;
typedef std::vector<std::string> diffVec;

using namespace KrellInstitute::CBTF;


class __attribute__ ((visibility ("hidden"))) psFE :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new psFE())
        );
    }

private:
    /** Default constructor. */
    psFE() :
        Component(Type(typeid(psFE)), Version(1, 0, 0))
    {
        declareInput<std::string >(
            "stringin", boost::bind(&psFE::stringinHandler, this, _1)
            );
        declareInput<std::vector<std::string> >(
            "vstringin", boost::bind(&psFE::vstringinHandler, this, _1)
            );
        declareInput<sameVec>(
            "sameVecIn", boost::bind(&psFE::sameVecInHandler, this, _1)
            );
        declareInput<diffVec>(
            "diffVecIn", boost::bind(&psFE::diffVecInHandler, this, _1)
            );
        declareOutput<std::string >("stringout");
        declareOutput<std::vector<std::string> >("vstringout");
        declareOutput<sameVec>("sameVecOut");
        declareOutput<diffVec>("diffVecOut");
    }

    /** Handler for the "stringin" input.*/
    // just pass it on..
    void stringinHandler(const std::string& in)
    { 
        emitOutput<std::string >("stringout", in);
    }

    /** Handler for the "vstringin" input.*/
    // just pass it on..
    void vstringinHandler(const std::vector<std::string>& in)
    { 
	if (in.size() > 0) {
            emitOutput<std::vector<std::string> >("vstringout", in);
	}
    }

    /** Handler for the "sameVec" input.*/
    // just pass it on..
    void sameVecInHandler(const sameVec& in)
    { 
	if (in.size() > 0) {
	    //std::cerr << "psFE::sameVecInHandler emits "
	    //    << in.size() << " values" << std::endl;
            emitOutput<sameVec>("sameVecOut", in);
	}
    }

    /** Handler for the "diffVec" input.*/
    // just pass it on..
    void diffVecInHandler(const diffVec& in)
    { 
	if (in.size() > 0) {
	    //std::cerr << "psFE::diffVecInHandler emits "
	    //    << in.size() << " values" << std::endl;
            emitOutput<diffVec>("diffVecOut", in);
	}
    }
}; // end class psFE

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(psFE)

/**
 *  * Filter to find the same proc's
 *   */
class __attribute__ ((visibility ("hidden"))) psSame :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new psSame())
        );
    }

private:
    // variables
    int runNum;
    int nodes;
    sameVec outVec;
    std::vector<std::vector<std::string> > inVec;

    /** Default constructor. */
    psSame() :
        Component(Type(typeid(psSame)), Version(1, 0, 0))
    {
        declareInput<std::vector <std::string> >(
            "in", boost::bind(&psSame::inHandler, this, _1)
            );
        declareOutput<sameVec>("out");

        // variables
        runNum = 0;
        nodes = 2;
        outVec.clear();
        inVec.clear();
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::vector<std::string>& in)
    { 
      //std::cerr << "HANDLING PSSAME" << std::endl;
      //reset the outVec on the first run
      //can't reset it after you emitoutput because because it is bound to it
      if(runNum == 0) {
        outVec.clear();
        inVec.clear();
      }
      // only emit output when all nodes have replied.
      runNum++;

      // save the ps output for this node
      inVec.push_back(in);

      // once all nodes have responded search through the inVec to see
      // what procs are the same on all nodes
      if(runNum >= nodes) {
        // check if a proc on one node is running on any other
        // grab each vector of procs
        int found;
        for(std::vector<std::vector<std::string> >::const_iterator
               procVec = inVec.begin();
               procVec != inVec.end();
               ++procVec)
        {
          // take one proc
          for(std::vector<std::string>::const_iterator proc = procVec->begin(); 
	        proc != procVec->end(); ++proc)
          {
            found = 0;
            // for all the other vector of procs
            for(std::vector<std::vector<std::string> >::const_iterator
                    procVecOther = inVec.begin();
                    procVecOther != inVec.end();
                    ++procVecOther)
            {
              // don't check itself
              if(procVecOther != procVec)
              {
                found = 0;
                // look at each of the procs in the other vectors
                for(std::vector<std::string>::const_iterator
                        procOther = procVecOther->begin();
                        procOther != procVecOther->end();
                        ++procOther)
                {
                // check to see if the original proc is the same as any other
                // if it is stop looking
                if(*proc == *procOther)
                {
                    found = 1;
                    break;
                  } 
                }  //procOther

                // if the proc was found then continue checking the others
                // else don't bother searching the others
                if(found == 0) 
                {
                  break;
                }
              }// if procVecOther != procVec
            } //procVecOther

            // after looking through all the procs on the other nodes
            // if found is 1 then it was found on every node
            // if found is 0 then it was not on every node
            if(found) 
            {
              // this proc was found on every node so save it
              // but don't save it twice
              for(sameVec::const_iterator outProc = outVec.begin();
                      outProc != outVec.end(); ++outProc) 
              {
                // if the proc is already in the list don't save it
                if(*outProc == *proc)
                {
                  found = 0;
                  break;
                }
              }

              // this proc hasn't been saved yet so save it
              if(found)
              {
                outVec.push_back(*proc);
              }
            }// end found
          } // proc
        } //procVec

        // output the outVec
	if (outVec.size() > 0 ) {
	    //std::cerr << "PSSAME found " << outVec.size()
	    //    << " same procs." << std::endl;
            emitOutput<sameVec>("out", outVec);
	}
        //reset the counter
        runNum = 0;
      }
    }
}; // end class psSame

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(psSame)


/**
 *  * Filter to fine the diff proc's
 *   */
class __attribute__ ((visibility ("hidden"))) psDiff :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new psDiff())
        );
    }

private:
    // variables
    int runNum;
    int nodes;
    diffVec outVec;
    std::vector<std::vector<std::string> > inVec;

    /** Default constructor. */
    psDiff() :
        Component(Type(typeid(psDiff)), Version(1, 0, 0))
    {
        declareInput<std::vector <std::string> >(
            "in", boost::bind(&psDiff::inHandler, this, _1)
            );
        declareOutput<diffVec>("out");

        // variables
        runNum = 0;
        nodes = 2;
        outVec.clear();
        inVec.clear();
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::vector<std::string>& in)
    { 
      //std::cerr << "HANDLING PSDIFF" << std::endl;
      //reset the outVec on the first run
      //can't reset it after you emitoutput because because it is bound to it
      if(runNum == 0) {
        outVec.clear();
        inVec.clear();
      }
      // only emit output when all nodes have replied.
      runNum++;

      // save the ps output for this node
      inVec.push_back(in);

      // once all nodes have responded search through the inVec to see
      // what procs are the same on all nodes
      if(runNum >= nodes) {
        // check if a proc on one node is running on any other
        // grab each vector of procs
        int found;
        for(std::vector<std::vector<std::string> >::const_iterator
                procVec = inVec.begin();
                procVec != inVec.end();
                ++procVec)
        {
          // take one proc
          for(std::vector<std::string>::const_iterator proc = procVec->begin(); 
	        proc != procVec->end(); ++proc)
          {
            found = 0;
            // for all the other vector of procs
            for(std::vector<std::vector<std::string> >::const_iterator
	           procVecOther = inVec.begin();
                   procVecOther != inVec.end();
                   ++procVecOther)
            {
              // don't check itself
              if (procVecOther != procVec)
              { 
                found = 0;
                // look at each of the procs in the other vectors
                for(std::vector<std::string>::const_iterator
                    procOther = procVecOther->begin();
                    procOther != procVecOther->end();
		    ++procOther)
                {
                  // check to see if the original proc is the same as any other
                  // if it is stop looking
                  if(*proc == *procOther)
                  {
                    found = 1;
                    break;
                  } 
                }  //procOther

                // if the proc was found stop don't want it
                // else continue checking the others
                if(found == 1) 
                {
                  break;
                }
              } //end if procVecOther != procVec

              if(found == 1) 
                {
                  break;
                }
            } //procVecOther

            // after looking through all the procs on the other nodes
            // if found is 1 then it was found on more then one node don't want
            // if found is 0 then it was only found on 1 node save it
            if(found == 0) 
            {
              // this proc was found on only one node so save it
              //std::cerr << "PSDIFF proc only on one node: " << *proc << std::endl;
              outVec.push_back(*proc);
            }
          } // proc
        } //procVec

        // output the outVec
        if (outVec.size() > 0) {
	    //std::cerr << "PSDIFF found " << outVec.size()
	    //    << " differing procs." << std::endl;
            emitOutput<diffVec>("out", outVec);
	} else {
	    outVec.push_back("NO different procs across nodes.\n");
            emitOutput<diffVec>("out", outVec);
	}
        //reset the counter
        runNum = 0;
      }
    }
}; // end class psDiff

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(psDiff)

///
/**
 *  * component for the ps command proc's
 *   */
class __attribute__ ((visibility ("hidden"))) psCmd :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new psCmd())
        );
    }

private:
    /** Default constructor. */
    psCmd() :
        Component(Type(typeid(psCmd)), Version(1, 0, 0))
    {
        declareInput<std::string>(
            "in", boost::bind(&psCmd::inHandler, this, _1)
            );
        declareOutput<std::vector <std::string> >("out");
    }

    /** Handler for the "in" input.*/
    // Apparently the FE client sends a std::string "start"
    // to this input.
    void inHandler(const std::string& in)
    { 
      char buffer[100];
      memset(&buffer,0,sizeof(buffer));
      FILE *p = NULL;
      std::string outline = "";
      std::vector<std::string> output;

      // get ps
      p = popen("hostname; ps -e -o comm= -o euser=", "r");
      if(p != NULL) {
	while(fgets(buffer, sizeof(buffer), p) != NULL)
        {
          outline.assign(buffer);
          output.push_back(outline);
        }
        pclose(p);
      } //end if p for hostname

      emitOutput<std::vector<std::string> >("out", output );
    }
}; // end class psCmd

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(psCmd)
