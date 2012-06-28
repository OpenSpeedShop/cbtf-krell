// Again as in any C++ file we start by including the libraries we will 
// need.  We load our boost, Krell and normal helper libraries.
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

// We are still working in the KrellInstitute::CBTF namespace.
using namespace KrellInstitute::CBTF;

// This is the C++ class definition for the component called getPID.
// This component will take the name of an MPI application as a string 
// and output a list, as a vector of strings, of all PIDs on the node 
// that have that name.  The stack tool then feeds this to the input of 
// a component that takes a list of PIDs and outputs a stack trace for 
// each of those PIDs.
class __attribute__ ((visibility ("hidden"))) getPID :
    public Component
{

// Remember most of this code is the same for each component so the 
// only thing you need to change for a new component so far is the name 
// getPID above and below.
public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new getPID())
        );
    }

// This part is important, here is where you define the inputs and 
// outputs to this component.  You use the functions declareInput 
// or declareOutput to define a new input/output.  You include they 
// C++ type for that input/output in the <>.  Then you name that 
// input/output in "", that name will be used in the XML file.  
// They should be unique with in a component but you can have multiple
//  component with an input called "in".  Although it may be helpful 
//  to be a little more descriptive when dealing with more complicated 
//  components.  At the moment there is no naming convention in CBTF. 
//  For the input you then need to bind the input to a function (a 
//  method of this class).
private:
    /** Default constructor. */
    getPID() :
        Component(Type(typeid(getPID)), Version(1, 0, 0))
    {
        declareInput<std::string>(
            "in", boost::bind(&getPID::inHandler, this, _1)
            );
        declareOutput<std::vector<std::string> >("out");
    }

// This is the function we have bound to the input for this component.  
// Notice that it takes one argument with the type defined in declareInput 
// above.  These input handler functions are where you add your code, 
// this is the part the tells the component what to do with the input.
    /** Handler for the "in" input.*/
    void inHandler(const std::string& in)
    { 
// Remember that instances of this component that will be run on all of 
// the backend nodes but any single component will only be running on 
// one node.  So you just need to write your code as if you are dealing 
// with one node and CBTF will handle dealing with the many nodes your 
// tool will run on.  Below we setup some variables used for this component.
      char buffer[100];
      memset(&buffer,0,sizeof(buffer));
      FILE *p = NULL;
      std::string outline = "";

// Here we setup the output variable with the same type as we stated in 
// the declareOutput function above.  This is the variable we send as output 
// at the end of this function.  We also prepare a command line that will get 
// a list of PIDs with the name that was sent down the tree.  Remember this 
// component only needs to get the PID list from the node it is running on.
      std::vector<std::string> output;
      std::string cmd = "ps -u $USER -o pid= -o comm= | grep ";
      cmd += in; 
      cmd += " | awk -F \" \" '{print $1}'";

// Here we use the normal C popen function to run the command on the node 
// and store the output PIDs in the output variable.
      // get cmd
      p = popen(cmd.c_str(), "r");
      if(p != NULL) {
	while(fgets(buffer, sizeof(buffer), p) != NULL)
        {
          outline.assign(buffer);
          outline = outline.substr(0,outline.rfind("\n"));
          output.push_back(outline);
        }
        pclose(p);
      } //end if p

// Once we have the PID list in the output variable we use the CBTF 
// function emitOutput to send the output to where ever "out" is connected to.
      emitOutput<std::vector <std::string> >("out", output ); 
    }
}; // end class getPID

// This macro is needed to end the definition of the component.
KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(getPID)


/**
 *  * Component type used by the unit test for the Component class.
 *   */
class __attribute__ ((visibility ("hidden"))) getStack :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new getStack())
        );
    }

private:
    /** Default constructor. */
    getStack() :
        Component(Type(typeid(getStack)), Version(1, 0, 0))
    {
        declareInput<std::vector<std::string> >(
            "in", boost::bind(&getStack::inHandler, this, _1)
            );
        declareOutput<std::vector<std::string> >("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::vector<std::string>& in)
    { 
      char buffer[100];
      memset(&buffer,0,sizeof(buffer));
      FILE *p = NULL;
      std::string outline = "";
      std::vector<std::string> output;
      std::string hostname = "";

      // get hostname
      std::string cmd = "hostname";

      // get cmd
      p = popen(cmd.c_str(), "r");
      if(p != NULL) {
	while(fgets(buffer, sizeof(buffer), p) != NULL)
        {
          hostname.assign(buffer);
        }
        pclose(p);
      } //end if p

      //loop over each pid and get stack trace
      for(std::vector<std::string>::const_iterator pid = in.begin(); 
        pid != in.end(); ++pid)
      {
        // add hostname(pid) to the output
        outline = hostname.substr(0,hostname.rfind("\n"));
        outline += "(";
        //outline += pid->substr(0,pid->rfind("\n"));
        outline += *pid;
        outline += ")\n";
        output.push_back(outline);

        // set cmd to gstack pid
        std::string cmd = "gstack ";
        cmd += *pid;
        cmd += " | sed -e s/'0x[A-Za-z0-9]*'/###/g -e s/'LWP [0-9]*'/'LWP ###'/g -e s/'[0-9].[0-9].[0-9].[0-9].[0-9].[0-9]*'/###/g";

        // get gstack
        p = NULL;
        p = popen(cmd.c_str(), "r");
        if(p != NULL) {
          while(fgets(buffer, sizeof(buffer), p) != NULL)
          {
            outline.assign(buffer);
            output.push_back(outline);
          }
          pclose(p);
        } //end if p

        // add a delimiter to the end of each stack
        output.push_back("STOP");
      }//end for

      emitOutput<std::vector <std::string> >("out", output ); 
    }
}; // end class getStack

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(getStack)

struct stackGroup {
  std::string name;
  std::vector<std::string> stack;
};

// check to see if two stacks (vector<string>) are the same
int equalStack(std::vector<std::string> vec1, std::vector<std::string> vec2)
{
  int ret = 0;
  // are they the smae lenght?
  if(vec1.size() == vec2.size())
  {
    ret = 1;
//    for(std::vector<std::string>::const_iterator str1 = vec1.begin(), std::vector<std::string>::const_iterator str2 = vec2.begin(); str1 != vec1.end(); ++str1, ++str2)
    for(int i = 0; i < vec1.size(); i++)
    {
      // make sure each line is the same
      // if not stop looking and return 0
//      if(*str1 != *str2)
      if(vec1[i] != vec2[i])
      {
        ret = 0;
        break;
      }
    }//end for
  }//end if size == size

  return ret;
}

/**
 *  * Filter used to group the same stack traces together
 *   */
class __attribute__ ((visibility ("hidden"))) groupStack :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new groupStack())
        );
    }

private:
    // variables
    int runNum;
    int nodes;
    std::vector<std::string> output;
    std::vector<stackGroup> tempIn;
    std::vector<stackGroup> tempOut;

    /** Default constructor. */
    groupStack() :
        Component(Type(typeid(groupStack)), Version(1, 0, 0))
    {
        declareInput<std::vector<std::string> >(
            "in", boost::bind(&groupStack::inHandler, this, _1)
            );
        declareOutput<std::vector<std::string> >("out");

        // variables
        runNum = 0;
        nodes = 1;
        output.clear();
        tempIn.clear();
        tempOut.clear();
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::vector<std::string>& in)
    { 
      std::string name;
      std::string tmpstr;
      stackGroup *stk;
      int stack = 0;
      int match = 0;

      //reset the output on the first run
      //can't reset it after you emitoutput because because it is bound to it
      if(runNum == 0) {
        output.clear();
        tempIn.clear();
        tempOut.clear();
      }
      // only emit output when all nodes have replied.
      runNum++;

      // save the in for each node in inVec then search it when all nodes 
      // have responded
      for(std::vector<std::string>::const_iterator str = in.begin(); str != in.end(); ++str)
      {
        // are we reading the stack or the hostname or STOP
        // if stack = 0 this is the hostname
        // make a new stackGroup and save the hostname then set the stack
        //   var so you continue reading in the stack until you hit STOP 
        if(stack == 0) 
        {
          // make a new stackGroup for this stack trace
          stk = new stackGroup;
          // save the hostname(PID)
          stk->name = *str;
          // set it up to now read in the stack
          stack = 1;
        }
        else 
        { // stack == 1 so read in the stack
          // the stack for this hostname(PID) stops at STOP
          if(*str != "STOP")
          {  // this is the stack save it in stk.stack      
            stk->stack.push_back(*str);
          }
          else 
          { // we hit STOP then end of this stack
            // save the stackGroup stk
            tempIn.push_back(*stk);
            // reset stack var to get hostname of next stack
            stack = 0;
          }//end else *str == STOP
        }//end else stack
      }//end for in

      // once all nodes have responded search through the inVec to group
      // together the same stack traces
      if(runNum >= nodes) {
        for(std::vector<stackGroup>::const_iterator stkIn = tempIn.begin(); stkIn != tempIn.end(); ++stkIn)
        {  // loop through tempIn group similar stacks together in tempOut 
          // reset match
          match = 0;
          for(std::vector<stackGroup>::iterator stkOut = tempOut.begin(); stkOut != tempOut.end(); ++stkOut)
          {
            //are these the same stacks?
            if( equalStack(stkIn->stack,stkOut->stack) )
            {
              tmpstr = "";
              tmpstr += stkOut->name.substr(0,stkOut->name.rfind("\n"));
              tmpstr += " ";
              tmpstr += stkIn->name.substr(0,stkIn->name.rfind("\n"));
              tmpstr += "\n";
              stkOut->name = tmpstr;
              match = 1;
              break;
            }
          }//end for tempOut

          // if match == 0 the this is a unique stack and need to save it
          if(match == 0)
          {
            tempOut.push_back(*stkIn);
          }
        }//end for stkIn
      }//end if runNum >= nodes

      // convert the tempOut vector of stackGroups to a flat vector of strings
      for(std::vector<stackGroup>::const_iterator stkOut = tempOut.begin(); stkOut != tempOut.end(); ++stkOut)
      {
        output.push_back(stkOut->name);
        for(std::vector<std::string>::const_iterator str = stkOut->stack.begin(); str != stkOut->stack.end(); ++str)
        {
          output.push_back(*str);
        }//end for str
        
        output.push_back("----------\n");
      }//end for stkOut

      emitOutput<std::vector <std::string> >("out", output ); 
    }
}; // end class groupStack

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(groupStack)
