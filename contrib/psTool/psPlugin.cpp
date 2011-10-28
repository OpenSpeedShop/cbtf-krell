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

using namespace KrellInstitute::CBTF;

/**
 *  * Filter to fine the same proc's
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
    std::vector<std::string> outVec;
    std::vector<std::vector<std::string> > inVec;

    /** Default constructor. */
    psSame() :
        Component(Type(typeid(psSame)), Version(1, 0, 0))
    {
        declareInput<std::vector <std::string> >(
            "in", boost::bind(&psSame::inHandler, this, _1)
            );
        declareOutput<std::vector <std::string> >("out");

        // variables
        runNum = 0;
        nodes = 2;
        outVec.clear();
        inVec.clear();
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::vector<std::string>& in)
    { 
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
        for(std::vector<std::vector<std::string> >::const_iterator procVec = inVec.begin(); procVec != inVec.end(); ++procVec)
        {
          // take one proc
          for(std::vector<std::string>::const_iterator proc = procVec->begin(); 
	        proc != procVec->end(); ++proc)
          {
            found = 0;
            // for all the other vector of procs
            for(std::vector<std::vector<std::string> >::const_iterator procVecOther = inVec.begin(); procVecOther != inVec.end(); ++procVecOther)
            {
              // don't check itself
              if(procVecOther != procVec)
              {
                found = 0;
                // look at each of the procs in the other vectors
                for(std::vector<std::string>::const_iterator procOther = procVecOther->begin(); procOther != procVecOther->end(); ++procOther)
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
              for(std::vector<std::string>::const_iterator outProc = outVec.begin(); outProc != outVec.end(); ++outProc) 
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
        emitOutput<std::vector<std::string> >("out", outVec);
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
    std::vector<std::string> outVec;
    std::vector<std::vector<std::string> > inVec;

    /** Default constructor. */
    psDiff() :
        Component(Type(typeid(psDiff)), Version(1, 0, 0))
    {
        declareInput<std::vector <std::string> >(
            "in", boost::bind(&psDiff::inHandler, this, _1)
            );
        declareOutput<std::vector <std::string> >("out");

        // variables
        runNum = 0;
        nodes = 2;
        outVec.clear();
        inVec.clear();
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::vector<std::string>& in)
    { 
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
        for(std::vector<std::vector<std::string> >::const_iterator procVec = inVec.begin(); procVec != inVec.end(); ++procVec)
        {
          // take one proc
          for(std::vector<std::string>::const_iterator proc = procVec->begin(); 
	        proc != procVec->end(); ++proc)
          {
            found = 0;
            // for all the other vector of procs
            for(std::vector<std::vector<std::string> >::const_iterator procVecOther = inVec.begin(); procVecOther != inVec.end(); ++procVecOther)
            {
              // don't check itself
              if (procVecOther != procVec)
              { 
                found = 0;
                // look at each of the procs in the other vectors
                for(std::vector<std::string>::const_iterator procOther = procVecOther->begin(); procOther != procVecOther->end(); ++procOther)
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
              outVec.push_back(*proc);
            }
          } // proc
        } //procVec

        // output the outVec
        emitOutput<std::vector<std::string> >("out", outVec);
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
