#include <boost/bind.hpp>
#include <boost/thread.hpp>
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

//boost::shared_ptr<int> topTog = new int;
//*topTog = 0;
int topTog = 0;

/**
 *  * top Component 
 *   */
class __attribute__ ((visibility ("hidden"))) tbonFStop :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new tbonFStop())
        );
    }

private:
    // prt
    //boost::shared_ptr<int> topptr = topTog;

    /** Default constructor. */
    tbonFStop() :
        Component(Type(typeid(tbonFStop)), Version(1, 0, 0))
    {
        declareInput<std::string>(
            "in", boost::bind(&tbonFStop::inHandler, this, _1)
            );
        declareOutput<std::vector<std::string> >("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::string& in)
    { 
std::cout << "---tbonFStop start\n";
      std::vector<std::string> output;
      char buffer[100];
      memset(&buffer,0,sizeof(buffer));
      FILE *p = NULL;
      std::string outline = "";

      // if topTog = 1 then do top else stop
      while(topTog)
      {
std::cout << " start loop topTog = " << topTog << std::endl;
        output.clear();
        // get hostname
        p = NULL;
        p = popen("hostname; top -b -n 1 -u $USER", "r");
        if(p != NULL) {
  	  while(fgets(buffer, sizeof(buffer), p) != NULL)
          {
            outline.assign(buffer);
            output.push_back(outline);
          }
          pclose(p);
        } //end if p

        emitOutput<std::vector <std::string> >("out", output );
        sleep(3); 
      }//end while
      
      output.clear();
      output.push_back("Done");
      emitOutput<std::vector <std::string> >("out", output );
std::cout << "---tbonFStop exit\n";
    }
}; // end class tbonFStop

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(tbonFStop)

/**
 *  * topToggle Component 
 *   */
class __attribute__ ((visibility ("hidden"))) tbonFStopToggle :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new tbonFStopToggle())
        );
    }

private:
    // prt
//    boost::shared_ptr<int> topTogptr = topTog;

    /** Default constructor. */
    tbonFStopToggle() :
        Component(Type(typeid(tbonFStopToggle)), Version(1, 0, 0))
    {
        declareInput<std::string>(
            "in", boost::bind(&tbonFStopToggle::launchThread, this, _1)
            );
        declareOutput<std::string>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::string& in)
    { 
std::cout << "---tbonFStopToggle start\n" << std::flush;
      std::string out = in;

      //toggle topTog
      if(topTog == 1) {
std::cout << " set topTog = 0\n";
        topTog = 0;
      }
      else {
std::cout << " set topTog = 1\n";
        topTog = 1;
      }

      emitOutput<std::string>("out", out ); 
std::cout << "---tbonFStopToggle end\n" << std::flush;
    }

    void launchThread(const std::string& in)
    {
      //boost::thread newT(boost::bind(&tbonFStopToggle::inHandler, in));
      boost::thread newT(&tbonFStopToggle::inHandler,this,in);
    }
}; // end class tbonFStopToggle

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(tbonFStopToggle)

/**
 *  * grep Component 
 *   */
class __attribute__ ((visibility ("hidden"))) tbonFSgrep :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new tbonFSgrep())
        );
    }

private:
    /** Default constructor. */
    tbonFSgrep() :
        Component(Type(typeid(tbonFSgrep)), Version(1, 0, 0))
    {
        declareInput<std::vector<std::string> >(
            "in", boost::bind(&tbonFSgrep::inHandler, this, _1)
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
      std::string cmd = "hostname; grep ";
      cmd += in[0];
      cmd += " ";
      cmd += in[1];

      // get cmd
      p = popen(cmd.c_str(), "r");
      if(p != NULL) {
	while(fgets(buffer, sizeof(buffer), p) != NULL)
        {
          outline.assign(buffer);
          output.push_back(outline);
        }
        pclose(p);
      } //end if p 

      emitOutput<std::vector <std::string> >("out", output ); 
    }
}; // end class tbonFSgrep

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(tbonFSgrep)

/**
 *  * tail Component 
 *   */
class __attribute__ ((visibility ("hidden"))) tbonFStail :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new tbonFStail())
        );
    }

private:
    /** Default constructor. */
    tbonFStail() :
        Component(Type(typeid(tbonFStail)), Version(1, 0, 0))
    {
        declareInput<std::string>(
            "in", boost::bind(&tbonFStail::inHandler, this, _1)
            );
        declareOutput<std::vector<std::string> >("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::string& in)
    { 
///
std::cout << "---Tail set topTog = 0\n" << std::flush;
topTog = 0;
//
      char buffer[100];
      memset(&buffer,0,sizeof(buffer));
      FILE *p = NULL;
      std::string outline = "";
      std::vector<std::string> output;
      std::string cmd = "hostname; tail ";
      cmd += in;

      // get cmd
      p = popen(cmd.c_str(), "r");
      if(p != NULL) {
	while(fgets(buffer, sizeof(buffer), p) != NULL)
        {
          outline.assign(buffer);
          output.push_back(outline);
        }
        pclose(p);
      } //end if p

      emitOutput<std::vector <std::string> >("out", output ); 
    }
}; // end class tbonFStail

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(tbonFStail)

/**
 *  * read Component 
 *   */
class __attribute__ ((visibility ("hidden"))) tbonFSread :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new tbonFSread())
        );
    }

private:
    /** Default constructor. */
    tbonFSread() :
        Component(Type(typeid(tbonFSread)), Version(1, 0, 0))
    {
        declareInput<std::string>(
            "in", boost::bind(&tbonFSread::inHandler, this, _1)
            );
        declareOutput<std::vector<std::string> >("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::string& in)
    { 
      char buffer[100];
      memset(&buffer,0,sizeof(buffer));
      FILE *p = NULL;
      std::string outline = "";
      std::vector<std::string> output;
      // get hostname
      p = popen("hostname", "r");
      if(p != NULL) {
	while(fgets(buffer, sizeof(buffer), p) != NULL)
        {
          outline.assign(buffer);
          output.push_back(outline);
        }
        pclose(p);
      } //end if p

      std::string line;
      std::ifstream readfile(in.c_str());
      if(readfile.is_open())
      {
        while(readfile.good())
        {
          getline(readfile,line);
          output.push_back(line);
        }
        readfile.close();
      } else output.push_back(" Unable to open file");

      emitOutput<std::vector <std::string> >("out", output ); 
    }
}; // end class tbonFSread

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(tbonFSread)

/**
 *  * write Component 
 *   */
class __attribute__ ((visibility ("hidden"))) tbonFSwrite :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new tbonFSwrite())
        );
    }

private:
    /** Default constructor. */
    tbonFSwrite() :
        Component(Type(typeid(tbonFSwrite)), Version(1, 0, 0))
    {
        declareInput<std::vector<std::string> >(
            "in", boost::bind(&tbonFSwrite::inHandler, this, _1)
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
      // get hostname
      p = popen("hostname", "r");
      if(p != NULL) {
	while(fgets(buffer, sizeof(buffer), p) != NULL)
        {
          outline.assign(buffer);
          output.push_back(outline);
        }
        pclose(p);
      } //end if p

      std::ofstream file(in[0].c_str());
      if(file.is_open())
      {
        file << in[1].c_str();
        file.close();
        output.push_back(" file written");
      } else output.push_back(" Unable to open file");

      emitOutput<std::vector<std::string> >("out", output ); 
    }
}; // end class tbonFSwrite

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(tbonFSwrite)

/**
 *  * cp Component 
 *   */
class __attribute__ ((visibility ("hidden"))) tbonFScp :
    public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
          reinterpret_cast<Component*>(new tbonFScp())
        );
    }

private:
    /** Default constructor. */
    tbonFScp() :
        Component(Type(typeid(tbonFScp)), Version(1, 0, 0))
    {
        declareInput<std::vector<std::string> >(
            "in", boost::bind(&tbonFScp::inHandler, this, _1)
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
      std::string cmd = "hostname; cp ";
      cmd += in[0];
      cmd += " ";
      cmd += in[1];

      // get cmd
      p = popen(cmd.c_str(), "r");
      if(p != NULL) {
	while(fgets(buffer, sizeof(buffer), p) != NULL)
        {
          outline.assign(buffer);
          output.push_back(outline);
        }
        pclose(p);
      } //end if p

      emitOutput<std::vector<std::string> >("out", output ); 
    }
}; // end class tbonFScp

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(tbonFScp)

