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
 * Component that will intercept an input going toward a CBTF
 * network.  When given an input it will pass that input to
 * the network.  If a frequency has been given to the
 * FrequencyIn input, the network will wait until after the time
 * given by the input frequency has passed.  It will also emit the
 * time at which the dependent network was started to the
 * StartTimeOut output.
 * This component will repeat the process if given a restart
 * signal to RestartIn.
 */
template <typename T>
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
        bool circut_input_exists;
        T circut_input;
        /** Default constructor. */
        BeginCircutLogic() :
            Component(Type(typeid(BeginCircutLogic)), Version(1, 0, 0)) {
                frequency.tv_sec = 0;
                frequency.tv_nsec = 0;
                circut_input_exists = false;
                declareInput<std::string>(
                        "CircutInputIn",
                        boost::bind(&BeginCircutLogic::inInputHandler,
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
                declareOutput<T>("CircutInputOut");
                declareOutput<timevalue>("StartTimeOut");
            }

        //Set the network input and possibly set a delayed start for
        //the network if given a frequency.
        void inInputHandler(const T & input) {
            circut_input = input;
            circut_input_exists = true;
            start_circut();
        }

        //Delayed restart of the network.
        void inRestartHandler(const bool & restart) {
            if (restart && circut_input_exists)
                start_circut();
        }

        //Set the delay for the network and possibly set the delayed start
        //if the input has been given.
        void inFrequencyHandler(const struct timespec & freq) {
            frequency.tv_sec = freq.tv_sec;
            frequency.tv_nsec = freq.tv_nsec;
        }

        //Make a delayed start of the network
        void start_circut() {
            timevalue start_time;
            gettimeofday(&start_time, NULL);
            emitOutput<timevalue>("StartTimeOut", start_time);
            nanosleep(&frequency, NULL);
            emitOutput<T>("CircutInputOut", circut_input);
        }
};

/**
 * Component that intercepts an output from a CBTF network and, if given
 * a start time to StartTimeIn will calculate the elapsed time of the network.
 * This component will also send a restart signal out via RestartOut if a
 * termination signal isn't given, or the termination signal is false.  The
 * termination signal is then passed back out via TermOut.
 */
template <typename T>
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
                start_time.tv_sec = 0;
                start_time.tv_usec = 0;
                declareInput<timevalue>(
                        "StartTimeIn",
                        boost::bind(&EndCircutLogic::inStartTimeHandler,
                                this,
                                _1)
                        );
                declareInput<T>(
                        "CircutOutputIn",
                        boost::bind(&EndCircutLogic::inOutputHandler, this, _1)
                        );
                declareInput<bool> (
                        "TermIn",
                        boost::bind(&EndCircutLogic::inTermHandler, this, _1)
                        );
                declareOutput<T>("CircutOuputOut");
                declareOutput<timevalue>("ElapsedTimeOut");
                declareOutput<bool>("RestartOut");
                declareOutput<bool>("TermOut");
            }

        //Sets the start time, presumably that of the same network
        //the output is coming from.
        void inStartTimeHandler(const timevalue & start) {
            start_time.tv_sec = start.tv_sec;
            start_time.tv_usec = start.tv_usec;
        }

        //Takes the output from the network and passes it back out through
        //CircutOutputOut.  Also calculates the ElapsedTime given the start
        //time.  If a true termination signal hasn't been recieved this will
        //send a restart signal through RestartOut.
        void inOutputHandler(const T & output) {
            if (start_time.tv_sec != 0 || start_time.tv_usec != 0) {
                timevalue elapsed_time = getElapsedTime();
                emitOutput<timevalue>("ElapsedTimeOut", elapsed_time);
            }
            if (!terminate)
                emitOutput<bool>("RestartOut", true);
            emitOutput<T>("CircutOutputOut", output);
        }

        //Saves the value of a termination signal and passes it back out
        //using TermOut.
        void inTermHandler(const bool & term_signal) {
            //If any incoming termination signal is true, then terminate.
            terminate = (terminate || term_signal);
            emitOutput<bool>("TermOut", terminate);
        }

        //Calculate the elapsed time given the start time.
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
