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
            "in", boost::bind(&ConvertStringToPacket::inHandler1, this, _1)
            );
        declareOutput<MRN::PacketPtr>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler1(const std::string& in)
    {
        emitOutput<MRN::PacketPtr>(
            "out", MRN::PacketPtr(new MRN::Packet(0, 0, "%s", in.c_str()))
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
            "in", boost::bind(&ConvertPacketToString::inHandler1, this, _1)
            );
        declareOutput<std::string>("out");

    }

    /** Handler for the "in" input.*/
    void inHandler1(const MRN::PacketPtr& in)
    {
	char* val = NULL;
        in->unpack("%s", &val);
        emitOutput<std::string>("out", std::string(val));
    }
    
}; // class ConvertPacketToString

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToString)

/**
 * Component that converts an std::vector<std::string> value into a MRNet packet.
 */
class __attribute__ ((visibility ("hidden"))) ConvertStringToPacketList :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertStringToPacketList())
            );
    }

private:

    /** Default constructor. */
    ConvertStringToPacketList() :
        Component(Type(typeid(ConvertStringToPacketList)), Version(1, 0, 0))
    {
        declareInput<std::vector<std::string> >(
            "in", boost::bind(&ConvertStringToPacketList::inHandler2, this, _1)
            );
        declareOutput<MRN::PacketPtr>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler2(const std::vector<std::string>& in)
    {
	char** arr = NULL;
	arr = (char**) malloc( in.size() * sizeof(char*) );

	std::vector< std::string >::const_iterator iter = in.begin();
	for( unsigned u = 0; iter != in.end() ; iter++, u++ ) {

	    arr[u] = strdup(iter->c_str());
	}
        emitOutput<MRN::PacketPtr>(
            "out", MRN::PacketPtr(new MRN::Packet(0, 0, "%as", arr, in.size() ))
            );
    }
    
}; // class ConvertStringToPacketList

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertStringToPacketList)

/**
 * Component that converts a MRNet packet into an vector<string> value.
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
        Component(Type(typeid(ConvertPacketToStringList)), Version(1, 0, 0))
    {
        declareInput<MRN::PacketPtr>(
            "in", boost::bind(&ConvertPacketToStringList::inHandler2, this, _1)
            );
        declareOutput<std::vector<std::string> >("out");
    }

    /** Handler for the "in" input.*/
    void inHandler2(const MRN::PacketPtr& in)
    {
        std::vector<std::string> out;
	char** val = NULL;
	unsigned len = 0;
        in->unpack("%as", &val, &len);
	for( unsigned u=0; u < len; u++ ) {
	    out.push_back(std::string(val[u]));
	}
        emitOutput<std::vector<std::string> >("out", out);
    }
    
}; // class ConvertPacketToStringList

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToStringList)


/**
 * Component that converts a MRNet packet into an string value.
 */
class __attribute__ ((visibility ("hidden"))) PassThrough :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new PassThrough())
            );
    }

private:

    /** Default constructor. */
    PassThrough() :
        Component(Type(typeid(PassThrough)), Version(1, 0, 0))
    {
        declareInput<std::string>(
            "in1", boost::bind(&PassThrough::inHandler1, this, _1)
            );
        declareOutput<std::string>("out1");

        declareInput<std::vector<std::string> >(
            "in2", boost::bind(&PassThrough::inHandler2, this, _1)
            );
        declareOutput<std::vector<std::string> >("out2");
    }

    /** Handler for the "in" input.*/
    void inHandler1(const std::string& in)
    {
        std::string out = in;
        emitOutput<std::string>("out1", out);
    }
    void inHandler2(const std::vector<std::string>& in)
    {
        std::vector<std::string> out = in;
        emitOutput<std::vector<std::string> >("out2", out);
    }
    
}; // class PassThrough

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(PassThrough)


