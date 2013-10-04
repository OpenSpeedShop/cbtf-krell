#include <boost/bind.hpp>
#include <typeinfo>
#include <stdio.h>
#include <sys/param.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <cmath>
#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <Python.h>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

// typedefs used by this tool
typedef std::string ToolInputArgs;
typedef std::vector<std::string> PYvalues;

using namespace KrellInstitute::CBTF;

namespace {

static bool done = false;

#ifndef NO_DLFCN_HACK
#include <dlfcn.h>
static int dlopen_hacked = 0;
int dlopen_python_hack()
{
    if (!dlopen_hacked) {
        dlopen(LIBPYTHON, RTLD_NOW|RTLD_GLOBAL);
        dlopen_hacked = 1;
    }
}
#else
#define dlopen_python_hack()
#endif

// Parses the value of the active python exception
// NOTE SHOULD NOT BE CALLED IF NO EXCEPTION
std::string parse_python_exception(){
    PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
    // Fetch the exception info from the Python C API
    PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);

    // Fallback error
    std::string ret("Unfetchable Python error");
    // If the fetch got a type pointer, parse the type into the exception string
    if(type_ptr != NULL){
        boost::python::handle<> h_type(type_ptr);
        boost::python::str type_pstr(h_type);
        // Extract the string from the boost::python object
        boost::python::extract<std::string> e_type_pstr(type_pstr);
        // If a valid string extraction is available, use it 
        //  otherwise use fallback
        if(e_type_pstr.check())
            ret = e_type_pstr();
        else
            ret = "Unknown exception type";
    }
    // Do the same for the exception value (the stringification of the exception)
    if(value_ptr != NULL){
        boost::python::handle<> h_val(value_ptr);
        boost::python::str a(h_val);
        boost::python::extract<std::string> returned(a);
        if(returned.check())
            ret +=  ": " + returned();
        else
            ret += std::string(": Unparseable Python error: ");
    }
    // Parse lines from the traceback using the Python traceback module
    if(traceback_ptr != NULL){
        boost::python::handle<> h_tb(traceback_ptr);
        // Load the traceback module and the format_tb function
        boost::python::object tb(boost::python::import("traceback"));
        boost::python::object fmt_tb(tb.attr("format_tb"));
        // Call format_tb to get a list of traceback strings
        boost::python::object tb_list(fmt_tb(h_tb));
        // Join the traceback strings into a single string
        boost::python::object tb_str(boost::python::str("\n").join(tb_list));
        // Extract the string, check the extraction, and fallback in necessary
        boost::python::extract<std::string> returned(tb_str);
        if(returned.check())
            ret += ": " + returned();
        else
            ret += std::string(": Unparseable Python traceback");
    }
    return ret;
}

static void printUsageExample() {
    std::cout << "daemonTool --tool \"path to python xml tool\" --toolargs \"pyexample.py\"" << std::endl;
}

} // namespace


/**
 *
 * Component type used by the unit test for the Component class.
 */
class __attribute__ ((visibility ("hidden"))) PyExampleView :
public Component {

    public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
                reinterpret_cast<Component*>(new PyExampleView())
                );
    }

    private:
    /** Default constructor. */
    PyExampleView() :
        Component(Type(typeid(PyExampleView)), Version(1, 0, 0))
    {
	    // input here is a vector of strings of results.
	    declareInput<std::vector<std::string> >(
                "ResultVecIn", boost::bind(&PyExampleView::resultVecHandler, this, _1)
                );
	    declareOutput<bool>("finished");
    }

    /** Handler for the "in" input.*/
    void resultVecHandler(const std::vector<std::string> & results)
    { 
        for(std::vector<std::string> ::const_iterator si = results.begin();
            si != results.end(); ++si) {
            std::cout << *si << std::endl;;
        }

	done = true;
	emitOutput<bool>("finished", true );
    }
}; // end class PyExampleView

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(PyExampleView)

/**
 *
 * Component type used by the unit test for the Component class.
 */
class __attribute__ ((visibility ("hidden"))) PyExampleFE :
public Component {

    public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
                reinterpret_cast<Component*>(new PyExampleFE())
                );
    }


    private:

    /** Default constructor. */
    PyExampleFE() : Component(Type(typeid(PyExampleFE)), Version(1, 0, 0))
    {
	// input arguments from client (daemonTool).
	declareInput<ToolInputArgs>(
            "args", boost::bind(&PyExampleFE::argsHandler, this, _1)
            );

	// incoming terminate signal
	declareInput<bool> (
            "TerminateIn",
            boost::bind(&PyExampleFE::terminateHandler, this, _1)
            );

	// output got python script to run.
	declareOutput<std::string>("PyScriptNameOut");
	declareOutput<bool>("TerminateOut");
	
    }

    bool terminate;  // when true, shutdown tool.

    /** Handler for the tool "args" input.*/
    void argsHandler(const ToolInputArgs& toolargs)
    { 
	// inititialize termination value
	terminate = false;

	// parse input string
	std::string py_script_name = "";
	std::string tmparg = toolargs;
	std::stringstream argstream(tmparg);
	argstream >> py_script_name;


	// send the python script name to run downstream.
	if( py_script_name == "" ) {
	    std::cerr << "Error must include vaild python script name" << std::endl;
	    printUsageExample();
	    terminate = true;
	    return;
	} else {
            emitOutput<std::string>("PyScriptNameOut", py_script_name ); 
	}

        struct timespec tim, tim2;
        tim.tv_sec = 0;
        tim.tv_nsec = 250000000L;
        while(!terminate) {
            nanosleep(&tim , &tim2);
        };

    }

    // record the termination signal.
    void terminateHandler(const bool & terminate_signal) {
        terminate = terminate_signal;
    }

}; // end class PyExampleFE

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(PyExampleFE)

/**
 * Component type used by the unit test for the Component class.
 */
class __attribute__ ((visibility ("hidden"))) PyExampleBE :
public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
                reinterpret_cast<Component*>(new PyExampleBE())
                );
    }

private:
    /** Default constructor. */
    PyExampleBE() :
        Component(Type(typeid(PyExampleBE)), Version(1, 0, 0))
    {
	// input for executable name to aquire pid(s) for.
        declareInput<std::string>(
            "PyScriptNameIn", boost::bind(&PyExampleBE::PyScriptNameHandler, this, _1)
            );

	// output from script as vector of strings.
        declareOutput<std::vector<std::string> >("PyExampleStrVecOut");
	// output for termination signal.
        declareOutput<bool> ("TerminateOut");
    }

    /** Handler for the "ExeNameIn" input.*/
    void PyScriptNameHandler(const std::string& in)
    { 
        bool be_terminate;
        std::vector<std::string>  rvecout;

	dlopen_python_hack();

	try {

	    Py_Initialize();

	    // using boost::python objects. Generate random number from random module.
	    boost::python::object  main_module = boost::python::import("__main__");
	    boost::python::object  random_module = boost::python::import("random");
	    boost::python::object  random_func = random_module.attr("random");
	    boost::python::object  rand_val = random_func();
	    std::stringstream python_output;
	    double tval = boost::python::extract<double>(rand_val);
	    python_output << "Testing boost python bindings..." << std::endl;
	    python_output << "value from python random func: " << tval << std::endl;
	    rvecout.push_back(python_output.str());

	    // Run a script via PyRun_SimpleFile.
	    FILE* fp = fopen(in.c_str(), "r");
	    PyRun_SimpleFile(fp, in.c_str());
	    Py_Finalize();

	    // Run passed python script via python cmd via popen.
	    char buffer[4096];
	    memset(&buffer,0,sizeof(buffer));
	    std::string cmd = "/usr/bin/python " + in;
	    std::string outline = "";
	    rvecout.push_back("executing python script: " + cmd);
	    FILE* p = popen(cmd.c_str(), "r");
	    if(p != NULL) {
		while(fgets(buffer, sizeof(buffer), p) != NULL)
                {
		    outline.assign(buffer);
		    rvecout.push_back(outline);
                }
                pclose(p);
            }


	} catch(boost::python::error_already_set const &) {
	    std::string perror_str = parse_python_exception();
	    std::cerr << "Error in Python: " << perror_str << std::endl;
	}

	// PYTHON (boost) code here to run script and return output as needed.
        if(rvecout.size() == 0) {
	    // no output... be_terminate.
	    std::cerr << "PyExampleBE::PyScriptNameHandler outputsize is 0, be_terminate = true" <<  std::endl;
            be_terminate = true;
        } else {
	    // emit the output
            emitOutput<std::vector<std::string> >("PyExampleStrVecOut", rvecout ); 
            be_terminate = true;
        }

	// emit the termination signal.
        emitOutput<bool>("TerminateOut", be_terminate);
    }
}; // end class PyExampleBE

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(PyExampleBE)
