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

// ======

/**
 * Component that converts a bool into a MRNet packet.
 */
class __attribute__ ((visibility ("hidden"))) ConvertBoolToPacket :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertBoolToPacket())
            );
    }

private:

    /** Default constructor. */
    ConvertBoolToPacket() :
        Component(Type(typeid(ConvertBoolToPacket)), Version(0, 0, 1))
    {
        declareInput<bool>(
            "in", boost::bind(&ConvertBoolToPacket::inHandler, this, _1)
            );
        declareOutput<MRN::PacketPtr>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const bool& in)
    {
        emitOutput<MRN::PacketPtr>(
            "out", MRN::PacketPtr(new MRN::Packet(0, 0, "%d", in))
            );
    }

}; // class ConvertBoolToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertBoolToPacket)


/**
 * Component that converts a MRNet packet into a bool.
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
        Component(Type(typeid(ConvertPacketToBool)), Version(0, 0, 1))
    {
        declareInput<MRN::PacketPtr>(
            "in", boost::bind(&ConvertPacketToBool::inHandler, this, _1)
            );
        declareOutput<bool>("out");
    }
    
    /** Handler for the "in" input.*/
    void inHandler(const MRN::PacketPtr& in)
    {
        bool* x = NULL;
        in->unpack("%d", &x);
        emitOutput<bool>("out", x);
    }

}; // class ConvertPacketToBool

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToBool)


/**
 * Component that converts an uint64_t value into a MRNet packet.
 */
class __attribute__ ((visibility ("hidden"))) ConvertUInt64ToPacket :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertUInt64ToPacket())
            );
    }

private:

    /** Default constructor. */
    ConvertUInt64ToPacket() :
        Component(Type(typeid(ConvertUInt64ToPacket)), Version(0, 0, 1))
    {
        declareInput<uint64_t>(
            "in", boost::bind(&ConvertUInt64ToPacket::inHandler, this, _1)
            );
        declareOutput<MRN::PacketPtr>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const uint64_t& in)
    {
        emitOutput<MRN::PacketPtr>(
            "out", MRN::PacketPtr(new MRN::Packet(0, 0, "%uld", in))
            );
    }
    
}; // class ConvertUInt64ToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertUInt64ToPacket)



/**
 * Component that converts a MRNet packet into an integer value.
 */
class __attribute__ ((visibility ("hidden"))) ConvertPacketToUInt64 :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertPacketToUInt64())
            );
    }

private:

    /** Default constructor. */
    ConvertPacketToUInt64() :
        Component(Type(typeid(ConvertPacketToUInt64)), Version(0, 0, 1))
    {
        declareInput<MRN::PacketPtr>(
            "in", boost::bind(&ConvertPacketToUInt64::inHandler, this, _1)
            );
        declareOutput<uint64_t>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const MRN::PacketPtr& in)
    {
        uint64_t out = 0;
        in->unpack("%uld", &out);
        emitOutput<uint64_t>("out", out);
    }
    
}; // class ConvertPacketToUInt64

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToUInt64)
