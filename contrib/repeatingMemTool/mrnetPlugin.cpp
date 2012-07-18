#include <sys/param.h>

#include <boost/bind.hpp>
#include <mrnet/MRNet.h>
#include <typeinfo>
#include <string>
#include <iostream>
#include "Memory.h"

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

using namespace KrellInstitute::CBTF;

/**
 * Component that converts an std::string value into a MRNet packet.
 */
class __attribute__ ((visibility ("hidden"))) ConvertStringVectToPacket :
public Component
{

    public:

        /** Factory function for this component type. */
        static Component::Instance factoryFunction()
        {
            return Component::Instance(
                    reinterpret_cast<Component*>(
                        new ConvertStringVectToPacket())
                    );
        }

    private:

        /** Default constructor. */
        ConvertStringVectToPacket() :
            Component(Type(typeid(ConvertStringVectToPacket)), Version(1, 0, 0))
    {
        declareInput<std::vector<std::string> >(
                "in2", 
                boost::bind(&ConvertStringVectToPacket::inHandler2, this, _1)
                );
        declareOutput<MRN::PacketPtr>("out2");
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
                    "out2", 
                    MRN::PacketPtr(
                        new MRN::Packet(0, 0, "%as", arr, in.size() ))
                    );
        }

}; // class ConvertStringVectToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertStringVectToPacket)

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
    }

        /** Handler for the "in" input.*/
        void inHandler1(const std::string& in)
        {
            emitOutput<MRN::PacketPtr>(
                    "out1", 
                    MRN::PacketPtr(new MRN::Packet(0, 0, "%s", in.c_str()))
                    );
        }
}; // class ConvertStringToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertStringToPacket)

    /**
     * Component that converts a MRNet packet into an string value.
     */
    class __attribute__ ((visibility ("hidden"))) ConvertPacketToStringVect :
    public Component
{

    public:

        /** Factory function for this component type. */
        static Component::Instance factoryFunction()
        {
            return Component::Instance(
                    reinterpret_cast<Component*>(new ConvertPacketToStringVect())
                    );
        }

    private:

        /** Default constructor. */
        ConvertPacketToStringVect() :
            Component(Type(typeid(ConvertPacketToStringVect)), Version(1, 0, 0))
    {
        declareInput<MRN::PacketPtr>(
                "in2", 
                boost::bind(&ConvertPacketToStringVect::inHandler2, this, _1)
                );
        declareOutput<std::vector<std::string> >("out2");
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
            emitOutput<std::vector<std::string> >("out2", out);
        }

}; // class ConvertPacketToStringVect

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToStringVect)


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
    }

        /** Handler for the "in" input.*/
        void inHandler1(const MRN::PacketPtr& in)
        {
            char* val = NULL;
            in->unpack("%s", &val);
            emitOutput<std::string>("out1", std::string(val));
        }
}; // class ConvertPacketToString

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToString)

    /**
     * Component that converts a bool to an MRNet packet.
     */
    class __attribute__ ((visibility ("hidden"))) ConvertBoolToPacket :
    public Component
{
    public:
        static Component::Instance factoryFunction()
        {
            return Component::Instance(
                    reinterpret_cast<Component*>(new ConvertBoolToPacket())
                    );
        }
    private:
        ConvertBoolToPacket() :
            Component(Type(typeid(ConvertBoolToPacket)), Version(1, 0, 0))
    {
        declareInput<bool>(
                "BoolIn", 
                boost::bind(&ConvertBoolToPacket::inBoolHandler, this, _1)
                );
        declareOutput<MRN::PacketPtr>("PacketOut");
    }

        void inBoolHandler(const bool & b) {
            int conversion;
            if (b) {
                conversion = 1;
            } else {
                conversion = 0;
            }
            emitOutput<MRN::PacketPtr>(
                    "PacketOut",
                    MRN::PacketPtr(new MRN::Packet(0, 0, "%d", conversion))
                    );
        }
};

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertBoolToPacket)

    /**
     * Component that converts a MRNet packet into an string value.
     */
    class __attribute__ ((visibility ("hidden"))) ConvertPacketToBool :
    public Component
{

    public:

        /** Factory function for this component type. */
        static Component::Instance factoryFunction()
        {
            return Component::Instance(
                    reinterpret_cast<Component*>(new ConvertPacketToBool())
                    );
        }

    private:

        /** Default constructor. */
        ConvertPacketToBool() :
            Component(Type(typeid(ConvertPacketToBool)), Version(1, 0, 0))
    {
        declareInput<MRN::PacketPtr>(
                "PacketIn", 
                boost::bind(&ConvertPacketToBool::inHandler1, this, _1)
                );
        declareOutput<bool>("BoolOut");
    }

        /** Handler for the "in" input.*/
        void inHandler1(const MRN::PacketPtr& in)
        {
            bool boolOut;
            int tempVal;
            in->unpack("%d", &tempVal);
            if(tempVal == 0) {
                boolOut = false;
            } else {
                boolOut = true;
            }
            emitOutput<bool>("BoolOut", boolOut);
        }
}; // class ConvertPacketToBool

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToBool)

    /**
     * Component that converts a MRNet packet into an string value.
     */
    class __attribute__ ((visibility ("hidden"))) ConvertNodeMemoryToPacket :
    public Component
{

    public:

        /** Factory function for this component type. */
        static Component::Instance factoryFunction()
        {
            return Component::Instance(
                    reinterpret_cast<Component*>(
                        new ConvertNodeMemoryToPacket())
                    );
        }

    private:

        /** Default constructor. */
        ConvertNodeMemoryToPacket() :
            Component(Type(typeid(ConvertNodeMemoryToPacket)), Version(1, 0, 0))
    {
        declareInput<NodeMemory>(
                "NodeMemoryIn", boost::bind(
                    &ConvertNodeMemoryToPacket::inHandler1, 
                    this, 
                    _1)
                );
        declareOutput<MRN::PacketPtr>("PacketOut");
    }

        /** Handler for the "in" input.*/
        void inHandler1(const NodeMemory& nm)
        {
            emitOutput<MRN::PacketPtr>(
                    "PacketOut",
                    MRN::PacketPtr(new MRN::Packet(0, 0, "%lf %lf %s", 
                            nm.getTotal(),
                            nm.getApplication(), 
                            nm.getId().c_str()))
                    );
        }
}; // class ConvertNodeMemoryToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertNodeMemoryToPacket)

    /**
     * Component that converts a MRNet packet into an string value.
     */
    class __attribute__ ((visibility ("hidden"))) ConvertPacketToNodeMemory :
    public Component
{

    public:

        /** Factory function for this component type. */
        static Component::Instance factoryFunction()
        {
            return Component::Instance(
                    reinterpret_cast<Component*>(
                        new ConvertPacketToNodeMemory())
                    );
        }

    private:

        /** Default constructor. */
        ConvertPacketToNodeMemory() :
            Component(Type(typeid(ConvertPacketToNodeMemory)), Version(1, 0, 0))
    {
        declareInput<MRN::PacketPtr>(
                "PacketIn", boost::bind(&ConvertPacketToNodeMemory::inHandler1, 
                    this, _1)
                );
        declareOutput<NodeMemory>("NodeMemoryOut");
    }

        /** Handler for the "in" input.*/
        void inHandler1(const MRN::PacketPtr& packetIn)
        {
            NodeMemory nmOut;
            double tmpTotal;
            double tmpAppl;
            std::string tmpId;
            packetIn->unpack("%lf %lf %s", &tmpTotal, &tmpAppl, &tmpId);
            nmOut.setTotal(tmpTotal);
            nmOut.setApplication(tmpAppl);
            nmOut.setId(tmpId);
            emitOutput<NodeMemory>("NodeMemoryOut", nmOut);
        }
}; // class ConvertPacketToNodeMemory

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToNodeMemory)

    /**
     * Component that converts a MRNet packet into an string value.
     */
    class __attribute__ ((visibility ("hidden"))) ConvertNodeMemoryVectToPacket:
    public Component
{

    public:

        /** Factory function for this component type. */
        static Component::Instance factoryFunction()
        {
            return Component::Instance(
                    reinterpret_cast<Component*>(
                        new ConvertNodeMemoryVectToPacket())
                    );
        }

    private:

        /** Default constructor. */
        ConvertNodeMemoryVectToPacket() :
            Component(Type(typeid(ConvertNodeMemoryVectToPacket)), 
                    Version(1, 0, 0))
    {
        declareInput< std::vector<NodeMemory> >(
                "NodeMemoryVectIn", boost::bind(
                    &ConvertNodeMemoryVectToPacket::inHandler1, this, _1)
                );
        declareOutput<MRN::PacketPtr>("PacketOut");
    }

        /** Handler for the "in" input.*/
        void inHandler1(const std::vector<NodeMemory>& nm)
        {
            int arrySize = nm.size();
            double * tmpTotalArry 
                = (double *) calloc(arrySize, sizeof(double));
            double * tmpApplicationArry 
                = (double *) calloc(arrySize, sizeof(double));
            char ** tmpIdArry = (char **) calloc(arrySize, sizeof(char *));

            for (int i = 0; i < arrySize; i++) {
                tmpTotalArry[i] = nm[i].getTotal();
                tmpApplicationArry[i] = nm[i].getApplication();
                std::string id = nm[i].getId();
                tmpIdArry[i] = (char *) calloc(id.size() + 1, sizeof(char));
                strncpy(tmpIdArry[i], id.c_str(), id.size() + 1);
            }

            emitOutput<MRN::PacketPtr>(
                    "PacketOut",
                    MRN::PacketPtr(new MRN::Packet(0, 0, "%alf %alf %as", 
                            tmpTotalArry, arrySize,
                            tmpApplicationArry, arrySize,
                            tmpIdArry, arrySize))
                    );
        }
}; // class ConvertNodeMemoryVectToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertNodeMemoryVectToPacket)

    /**
     * Component that converts a MRNet packet into an string value.
     */
    class __attribute__ ((visibility ("hidden"))) ConvertPacketToNodeMemoryVect:
    public Component
{

    public:

        /** Factory function for this component type. */
        static Component::Instance factoryFunction()
        {
            return Component::Instance(
                    reinterpret_cast<Component*>(
                        new ConvertPacketToNodeMemoryVect())
                    );
        }

    private:

        /** Default constructor. */
        ConvertPacketToNodeMemoryVect() :
            Component(Type(typeid(ConvertPacketToNodeMemoryVect)), 
                    Version(1, 0, 0))
    {
        declareInput<MRN::PacketPtr>(
                "PacketIn", boost::bind(
                    &ConvertPacketToNodeMemoryVect::inHandler1, 
                    this,
                    _1)
                );
        declareOutput< std::vector<NodeMemory> >("NodeMemoryOut");
    }

        /** Handler for the "in" input.*/
        void inHandler1(const MRN::PacketPtr& packetIn)
        {
            std::vector<NodeMemory> nmOut;
            int arrySize = 0;
            double * tmpTotalArry = NULL;
            double * tmpApplArry = NULL;
            char ** tmpIdArry = NULL;

            packetIn->unpack("%alf %alf %as", 
                    &tmpTotalArry, &arrySize,
                    &tmpApplArry, &arrySize,
                    &tmpIdArry, &arrySize);
            
            for (int i = 0; i < arrySize; i++) {
                NodeMemory tmp;
                tmp.setTotal(tmpTotalArry[i]);
                tmp.setApplication(tmpApplArry[i]);
                tmp.setId(tmpIdArry[i]);
                nmOut.push_back(tmp);
            }
            
            emitOutput< std::vector<NodeMemory> >("NodeMemoryOut", nmOut);
        }
}; // class ConvertPacketToNodeMemoryVect

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToNodeMemoryVect)
