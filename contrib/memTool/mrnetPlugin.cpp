#include <sys/param.h>

#include <boost/bind.hpp>
#include <mrnet/MRNet.h>
#include <typeinfo>
#include <string>

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
        Component(Type(typeid(ConvertStringToPacket)), Version(1, 0, 0))
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
        Component(Type(typeid(ConvertPacketToString)), Version(1, 0, 0))
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


