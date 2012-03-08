#include <boost/bind.hpp>
#include <typeinfo>
#include <iostream>
#include <stdio.h>
#include <sys/param.h>
#include <string>
#include <sstream>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <cmath>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

typedef struct timeval timevalue;

using namespace KrellInstitute::CBTF;

/**
 *  * Component type used by the unit test for the Component class.
 *   */
class __attribute__ ((visibility ("hidden"))) memPlugin :
public Component
{

    public:
        /** Factory function for this component type. */
        static Component::Instance factoryFunction()
        {
            return Component::Instance(
                    reinterpret_cast<Component*>(new memPlugin())
                    );
        }

    private:
        /** Default constructor. */
        memPlugin() :
            Component(Type(typeid(memPlugin)), Version(1, 0, 0))
    {
        declareInput<std::vector<std::string> >(
                "in", boost::bind(&memPlugin::inHandler, this, _1)
                );
        declareOutput<std::vector<std::string> >("out");
        declareOutput<bool>("TermOut");
    }

        /** Handler for the "in" input.*/
        void inHandler(const std::vector<std::string>& in)
        { 
            char buffer[100];
            memset(&buffer,0,sizeof(buffer));
            FILE *p = NULL;
            std::string outline = "";
            std::string tmpstr = "";
            std::vector<std::string> output;
            std::string cmd = "";
            float totalmem = 0.0;
            float percentmem = 0.0;
            float pidmem = 0.0;
            char buf[10];
            int result = 0;

            // get hostname
            cmd = "hostname";
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

            // get total memory for calculation
            cmd = "free -m | grep Mem | awk -F \" \" '{print $2}'";
            p = NULL;
            p = popen(cmd.c_str(), "r");
            if(p != NULL) {
                while(fgets(buffer, sizeof(buffer), p) != NULL)
                {
                    outline.assign(buffer);
                    //outline = outline.substr(0,outline.rfind("\n"));
                    std::stringstream memStream(outline);
                    memStream >> totalmem;
                    //std::cout << "totalmem = " << totalmem << std::endl;
                }
                pclose(p);
            } //end if p

            // get free
            cmd = "free -m | head -n 2";
            p = NULL;
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

            output.push_back("pid \t%mem \tmem(MB)");

            //loop over each pid and ps -o %mem
            for(std::vector<std::string>::const_iterator pid = in.begin(); 
                    pid != in.end(); ++pid)
            {
                // get %mem from ps ps -p PID -o %mem
                cmd = "ps -p ";
                cmd += *pid;
                cmd += " -o %mem=";

                tmpstr = *pid + " \t";

                p = NULL;
                p = popen(cmd.c_str(), "r");
                if(p != NULL) {
                    while(fgets(buffer, sizeof(buffer), p) != NULL)
                    {
                        outline.assign(buffer);
                        outline = outline.substr(0,outline.rfind("\n"));
                        std::stringstream percentStream(outline);
                        percentStream >> percentmem;
                        //std::cout << "outline=" << outline << " percentmem=" << percentmem << std::endl;
                        percentmem = percentmem / 100;
                        //std::cout << "percentmem/100=" << percentmem << std::endl;
                    }
                    pclose(p);
                    tmpstr += outline;
                    tmpstr += " \t";
                    //calculate mem from %mem and total mem
                    //!!! totalmem percentmem
                    pidmem = totalmem * percentmem;
                    //std::cout << "pidmem=" << pidmem << std::endl;
                    result = snprintf(buf, 10, "%f", pidmem);
                    tmpstr += buf;
                    tmpstr += "(MB)";
                    output.push_back(tmpstr);
                    //std::cout << "tmpstr = " << tmpstr << std::endl;
                } //end if p
            } // end for pid

            emitOutput<std::vector<std::string> >("out", output ); 
        }
}; // end class memPlugin

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(memPlugin)

/**
 * Component type used by the unit test for the Component class.
 */
class __attribute__ ((visibility ("hidden"))) getPID :
public Component
{

public:
    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
                reinterpret_cast<Component*>(new getPID())
                );
    }

private:
    /** Default constructor. */
    getPID() :
        Component(Type(typeid(getPID)), Version(1, 0, 0))
{
    declareInput<std::string>(
            "in", boost::bind(&getPID::inHandler, this, _1)
            );
    declareOutput<std::vector<std::string> >("out");
    declareOutput<bool> ("TermOut");
}

    /** Handler for the "in" input.*/
    void inHandler(const std::string& in)
    { 
        bool terminate;
        char buffer[100];
        //memset(&buffer,0,sizeof(buffer));
        FILE *p = NULL;
        FILE *f = NULL;
        std::string outline = "";
        std::vector<std::string> output;
        std::string cmd = "ps -u $USER -o pid= -o comm= | grep ";
        cmd += in; 
        //      cmd += " | cut -f 1 -d \" \"";
        cmd += " | awk -F \" \" '{print $1}'";   

        //std::cout << cmd.c_str() << std::endl;

        // get cmd
        p = popen(cmd.c_str(), "r");
        if(p != NULL) {
            //std::cout << "opened cmd" << std::endl;   
            while(fgets(buffer, sizeof(buffer), p) != NULL)
            {
                outline.assign(buffer);
                outline = outline.substr(0,outline.rfind("\n"));
                output.push_back(outline);
                //std::cout << "outline=" << outline << std::endl;
            }
            //std::cout << "closing p buffer = " << buffer << std::endl;
            pclose(p);
        } //end if p
        else
        {
            std::cout << "cannot run cmd = " << cmd << std::endl;
        }

        emitOutput<std::vector <std::string> >("out", output ); 
        if(output.size() == 0) {
            terminate = true;
        } else {
            terminate = false;
        }
        emitOutput<bool>("TermOut", terminate);
    }
}; // end class getPID

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(getPID)

class __attribute__((visibility("hidden"))) BeginCircutLogic:
public Component { 
    public:
        /** Factory function for this component type. */
        static Component::Instance factoryFunction() {
            return Component::Instance(
                    reinterpret_cast<Component*>(new BeginCircutLogic())
                    );
        }

    private:
        struct timespec frequency;
        std::string filename;
        /** Default constructor. */
        BeginCircutLogic() :
            Component(Type(typeid(BeginCircutLogic)), Version(1, 0, 0)) {
                frequency.tv_sec = 0;
                frequency.tv_nsec = 0;
                filename = "";
                declareInput<std::string>(
                        "FilenameIn",
                        boost::bind(&BeginCircutLogic::inFilenameHandler,
                                this,
                                _1)
                        );
                declareInput<struct timespec>(
                        "FrequencyIn",
                        boost::bind(&BeginCircutLogic::inFrequencyHandler,
                                this,
                                _1)
                        );
                declareInput<bool>(
                        "RestartIn",
                        boost::bind(&BeginCircutLogic::inRestartHandler,
                                this,
                                _1)
                        );
                declareOutput<std::string>("FilenameOut");
                declareOutput<timevalue>("StartTimeOut");
            }

        void inFilenameHandler(const std::string & fn) {
            filename = fn;
            if (frequency.tv_sec != 0 || frequency.tv_nsec !=0)
                start_circut();
        }

        void inRestartHandler(const bool & restart) {
            if (restart)
                start_circut();
        }

        void inFrequencyHandler(const struct timespec & freq) {
            frequency.tv_sec = freq.tv_sec;
            frequency.tv_nsec = freq.tv_nsec;
            if (filename != "")
                start_circut();
        }

        void start_circut() {
            timevalue start_time;
            gettimeofday(&start_time, NULL);
            emitOutput<timevalue>("StartTimeOut", start_time);
            nanosleep(&frequency, NULL);
            emitOutput<std::string>("FilenameOut", filename);
        }
};

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(BeginCircutLogic)

class __attribute__((visibility("hidden"))) EndCircutLogic:
public Component {
    public:
        static Component::Instance factoryFunction() {
            return Component::Instance(
                    reinterpret_cast<Component*>(new EndCircutLogic())
                    );
        }

    private:
        timevalue start_time;
        bool terminate;
        EndCircutLogic() :
            Component(Type(typeid(EndCircutLogic)), Version(1, 0, 0)) {
                terminate = false;
                declareInput<timevalue>(
                        "StartTimeIn",
                        boost::bind(&EndCircutLogic::inStartTimeHandler,
                                this,
                                _1)
                        );
                declareInput<std::vector <std::string> >(
                        "MemoryInfoIn",
                        boost::bind(&EndCircutLogic::inMemoryInfo, this, _1)
                        );
                declareInput<bool> (
                        "TermIn",
                        boost::bind(&EndCircutLogic::inTerm, this, _1)
                        );
                declareOutput<std::vector <std::string> >("MemoryInfoOut");
                declareOutput<timevalue>("ElapsedTimeOut");
                declareOutput<bool>("RestartOut");
                declareOutput<bool>("TermOut");
            }

        void inStartTimeHandler(const timevalue & start) {
            start_time.tv_sec = start.tv_sec;
            start_time.tv_usec = start.tv_usec;
        }

        void inMemoryInfo(const std::vector<std::string> & mem_info) {
            timevalue elapsed_time = getElapsedTime();
            emitOutput<timevalue>("ElapsedTimeOut", elapsed_time);
            if (!terminate)
                emitOutput<bool>("RestartOut", true);
            emitOutput<std::vector <std::string> >("MemoryInfoOut", mem_info);
        }

        void inTerm(const bool & term_signal) {
            terminate = term_signal;
            emitOutput<bool>("TermOut", terminate);
        }

        timevalue getElapsedTime() {
            long sec;
            long usec;
            timevalue elapsed_time;
            timevalue end_time;
            gettimeofday(&end_time, NULL);
            sec = end_time.tv_sec - start_time.tv_sec;
            if ((usec = end_time.tv_usec - start_time.tv_usec) < 0) {
                usec += 1000000;
                sec -= 1;
                if(sec < 0) {
                    std::cerr << "Elapsed time is negative." << std::endl;
                    sec = 0;
                    usec = 0;
                }
            }
            elapsed_time.tv_sec = sec;
            elapsed_time.tv_usec = usec;
            return elapsed_time;
        }
};

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(EndCircutLogic)
