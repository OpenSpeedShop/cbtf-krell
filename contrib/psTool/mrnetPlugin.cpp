#include <sys/param.h>

#include <boost/bind.hpp>
#include <mrnet/MRNet.h>
#include <typeinfo>
#include <string>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

using namespace KrellInstitute::CBTF;
using namespace std;


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
            x[i] = strdup(in[i].c_str());
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
