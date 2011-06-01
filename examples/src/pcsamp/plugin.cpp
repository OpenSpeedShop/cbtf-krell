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

#include <boost/bind.hpp>
#include <mrnet/MRNet.h>
#include <typeinfo>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>

using namespace KrellInstitute::CBTF;



/**
 * Component that converts an integer value into a MRNet packet.
 */
class __attribute__ ((visibility ("hidden"))) ConvertIntToPacket :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertIntToPacket())
            );
    }

private:

    /** Default constructor. */
    ConvertIntToPacket() :
        Component(Type(typeid(ConvertIntToPacket)), Version(0, 0, 1))
    {
        declareInput<int>(
            "in", boost::bind(&ConvertIntToPacket::inHandler, this, _1)
            );
        declareOutput<MRN::PacketPtr>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const int& in)
    {
        emitOutput<MRN::PacketPtr>(
            "out", MRN::PacketPtr(new MRN::Packet(0, 0, "%d", in))
            );
    }
    
}; // class ConvertIntToPacket

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertIntToPacket)



/**
 * Component that converts a MRNet packet into an integer value.
 */
class __attribute__ ((visibility ("hidden"))) ConvertPacketToInt :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new ConvertPacketToInt())
            );
    }

private:

    /** Default constructor. */
    ConvertPacketToInt() :
        Component(Type(typeid(ConvertPacketToInt)), Version(0, 0, 1))
    {
        declareInput<MRN::PacketPtr>(
            "in", boost::bind(&ConvertPacketToInt::inHandler, this, _1)
            );
        declareOutput<int>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const MRN::PacketPtr& in)
    {
        int out = 0;
        in->unpack("%d", &out);
        emitOutput<int>("out", out);
    }
    
}; // class ConvertPacketToInt

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToInt)



/**
 * Component that doubles an integer value.
 */
class __attribute__ ((visibility ("hidden"))) Doubler :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new Doubler())
            );
    }

private:

    /** Default constructor. */
    Doubler() :
        Component(Type(typeid(Doubler)), Version(0, 0, 1))
    {
        declareInput<int>(
            "in", boost::bind(&Doubler::inHandler, this, _1)
            );
        declareOutput<int>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const int& in)
    {
        emitOutput<int>("out", in * 2);
    }
    
}; // class Doubler

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(Doubler)



/**
 * Component that increments an integer value.
 */
class __attribute__ ((visibility ("hidden"))) Incrementer :
    public Component
{

public:

    /** Factory function for this component type. */
    static Component::Instance factoryFunction()
    {
        return Component::Instance(
            reinterpret_cast<Component*>(new Incrementer())
            );
    }

private:

    /** Default constructor. */
    Incrementer() :
        Component(Type(typeid(Incrementer)), Version(0, 0, 1))
    {
        declareInput<int>(
            "in", boost::bind(&Incrementer::inHandler, this, _1)
            );
        declareOutput<int>("out");
    }

    /** Handler for the "in" input.*/
    void inHandler(const int& in)
    {
        emitOutput<int>("out", in + 1);
    }
    
}; // class Incrementer

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(Incrementer)
