////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010,2011 Krell Institute. All Rights Reserved.
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

/** @file Plugin used by unit tests for the CBTF MRNet library. */

#include <sys/param.h>

#include <boost/bind.hpp>
#include <mrnet/MRNet.h>
#include <typeinfo>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

using namespace KrellInstitute::CBTF;



/**
 * Component that converts an std::string value into a MRNet packet.
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
        declareInput<std::string>(
            "in1", boost::bind(&ConvertStringToPacket::inHandler1, this, _1)
            );
        declareOutput<MRN::PacketPtr>("out1");

        declareInput<std::vector<std::string> >(
            "in2", boost::bind(&ConvertStringToPacket::inHandler2, this, _1)
            );
        declareOutput<MRN::PacketPtr>("out2");
    }

    /** Handler for the "in" input.*/
    void inHandler1(const std::string& in)
    {
        emitOutput<MRN::PacketPtr>(
            "out1", MRN::PacketPtr(new MRN::Packet(0, 0, "%s", in.c_str()))
            );
    }
    void inHandler2(const std::vector<std::string>& in)
    {
	char** arr = NULL;
	arr = (char**) malloc( in.size() * sizeof(char*) );

	std::vector< std::string >::const_iterator iter = in.begin();
	for( unsigned u = 0; iter != in.end() ; iter++, u++ ) {

	    arr[u] = strdup(iter->c_str());
	}
        emitOutput<MRN::PacketPtr>(
            "out2", MRN::PacketPtr(new MRN::Packet(0, 0, "%as", arr, in.size() ))
            );
    }
    
}; // class ConvertStringToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertStringToPacket)



/**
 * Component that converts a MRNet packet into an string value.
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
            "in1", boost::bind(&ConvertPacketToString::inHandler1, this, _1)
            );
        declareOutput<std::string>("out1");

        declareInput<MRN::PacketPtr>(
            "in2", boost::bind(&ConvertPacketToString::inHandler2, this, _1)
            );
        declareOutput<std::vector<std::string> >("out2");
    }

    /** Handler for the "in" input.*/
    void inHandler1(const MRN::PacketPtr& in)
    {
	char* val = NULL;
        in->unpack("%s", &val);
        emitOutput<std::string>("out1", std::string(val));
    }
    void inHandler2(const MRN::PacketPtr& in)
    {
        std::vector<std::string> out;
	char** val = NULL;
	unsigned len = 0;
        in->unpack("%as", &val, &len);
	for( unsigned u=0; u < len; u++ ) {
	    out.push_back(std::string(val[u]));
	}
        emitOutput<std::vector<std::string> >("out2", out);
    }
    
}; // class ConvertPacketToString

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToString)



/**
 * Component that calls the ps command.
 */
class __attribute__ ((visibility ("hidden"))) PopenCommand :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new PopenCommand())
            );
    }

private:

    /** Default constructor. */
    PopenCommand() :
        Component(Type(typeid(PopenCommand)), Version(0, 0, 1))
    {
        declareInput<std::string>(
            "in", boost::bind(&PopenCommand::inHandler, this, _1)
            );
        declareOutput< std::vector<std::string> >("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const std::string& in)
    {
	std::string command;

	if (in == "" ) {
	    command = "ps -ef";
	} else {
	    command = in;
	}

	FILE *fd = NULL;
	char buffer[ BUFSIZ ];
	memset(&buffer,0,sizeof(buffer));

	std::vector<std::string> psout;
	std::string line;

	fd = popen( in.c_str(), "r" );

	char hostname[MAXHOSTNAMELEN];
	gethostname(hostname, MAXHOSTNAMELEN);
	std::string header("Output of " + in + " from host " +hostname);
	psout.push_back(header);

	if( fd != NULL ) {
            while( fgets( buffer, sizeof( buffer ), fd ) ) {
                std::string fline(buffer);
		if (!fline.empty() && fline[fline.length()-1] == '\n') {
		    fline.erase(fline.length()-1);
		}
		if (fline != "") {
		    psout.push_back(fline);
		}
		line = fline;
            }
	    pclose( fd );

            fd = NULL;

        } else {
            std::cerr << "Error opening " << in << std::endl;
        }

//DEBUG
#if 0
	std::vector<std::string>::const_iterator ci;
	for(ci=psout.begin(); ci!=psout.end(); ci++) {
	    std::cout << *ci << std::endl;
	}
#endif

        emitOutput< std::vector<std::string> >("out", psout);
    }
    
}; // class PopenCommand

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(PopenCommand)


/**
 */
class __attribute__ ((visibility ("hidden"))) DisplayData :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new DisplayData())
            );
    }

private:

    /** Default constructor. */
    DisplayData() :
        Component(Type(typeid(DisplayData)), Version(0, 0, 1))
    {
        declareInput<std::string>(
            "in1", boost::bind(&DisplayData::displayHandler1, this, _1)
            );
        declareOutput<std::string>("out1");

        declareInput<std::vector<std::string> >(
            "in2", boost::bind(&DisplayData::displayHandler2, this, _1)
            );
        declareOutput<std::vector<std::string> >("out2");
    }

    /** Handler for the "in" input.*/
    void displayHandler1(const std::string& in)
    {
	std::cout << in << std::endl;
    }

    void displayHandler2(const std::vector<std::string>& in)
    {
	std::cout << "TODO for displayHandler2..." << std::endl;
    }

}; // class DisplayData

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(DisplayData)
