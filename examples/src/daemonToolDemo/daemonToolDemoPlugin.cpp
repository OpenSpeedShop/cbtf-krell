////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011 Krell Institute. All Rights Reserved.
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

/** @file Plugins used by the example daemon tool. */

#include <boost/bind.hpp>
#include <mrnet/MRNet.h>
#include <stdio.h>
#include <string>
#include <sys/param.h>
#include <typeinfo>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

using namespace KrellInstitute::CBTF;
using namespace std;



/**
 * Component that executes the specified command.
 *
 * All CBTF components must inherit from the Component class and should be
 * marked with "hidden" visibility. Hiding the class declaration causes its
 * symbols to NOT be exported by the linker. This is necessary in order to
 * allow multiple definitions of the same class, but with different versions,
 * to peacefully co-exist in CBTF.
 */
class __attribute__ ((visibility ("hidden"))) ExecuteCommand :
    public Component
{

public:
    
    /**
     * Factory function for this component type.
     *
     * All CBTF components must have a factory function with this name. This
     * is the means by which CBTF "discovers" the component's type, version,
     * etc. and by which it creates instances of the component.
     */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ExecuteCommand())
            );
    }
    
private:

    /**
     * Default constructor.
     *
     * All CBTF components must have a default constructor. This constructor
     * defines the type of the component, the version of the component, and
     * the input/output names and their types. A binding of inputs to the
     * function that will handle those inputs is also specified at this time.
     */
    ExecuteCommand() :
        Component(Type(typeid(ExecuteCommand)), Version(0, 0, 1))
    {
        declareInput<string>(
            "command", boost::bind(&ExecuteCommand::commandHandler, this, _1)
            );
        declareOutput<vector<string> >("output");
    }
    
    /**
     * Handler for the "command" input.
     *
     * This handler is invoked every time someone provides a value for the
     * "command" input. In this case, we immediately execute the requested
     * command, gather its output, and emit that command's output on the
     * component's output.
     *
     * Note that components don't HAVE to follow this precise model. Inputs
     * can be queued or even ignored. Components may also emit outputs in a
     * completely asynchronous manner. For example, the defaul constructor
     * might create a thread which monitors, say, /proc, and emits an output
     * every time a particular process' CPU usage exceeds 50%.
     */
    void commandHandler(const string& command)
    {
        vector<string> output;
        
        char hostname[MAXHOSTNAMELEN];
        gethostname(hostname, MAXHOSTNAMELEN);

        output.push_back(string(
            "Output of \"" + command + "\" from host \"" + hostname + "\"."
            ));
            
        FILE *fd = popen(command.c_str(), "r");
        
        if (fd != NULL)
        {
            char buffer[BUFSIZ];
            memset(&buffer, 0, sizeof(buffer));

            while (fgets(buffer, sizeof(buffer), fd))
            {
                string line(buffer);

                if (!line.empty())
                {
                    if (line[line.length() - 1] == '\n')
                    {
                        line.erase(line.length() - 1);
                    }
                    output.push_back(line);
                }
            }
            pclose(fd);
        }
        else 
        {
            output.push_back(string("Error executing this command!"));
        }
        
        emitOutput<vector<string> >("output", output);
    }
    
}; // class ExecuteCommand

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ExecuteCommand)



/**
 * Component that converts a string into a MRNet packet.
 */
class __attribute__ ((visibility ("hidden"))) ConvertStringToPacket :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertStringToPacket())
            );
    }

private:

    /** Default constructor. */
    ConvertStringToPacket() :
        Component(Type(typeid(ConvertStringToPacket)), Version(0, 0, 1))
    {
        declareInput<string>(
            "in", boost::bind(&ConvertStringToPacket::inHandler, this, _1)
            );
        declareOutput<MRN::PacketPtr>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const string& in)
    {
        emitOutput<MRN::PacketPtr>(
            "out", MRN::PacketPtr(new MRN::Packet(0, 0, "%s", in.c_str()))
            );
    }

}; // class ConvertStringToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertStringToPacket)



//
// The remainder of the components in this file are converison components that
// translate several datatypes to/from a MRNet packet. These are used in order
// to bind various component inputs and outputs to the upward and downward
// streams that MRNet provides.
//
// Their implementation is very similar in nature to the above component, so
// they have not been extensively documented.
//



/**
 * Component that converts a vector<string> into a MRNet packet.
 */
class __attribute__ ((visibility ("hidden"))) ConvertStringListToPacket :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertStringListToPacket())
            );
    }

private:

    /** Default constructor. */
    ConvertStringListToPacket() :
        Component(Type(typeid(ConvertStringListToPacket)), Version(0, 0, 1))
    {
        declareInput<vector<string> >(
            "in", boost::bind(&ConvertStringListToPacket::inHandler, this, _1)
            );
        declareOutput<MRN::PacketPtr>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const vector<string>& in)
    {
        char** x = (char**)malloc(in.size() * sizeof(char*));

        for (int i = 0; i < in.size(); ++i)
        {
            x[i] = strdup(in[i]->c_str());
        }

        emitOutput<MRN::PacketPtr>(
            "out", MRN::PacketPtr(new MRN::Packet(0, 0, "%as", x, in.size()))
            );
    }
    
}; // class ConvertStringListToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertStringListToPacket)



/**
 * Component that converts a MRNet packet into a string.
 */
class __attribute__ ((visibility ("hidden"))) ConvertPacketToString :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertPacketToString())
            );
    }

private:

    /** Default constructor. */
    ConvertPacketToString() :
        Component(Type(typeid(ConvertPacketToString)), Version(0, 0, 1))
    {
        declareInput<MRN::PacketPtr>(
            "in", boost::bind(&ConvertPacketToString::inHandler, this, _1)
            );
        declareOutput<string>("out");
    }
    
    /** Handler for the "in" input.*/
    void inHandler(const MRN::PacketPtr& in)
    {
        char* x = NULL;
        in->unpack("%s", &x);
        emitOutput<string>("out", string(x));
    }

}; // class ConvertPacketToString

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToString)



/**
 * Component that converts a MRNet packet into a vector<string>.
 */
class __attribute__ ((visibility ("hidden"))) ConvertPacketToStringList :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertPacketToStringList())
            );
    }

private:

    /** Default constructor. */
    ConvertPacketToStringList() :
        Component(Type(typeid(ConvertPacketToStringList)), Version(0, 0, 1))
    {
        declareInput<MRN::PacketPtr>(
            "in", boost::bind(&ConvertPacketToStringList::inHandler, this, _1)
            );
        declareOutput<vector<string> >("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const MRN::PacketPtr& in)
    {
        char** x = NULL;
        unsigned int length = 0;
        in->unpack("%as", &x, &length);
        
        vector<string> value;
        for (unsigned int i = 0; i < length; ++i)
        {
            value.push_back(string(x[i]));
        }
        
        emitOutput<vector<string> >("out", value);
    }
    
}; // class ConvertPacketToStringList

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToStringList)
