////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file Unit tests for the CBTF symbol table library. */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE CBTF-SymbolTable

#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/ref.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <KrellInstitute/SymbolTable/Address.hpp>
#include <KrellInstitute/SymbolTable/AddressRange.hpp>
#include <KrellInstitute/SymbolTable/AddressSpace.hpp>
#include <KrellInstitute/SymbolTable/FileName.hpp>
#include <KrellInstitute/SymbolTable/Function.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>
#include <KrellInstitute/SymbolTable/Statement.hpp>
#include <KrellInstitute/SymbolTable/Time.hpp>
#include <KrellInstitute/SymbolTable/TimeInterval.hpp>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "AddressBitmap.hpp"

using namespace KrellInstitute::SymbolTable;
using namespace KrellInstitute::SymbolTable::Impl;



/** Anonymous namespace hiding implementation details. */
namespace {

    /** Templated visitor for accumulating functions, statements, etc. */
    template <typename T>
    bool accumulate(const T& x, std::set<T>& set)
    {
        set.insert(x);
        return true;
    }

    /** Templated visitor for accumulating mappings. */
    bool accumulateMappings(
        const LinkedObject& x,
        const AddressRange& range,
        const TimeInterval& interval,
        std::map<LinkedObject, std::vector<boost::uint64_t> >& set
        )
    {
        std::vector<boost::uint64_t> where = boost::assign::list_of
            (static_cast<boost::uint64_t>(range.begin()))
            (static_cast<boost::uint64_t>(range.end()))
            (static_cast<boost::uint64_t>(interval.begin()))
            (static_cast<boost::uint64_t>(interval.end()));

        set.insert(std::make_pair(x, where));
        return true;
    }
    
} // namespace <anonymous>



/**
 * Unit test for the Address class.
 */
BOOST_AUTO_TEST_CASE(TestAddress)
{
    BOOST_CHECK_EQUAL(Address(), Address::TheLowest());
    BOOST_CHECK_NE(Address::TheLowest(), Address::TheHighest());
    BOOST_CHECK_LT(Address::TheLowest(), Address::TheHighest());
    BOOST_CHECK_GT(Address::TheHighest(), Address::TheLowest());

    BOOST_CHECK_EQUAL(Address(static_cast<CBTF_Protocol_Address>(Address(27))),
                      Address(27));

    BOOST_CHECK_EQUAL(--Address::TheLowest(), Address::TheHighest());
    BOOST_CHECK_EQUAL(Address::TheLowest() - 1, Address::TheHighest());

    BOOST_CHECK_EQUAL(Address::TheLowest(), ++Address::TheHighest());
    BOOST_CHECK_EQUAL(Address::TheLowest(), Address::TheHighest() + 1);

    BOOST_CHECK_EQUAL(Address(27), Address(4) + Address(23));
    BOOST_CHECK_EQUAL(Address(27), Address(2) + 25);

    BOOST_CHECK_EQUAL(Address(27), Address(30) - Address(3));
    BOOST_CHECK_EQUAL(Address(27), Address(29) - 2);

    BOOST_CHECK_EQUAL(static_cast<std::string>(Address(13*27)),
                      "0x000000000000015F");
}



/**
 * Unit test for the AddressBitmap class.
 */
BOOST_AUTO_TEST_CASE(TestAddressBitmap)
{
    AddressBitmap bitmap(AddressRange(0, 13));

    BOOST_CHECK_EQUAL(bitmap.range(), AddressRange(0, 13));
    
    BOOST_CHECK(!bitmap.get(0));
    BOOST_CHECK(!bitmap.get(7));
    BOOST_CHECK(!bitmap.get(13));
    BOOST_CHECK_THROW(bitmap.get(27), std::invalid_argument);
    bitmap.set(7, true);
    BOOST_CHECK(!bitmap.get(0));
    BOOST_CHECK(bitmap.get(7));
    BOOST_CHECK(!bitmap.get(13));
    BOOST_CHECK_THROW(bitmap.set(27, true), std::invalid_argument);
    bitmap.set(7, false);
    BOOST_CHECK(!bitmap.get(0));
    BOOST_CHECK(!bitmap.get(7));
    BOOST_CHECK(!bitmap.get(13));

    bitmap.set(7, true);
    std::set<AddressRange> ranges = bitmap.ranges(false);
    BOOST_CHECK_EQUAL(ranges.size(), 2);
    BOOST_CHECK_EQUAL(*(ranges.begin()), AddressRange(0, 6));
    BOOST_CHECK_EQUAL(*(++ranges.begin()), AddressRange(8, 13));
    ranges = bitmap.ranges(true);
    BOOST_CHECK_EQUAL(ranges.size(), 1);
    BOOST_CHECK_EQUAL(*(ranges.begin()), AddressRange(7, 7));
    bitmap.set(12, true);
    bitmap.set(13, true);
    ranges = bitmap.ranges(true);
    BOOST_CHECK_EQUAL(ranges.size(), 2);
    BOOST_CHECK_EQUAL(*(ranges.begin()), AddressRange(7, 7));
    BOOST_CHECK_EQUAL(*(++ranges.begin()), AddressRange(12, 13));

    std::set<Address> addresses = boost::assign::list_of(0)(7)(13)(27);
    bitmap = AddressBitmap(addresses);
    BOOST_CHECK_EQUAL(bitmap.range(), AddressRange(0, 27));
    BOOST_CHECK(bitmap.get(0));
    BOOST_CHECK(bitmap.get(7));
    BOOST_CHECK(bitmap.get(13));
    BOOST_CHECK(bitmap.get(27));
    BOOST_CHECK(!bitmap.get(1));

    BOOST_CHECK_EQUAL(
        static_cast<std::string>(bitmap),
        "[0x0000000000000000, 0x000000000000001B]: 1000000100000100000000000001"
        );

    BOOST_CHECK_EQUAL(
        AddressBitmap(static_cast<CBTF_Protocol_AddressBitmap>(bitmap)),
        bitmap
        );
}



/**
 * Unit test for the AddressRange class.
 */
BOOST_AUTO_TEST_CASE(TestAddressRange)
{
    BOOST_CHECK(AddressRange().empty());
    BOOST_CHECK(!AddressRange(0).empty());
    BOOST_CHECK_EQUAL(AddressRange(27).begin(), AddressRange(27).end());
    BOOST_CHECK(!AddressRange(0, 1).empty());
    BOOST_CHECK(AddressRange(1, 0).empty());

    BOOST_CHECK_EQUAL(
        AddressRange(
            static_cast<CBTF_Protocol_AddressRange>(AddressRange(0, 27))
            ),
        AddressRange(0, 27)
        );

    BOOST_CHECK_LT(AddressRange(0, 13), AddressRange(1, 13));
    BOOST_CHECK_LT(AddressRange(0, 13), AddressRange(0, 27));
    BOOST_CHECK_GT(AddressRange(1, 13), AddressRange(0, 13));
    BOOST_CHECK_GT(AddressRange(0, 27), AddressRange(0, 13));
    BOOST_CHECK_EQUAL(AddressRange(0, 13), AddressRange(0, 13));
    BOOST_CHECK_NE(AddressRange(0, 13), AddressRange(0, 27));

    BOOST_CHECK_EQUAL(AddressRange(0, 13) | AddressRange(7, 27),
                      AddressRange(0, 27));
    BOOST_CHECK_EQUAL(AddressRange(7, 27) | AddressRange(0, 13),
                      AddressRange(0, 27));
    BOOST_CHECK_EQUAL(AddressRange(0, 7) | AddressRange(13, 27),
                      AddressRange(0, 27));
    BOOST_CHECK_EQUAL(AddressRange(13, 27) | AddressRange(0, 7),
                      AddressRange(0, 27));
    BOOST_CHECK_EQUAL(AddressRange(0, 7) | AddressRange(),
                      AddressRange(0, 7));
    BOOST_CHECK_EQUAL(AddressRange() | AddressRange(13, 27),
                      AddressRange(13, 27));

    BOOST_CHECK_EQUAL(AddressRange(0, 13) & AddressRange(7, 27),
                      AddressRange(7, 13));
    BOOST_CHECK_EQUAL(AddressRange(7, 27) & AddressRange(0, 13),
                      AddressRange(7, 13));
    BOOST_CHECK((AddressRange(0, 7) & AddressRange(13, 27)).empty());
    BOOST_CHECK((AddressRange(13, 27) & AddressRange(0, 7)).empty());
    BOOST_CHECK((AddressRange(0, 13) & AddressRange()).empty());
    BOOST_CHECK((AddressRange() & AddressRange(0, 13)).empty());

    BOOST_CHECK_EQUAL(AddressRange(0, 13).width(), Address(14));
    BOOST_CHECK_EQUAL(AddressRange(13, 0).width(), Address(0));

    BOOST_CHECK(AddressRange(0, 13).contains(7));
    BOOST_CHECK(!AddressRange(0, 13).contains(27));
    BOOST_CHECK(!AddressRange(13, 0).contains(7));
    BOOST_CHECK(!AddressRange(13, 0).contains(27));

    BOOST_CHECK(AddressRange(0, 27).contains(AddressRange(7, 13)));
    BOOST_CHECK(!AddressRange(0, 13).contains(AddressRange(7, 27)));
    BOOST_CHECK(!AddressRange(27, 0).contains(AddressRange(7, 13)));
    BOOST_CHECK(!AddressRange(13, 0).contains(AddressRange(7, 27)));
    BOOST_CHECK(!AddressRange(0, 27).contains(AddressRange(13, 7)));
    BOOST_CHECK(!AddressRange(0, 13).contains(AddressRange(27, 7)));    
    BOOST_CHECK(!AddressRange(27, 0).contains(AddressRange(13, 7)));
    BOOST_CHECK(!AddressRange(13, 0).contains(AddressRange(27, 7)));

    BOOST_CHECK(AddressRange(0, 27).intersects(AddressRange(7, 13)));
    BOOST_CHECK(AddressRange(0, 13).intersects(AddressRange(7, 27)));
    BOOST_CHECK(!AddressRange(27, 0).intersects(AddressRange(7, 13)));
    BOOST_CHECK(!AddressRange(13, 0).intersects(AddressRange(7, 27)));
    BOOST_CHECK(!AddressRange(0, 27).intersects(AddressRange(13, 7)));
    BOOST_CHECK(!AddressRange(0, 13).intersects(AddressRange(27, 7)));    
    BOOST_CHECK(!AddressRange(27, 0).intersects(AddressRange(13, 7)));
    BOOST_CHECK(!AddressRange(13, 0).intersects(AddressRange(27, 7)));
    
    BOOST_CHECK_EQUAL(static_cast<std::string>(AddressRange(13, 27)),
                      "[0x000000000000000D, 0x000000000000001B]");

    BOOST_CHECK(!std::less<AddressRange>()(
                    AddressRange(0, 13), AddressRange(7, 27)
                    ));
    BOOST_CHECK(!std::less<AddressRange>()(
                    AddressRange(7, 27), AddressRange(0, 13)
                    ));
    BOOST_CHECK(!std::less<AddressRange>()(
                    AddressRange(7, 13), AddressRange(7, 27)
                    ));
    BOOST_CHECK(!std::less<AddressRange>()(
                    AddressRange(7, 27), AddressRange(7, 13)
                    ));
    BOOST_CHECK(!std::less<AddressRange>()(
                    AddressRange(0, 7), AddressRange(7, 27)
                    ));
    BOOST_CHECK(!std::less<AddressRange>()(
                    AddressRange(7, 27), AddressRange(0, 7)
                    ));
    BOOST_CHECK(std::less<AddressRange>()(
                    AddressRange(0, 7), AddressRange(13, 27)
                    ));
    BOOST_CHECK(!std::less<AddressRange>()(
                    AddressRange(13, 27), AddressRange(0, 7)
                    ));
}



/**
 * Unit test for the AddressSpace class.
 */
BOOST_AUTO_TEST_CASE(TestAddressSpace)
{
    LinkedObject linked_object1(FileName("/path/to/nonexistent/executable"));
    LinkedObject linked_object2(FileName("/path/to/nonexistent/dso/1"));
    LinkedObject linked_object3(FileName("/path/to/nonexistent/dso/2"));
    LinkedObject linked_object4(FileName("/path/to/nonexistent/dso/3"));

    AddressSpace address_space;

    std::set<LinkedObject> linked_objects;
    address_space.visitLinkedObjects(
        boost::bind(accumulate<LinkedObject>, _1, boost::ref(linked_objects))
        );
    BOOST_CHECK(linked_objects.empty());

    std::map<LinkedObject, std::vector<boost::uint64_t> > mappings;
    address_space.visitMappings(
        boost::bind(accumulateMappings, _1, _2, _3, boost::ref(mappings))
        );
    BOOST_CHECK(mappings.empty());

    //
    // The following linked objects (along with their corresponding address
    // ranges and time intervals) are added to this address space for the test:
    //
    //                      AddressRange  TimeInterval
    //     linked_object1:    [  0,   7]    [ TB,  TE]
    //     linked_object2:    [ 13,  27]    [ TB,   7]
    //     linked_object3:    [ 13, 113]    [ 13,  27]
    //     linked_object4:    [213, 227]    [ 13,  TE]
    //
    // where "TB" is the earliest possible time, and "TE" is the latest possible
    // time.

    //
    // Test loading linked objects and the AddressSpace::visitLinkedObjects
    // and visitMappings queries.
    //
    
    address_space.loadLinkedObject(
        linked_object1, true, AddressRange(0, 7)
        );
    address_space.loadLinkedObject(
        linked_object2, false, AddressRange(13, 27)
        );
    address_space.loadLinkedObject(
        linked_object3, false, AddressRange(13, 113), Time(13)
        );

    linked_objects.clear();
    address_space.visitLinkedObjects(
        boost::bind(accumulate<LinkedObject>, _1, boost::ref(linked_objects))
        );
    BOOST_CHECK_EQUAL(linked_objects.size(), 3);
    BOOST_CHECK(linked_objects.find(linked_object1) != linked_objects.end());
    BOOST_CHECK(linked_objects.find(linked_object2) != linked_objects.end());
    BOOST_CHECK(linked_objects.find(linked_object3) != linked_objects.end());
    BOOST_CHECK(linked_objects.find(linked_object4) == linked_objects.end());
    
    mappings.clear();
    address_space.visitMappings(
        boost::bind(accumulateMappings, _1, _2, _3, boost::ref(mappings))
        );
    BOOST_CHECK_EQUAL(mappings.size(), 3);
    BOOST_CHECK(mappings.find(linked_object1) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object2) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object3) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object4) == mappings.end());

    std::vector<boost::uint64_t> where = mappings.find(linked_object3)->second;
    BOOST_CHECK_EQUAL(where[0], 13);
    BOOST_CHECK_EQUAL(where[1], 113);
    BOOST_CHECK_EQUAL(where[2], 13);
    BOOST_CHECK_EQUAL(where[3], Time::TheEnd());

    mappings.clear();
    address_space.visitMappings(
        AddressRange(14, 27),
        TimeInterval(Time::TheBeginning(), Time::TheEnd()),
        boost::bind(accumulateMappings, _1, _2, _3, boost::ref(mappings))
        );
    BOOST_CHECK_EQUAL(mappings.size(), 2);
    BOOST_CHECK(mappings.find(linked_object1) == mappings.end());
    BOOST_CHECK(mappings.find(linked_object2) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object3) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object4) == mappings.end());

    //
    // Test unloading linked objects and the AddressSpace::visitLinkedObjects
    // and visitMappings queries.
    //

    address_space.unloadLinkedObject(linked_object1);
    address_space.unloadLinkedObject(linked_object2, Time(7));
    address_space.unloadLinkedObject(linked_object3, Time(27));

    linked_objects.clear();
    address_space.visitLinkedObjects(
        boost::bind(accumulate<LinkedObject>, _1, boost::ref(linked_objects))
        );
    BOOST_CHECK_EQUAL(linked_objects.size(), 3);
    BOOST_CHECK(linked_objects.find(linked_object1) != linked_objects.end());
    BOOST_CHECK(linked_objects.find(linked_object2) != linked_objects.end());
    BOOST_CHECK(linked_objects.find(linked_object3) != linked_objects.end());
    BOOST_CHECK(linked_objects.find(linked_object4) == linked_objects.end());

    mappings.clear();
    address_space.visitMappings(
        boost::bind(accumulateMappings, _1, _2, _3, boost::ref(mappings))
        );
    BOOST_CHECK_EQUAL(mappings.size(), 3);
    BOOST_CHECK(mappings.find(linked_object1) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object2) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object3) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object4) == mappings.end());

    where = mappings.find(linked_object3)->second;
    BOOST_CHECK_EQUAL(where[0], 13);
    BOOST_CHECK_EQUAL(where[1], 113);
    BOOST_CHECK_EQUAL(where[2], 13);
    BOOST_CHECK_EQUAL(where[3], 27);

    mappings.clear();
    address_space.visitMappings(
        AddressRange(14, 27),
        TimeInterval(Time::TheBeginning(), 7),
        boost::bind(accumulateMappings, _1, _2, _3, boost::ref(mappings))
        );
    BOOST_CHECK_EQUAL(mappings.size(), 1);
    BOOST_CHECK(mappings.find(linked_object1) == mappings.end());
    BOOST_CHECK(mappings.find(linked_object2) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object3) == mappings.end());
    BOOST_CHECK(mappings.find(linked_object4) == mappings.end());

    //
    // Test the application of CBTF_Protocol_[Unl|L]oadedLinkedObject messages.
    //

    CBTF_Protocol_LoadedLinkedObject loaded_message;
    loaded_message.time = 13;
    loaded_message.range.begin = 213;
    loaded_message.range.end = 227 + 1;
    loaded_message.linked_object = linked_object4.getFile();
    
    CBTF_Protocol_UnloadedLinkedObject unloaded_message;
    unloaded_message.time = Time::TheEnd();
    unloaded_message.linked_object = linked_object4.getFile();
    
    address_space.applyMessage(loaded_message);
    address_space.applyMessage(unloaded_message);

    linked_objects.clear();
    address_space.visitLinkedObjects(
        boost::bind(accumulate<LinkedObject>, _1, boost::ref(linked_objects))
        );
    BOOST_CHECK_EQUAL(linked_objects.size(), 4);
    BOOST_CHECK(linked_objects.find(linked_object1) != linked_objects.end());
    BOOST_CHECK(linked_objects.find(linked_object2) != linked_objects.end());
    BOOST_CHECK(linked_objects.find(linked_object3) != linked_objects.end());
    BOOST_CHECK(linked_objects.find(linked_object4) != linked_objects.end());

    mappings.clear();
    address_space.visitMappings(
        boost::bind(accumulateMappings, _1, _2, _3, boost::ref(mappings))
        );
    BOOST_CHECK_EQUAL(mappings.size(), 4);
    BOOST_CHECK(mappings.find(linked_object1) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object2) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object3) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object4) != mappings.end());

    where = mappings.find(linked_object4)->second;
    BOOST_CHECK_EQUAL(where[0], 213);
    BOOST_CHECK_EQUAL(where[1], 227);
    BOOST_CHECK_EQUAL(where[2], 13);
    BOOST_CHECK_EQUAL(where[3], Time::TheEnd());

    mappings.clear();
    address_space.visitMappings(
        AddressRange(Address::TheLowest(), Address::TheHighest()),
        TimeInterval(13, 27),
        boost::bind(accumulateMappings, _1, _2, _3, boost::ref(mappings))
        );
    BOOST_CHECK_EQUAL(mappings.size(), 2);
    BOOST_CHECK(mappings.find(linked_object1) == mappings.end());
    BOOST_CHECK(mappings.find(linked_object2) == mappings.end());
    BOOST_CHECK(mappings.find(linked_object3) != mappings.end());
    BOOST_CHECK(mappings.find(linked_object4) != mappings.end());

    //
    // Test the conversion of AddressSpace to/from 
    // CBTF_Protocol_LinkedObjectGroup.
    //

    BOOST_CHECK(equivalent(
        AddressSpace(
            static_cast<CBTF_Protocol_LinkedObjectGroup>(address_space)
            ),
        address_space
        ));
}



/**
 * Unit test for the FileName class.
 */
BOOST_AUTO_TEST_CASE(TestFileName)
{    
    FileName name1("/path/to/nonexistent/file");
    
    BOOST_CHECK_EQUAL(name1.path(), "/path/to/nonexistent/file");
    BOOST_CHECK_EQUAL(name1.checksum(), 0);

    BOOST_CHECK_EQUAL(static_cast<std::string>(name1),
                      "0x0000000000000000: \"/path/to/nonexistent/file\"");
    
#if defined(BOOST_FILESYSTEM_VERSION) && (BOOST_FILESYSTEM_VERSION == 3)
    boost::filesystem::path tmp_path = boost::filesystem::unique_path();    
#else
    char model[] = "test.XXXXXX";
    BOOST_REQUIRE_EQUAL(mkstemp(model), 0);
    boost::filesystem::path tmp_path(model);
#endif

    boost::filesystem::ofstream stream(tmp_path);
    stream << "Four score and seven years ago our fathers brought forth "
           << "on this continent a new nation, conceived in liberty, and "
           << "dedicated to the proposition that all men are created equal."
           << std::endl << std::endl
           << "Now we are engaged in a great civil war, testing whether "
           << "that nation, or any nation so conceived and so dedicated, "
           << "can long endure. We are met on a great battlefield of that "
           << "war. We have come to dedicate a portion of that field, as "
           << "a final resting place for those who here gave their lives "
           << "that that nation might live. It is altogether fitting and "
           << "proper that we should do this."
           << std::endl << std::endl
           << "But, in a larger sense, we can not dedicate, we can not "
           << "consecrate, we can not hallow this ground. The brave men, "
           << "living and dead, who struggled here, have consecrated it, "
           << "far above our poor power to add or detract. The world will "
           << "little note, nor long remember what we say here, but it can "
           << "never forget what they did here. It is for us the living, "
           << "rather, to be dedicated here to the unfinished work which "
           << "they who fought here have thus far so nobly advanced. It is "
           << "rather for us to be here dedicated to the great task remaining "
           << "before us - that from these honored dead we take increased "
           << "devotion to that cause for which they gave the last full "
           << "measure of devotion - that we here highly resolve that these "
           << "dead shall not have died in vain - that this nation, under "
           << "God, shall have a new birth of freedom - and that government "
           << "of the people, by the people, for the people, shall not perish "
           << "from the earth."
           << std::endl;
    stream.close();
    FileName name2 = FileName(tmp_path);

    BOOST_CHECK_EQUAL(name2.path(), tmp_path);
    BOOST_CHECK_EQUAL(name2.checksum(), 17734875587178274082llu);

    stream.open(tmp_path, std::ios::app);
    stream << std::endl
           << "-- President Abraham Lincoln, November 19, 1863"
           << std::endl;
    stream.close();
    FileName name3 = FileName(tmp_path);

    BOOST_CHECK_EQUAL(name3.path(), tmp_path);
    BOOST_CHECK_EQUAL(name3.checksum(), 1506913182069408458llu);
    
    boost::filesystem::remove(tmp_path);

    BOOST_CHECK_NE(name1, name2);
    BOOST_CHECK_NE(name1, name3);
    BOOST_CHECK_NE(name2, name3);

    BOOST_CHECK_LT(name3, name2);
    BOOST_CHECK_GT(name2, name3);

    BOOST_CHECK_EQUAL(
        FileName(static_cast<CBTF_Protocol_FileName>(name1)), name1
        );
    BOOST_CHECK_EQUAL(
        FileName(static_cast<CBTF_Protocol_FileName>(name2)), name2
        );
}



/**
 * Unit test for the SymbolTable classes (LinkedObject, Function, Statement).
 */
BOOST_AUTO_TEST_CASE(TestSymbolTable)
{
    std::set<AddressRange> addresses;

    LinkedObject linked_object(FileName("/path/to/nonexistent/dso"));
    
    BOOST_CHECK_EQUAL(LinkedObject(linked_object), linked_object);
    BOOST_CHECK_EQUAL(linked_object.getFile(),
                      FileName("/path/to/nonexistent/dso"));

    //
    // The following functions and statements (along with their corresponding
    // address ranges) are added to this linked object during the test:
    //
    //      function1:  [  0,   7]  [ 13,  27]
    //      function2:  [113, 127]
    //      function3:  [  7,  13]  [213, 227]
    //      function4:  [ 57,  63]
    //
    //     statement1:  [  0,   7]  [113, 127]
    //     statement2:  [ 13,  27]
    //     statement3:  [ 75, 100]
    //     statement4:  [213, 227]
    //

    //
    // Test adding functions and the LinkedObject::visitFunctions query.
    //

    std::set<Function> functions;
    linked_object.visitFunctions(
        boost::bind(accumulate<Function>, _1, boost::ref(functions))
        );
    BOOST_CHECK(functions.empty());

    Function function1(linked_object, "_Z2f1RKf");

    BOOST_CHECK_EQUAL(Function(function1), function1);
    BOOST_CHECK_EQUAL(function1.getLinkedObject(), linked_object);
    BOOST_CHECK_EQUAL(function1.getMangledName(), "_Z2f1RKf");
    BOOST_CHECK_EQUAL(function1.getDemangledName(), "f1(float const&)");
    BOOST_CHECK(function1.getAddressRanges().empty());

    Function function2(linked_object, "_Z2f2RKf");
    Function function3(linked_object, "_Z2f3RKf");
    Function function4(function3.clone(linked_object));

    BOOST_CHECK_NE(function1, function2);
    BOOST_CHECK_LT(function1, function2);
    BOOST_CHECK_GT(function3, function2);
    BOOST_CHECK_NE(function3, function4);

    BOOST_CHECK(!equivalent(function1, function2));
    BOOST_CHECK(equivalent(function3, function4));

    functions.clear();
    linked_object.visitFunctions(
        boost::bind(accumulate<Function>, _1, boost::ref(functions))
        );
    BOOST_CHECK_EQUAL(functions.size(), 4);

    //
    // Test adding statements and the LinkedObject::visitStatements query.
    //

    std::set<Statement> statements;
    linked_object.visitStatements(
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK(statements.empty());
    
    Statement statement1(linked_object,
                         FileName("/path/to/nonexistent/source/file"), 1, 1);

    BOOST_CHECK_EQUAL(Statement(statement1), statement1);
    BOOST_CHECK_EQUAL(statement1.getLinkedObject(), linked_object);
    BOOST_CHECK_EQUAL(statement1.getFile(),
                      FileName("/path/to/nonexistent/source/file"));
    BOOST_CHECK_EQUAL(statement1.getLine(), 1);
    BOOST_CHECK_EQUAL(statement1.getColumn(), 1);
    BOOST_CHECK(statement1.getAddressRanges().empty());

    Statement statement2(linked_object,
                         FileName("/path/to/nonexistent/source/file"), 20, 1);
    Statement statement3(linked_object,
                         FileName("/path/to/nonexistent/source/file"), 30, 1);
    Statement statement4(statement3.clone(linked_object));
    
    BOOST_CHECK_NE(statement1, statement2);
    BOOST_CHECK_LT(statement1, statement2);
    BOOST_CHECK_GT(statement3, statement2);
    BOOST_CHECK_NE(statement3, statement4);

    BOOST_CHECK(!equivalent(statement1, statement2));
    BOOST_CHECK(equivalent(statement3, statement4));

    statements.clear();
    linked_object.visitStatements(
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK_EQUAL(statements.size(), 4);

    //
    // Add address ranges to the functions.
    //

    addresses = boost::assign::list_of
        (AddressRange(0, 7))
        (AddressRange(13, 27));
    function1.addAddressRanges(addresses);

    addresses = boost::assign::list_of
        (AddressRange(113, 127));
    function2.addAddressRanges(addresses);

    addresses = boost::assign::list_of
        (AddressRange(7, 13))
        (AddressRange(213, 227));
    function3.addAddressRanges(addresses);
    
    addresses = boost::assign::list_of
        (AddressRange(57, 63));
    function4.addAddressRanges(addresses);
    
    BOOST_CHECK(!function1.getAddressRanges().empty());
    BOOST_CHECK(!function2.getAddressRanges().empty());
    BOOST_CHECK(!function3.getAddressRanges().empty());
    BOOST_CHECK(!function4.getAddressRanges().empty());

    //
    // Test the LinkedObject::visitFunctions(<address_range>) query.
    //

    functions.clear();
    linked_object.visitFunctions(
        AddressRange(0, 10),
        boost::bind(accumulate<Function>, _1, boost::ref(functions))
        );
    BOOST_CHECK_EQUAL(functions.size(), 2);
    BOOST_CHECK(functions.find(function1) != functions.end());
    BOOST_CHECK(functions.find(function2) == functions.end());
    BOOST_CHECK(functions.find(function3) != functions.end());
    BOOST_CHECK(functions.find(function4) == functions.end());

    functions.clear();
    linked_object.visitFunctions(
        AddressRange(200, 400),
        boost::bind(accumulate<Function>, _1, boost::ref(functions))
        );
    BOOST_CHECK_EQUAL(functions.size(), 1);
    BOOST_CHECK(functions.find(function1) == functions.end());
    BOOST_CHECK(functions.find(function2) == functions.end());
    BOOST_CHECK(functions.find(function3) != functions.end());
    BOOST_CHECK(functions.find(function4) == functions.end());

    functions.clear();
    linked_object.visitFunctions(
        AddressRange(8, 10),
        boost::bind(accumulate<Function>, _1, boost::ref(functions))
        );
    BOOST_CHECK(functions.empty());

    //
    // Add address ranges to the statements.
    //

    addresses = boost::assign::list_of
        (AddressRange(0, 7))
        (AddressRange(113, 127));
    statement1.addAddressRanges(addresses);

    addresses = boost::assign::list_of
        (AddressRange(13, 27));
    statement2.addAddressRanges(addresses);

    addresses = boost::assign::list_of
        (AddressRange(75, 100));
    statement3.addAddressRanges(addresses);
    
    addresses = boost::assign::list_of
        (AddressRange(213, 227));
    statement4.addAddressRanges(addresses);

    BOOST_CHECK(!statement1.getAddressRanges().empty());
    BOOST_CHECK(!statement2.getAddressRanges().empty());
    BOOST_CHECK(!statement3.getAddressRanges().empty());
    BOOST_CHECK(!statement4.getAddressRanges().empty());

    //
    // Test the LinkedObject::visitStatements(<address-range>) query.
    //

    statements.clear();
    linked_object.visitStatements(
        AddressRange(0, 20),
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK_EQUAL(statements.size(), 2);
    BOOST_CHECK(statements.find(statement1) != statements.end());
    BOOST_CHECK(statements.find(statement2) != statements.end());
    BOOST_CHECK(statements.find(statement3) == statements.end());
    BOOST_CHECK(statements.find(statement4) == statements.end());

    statements.clear();
    linked_object.visitStatements(
        AddressRange(90, 110),
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK_EQUAL(statements.size(), 1);
    BOOST_CHECK(statements.find(statement1) == statements.end());
    BOOST_CHECK(statements.find(statement2) == statements.end());
    BOOST_CHECK(statements.find(statement3) != statements.end());
    BOOST_CHECK(statements.find(statement4) == statements.end());

    statements.clear();
    linked_object.visitStatements(
        AddressRange(30, 40),
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK(statements.empty());

    //
    // Test the Function::visitDefinitions() query.
    //

    statements.clear();
    function1.visitDefinitions(
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK_EQUAL(statements.size(), 1);
    BOOST_CHECK(statements.find(statement1) != statements.end());
    BOOST_CHECK(statements.find(statement2) == statements.end());
    BOOST_CHECK(statements.find(statement3) == statements.end());
    BOOST_CHECK(statements.find(statement4) == statements.end());

    statements.clear();
    function2.visitDefinitions(
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK_EQUAL(statements.size(), 1);
    BOOST_CHECK(statements.find(statement1) != statements.end());
    BOOST_CHECK(statements.find(statement2) == statements.end());
    BOOST_CHECK(statements.find(statement3) == statements.end());
    BOOST_CHECK(statements.find(statement4) == statements.end());

    statements.clear();
    function3.visitDefinitions(
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK_EQUAL(statements.size(), 1);
    BOOST_CHECK(statements.find(statement1) != statements.end());
    BOOST_CHECK(statements.find(statement2) == statements.end());
    BOOST_CHECK(statements.find(statement3) == statements.end());
    BOOST_CHECK(statements.find(statement4) == statements.end());

    statements.clear();
    function4.visitDefinitions(
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK(statements.empty());

    //
    // Test the Function::visitStatements() query.
    //

    statements.clear();
    function1.visitStatements(
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK_EQUAL(statements.size(), 2);
    BOOST_CHECK(statements.find(statement1) != statements.end());
    BOOST_CHECK(statements.find(statement2) != statements.end());
    BOOST_CHECK(statements.find(statement3) == statements.end());
    BOOST_CHECK(statements.find(statement4) == statements.end());

    statements.clear();
    function2.visitStatements(
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK_EQUAL(statements.size(), 1);
    BOOST_CHECK(statements.find(statement1) != statements.end());
    BOOST_CHECK(statements.find(statement2) == statements.end());
    BOOST_CHECK(statements.find(statement3) == statements.end());
    BOOST_CHECK(statements.find(statement4) == statements.end());

    statements.clear();
    function3.visitStatements(
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK_EQUAL(statements.size(), 3);
    BOOST_CHECK(statements.find(statement1) != statements.end());
    BOOST_CHECK(statements.find(statement2) != statements.end());
    BOOST_CHECK(statements.find(statement3) == statements.end());
    BOOST_CHECK(statements.find(statement4) != statements.end());
    
    statements.clear();
    function4.visitStatements(
        boost::bind(accumulate<Statement>, _1, boost::ref(statements))
        );
    BOOST_CHECK(statements.empty());

    //
    // Test the Statement::visitFunctions() query.
    //

    functions.clear();
    statement1.visitFunctions(
        boost::bind(accumulate<Function>, _1, boost::ref(functions))
        );
    BOOST_CHECK_EQUAL(functions.size(), 3);
    BOOST_CHECK(functions.find(function1) != functions.end());
    BOOST_CHECK(functions.find(function2) != functions.end());
    BOOST_CHECK(functions.find(function3) != functions.end());
    BOOST_CHECK(functions.find(function4) == functions.end());

    functions.clear();
    statement2.visitFunctions(
        boost::bind(accumulate<Function>, _1, boost::ref(functions))
        );
    BOOST_CHECK_EQUAL(functions.size(), 2);
    BOOST_CHECK(functions.find(function1) != functions.end());
    BOOST_CHECK(functions.find(function2) == functions.end());
    BOOST_CHECK(functions.find(function3) != functions.end());
    BOOST_CHECK(functions.find(function4) == functions.end());

    functions.clear();
    statement3.visitFunctions(
        boost::bind(accumulate<Function>, _1, boost::ref(functions))
        );
    BOOST_CHECK(functions.empty());

    functions.clear();
    statement4.visitFunctions(
        boost::bind(accumulate<Function>, _1, boost::ref(functions))
        );
    BOOST_CHECK_EQUAL(functions.size(), 1);
    BOOST_CHECK(functions.find(function1) == functions.end());
    BOOST_CHECK(functions.find(function2) == functions.end());
    BOOST_CHECK(functions.find(function3) != functions.end());
    BOOST_CHECK(functions.find(function4) == functions.end());

    //
    // Test the conversion of LinkedObject to/from CBTF_Protocol_SymbolTable
    // and LinkedObject::clone().
    //

    BOOST_CHECK(equivalent(
        LinkedObject(static_cast<CBTF_Protocol_SymbolTable>(linked_object)),
        linked_object
        ));

    LinkedObject clone = linked_object.clone();
    
    BOOST_CHECK((clone < linked_object) || (linked_object < clone));
    BOOST_CHECK_NE(clone, linked_object);

    BOOST_CHECK(equivalent(clone, linked_object));
    Function function5(clone, "_Z2f5RKf");
    BOOST_CHECK(!equivalent(clone, linked_object));
}



/**
 * Unit test for the Time class.
 */
BOOST_AUTO_TEST_CASE(TestTime)
{
    BOOST_CHECK_EQUAL(Time(), Time::TheBeginning());
    BOOST_CHECK_NE(Time::TheBeginning(), Time::TheEnd());
    BOOST_CHECK_LT(Time::TheBeginning(), Time::TheEnd());
    BOOST_CHECK_GT(Time::TheEnd(), Time::TheBeginning());

    Time t1 = Time::Now();
    BOOST_CHECK_GT(t1, Time::TheBeginning());
    BOOST_CHECK_LT(t1, Time::TheEnd());
    Time t2 = Time::Now();
    BOOST_CHECK_GT(t2, Time::TheBeginning());
    BOOST_CHECK_GT(t2, t1);
    BOOST_CHECK_LT(t2, Time::TheEnd());

    BOOST_CHECK_EQUAL(Time(static_cast<CBTF_Protocol_Time>(Time(27))),
                      Time(27));

    BOOST_CHECK_EQUAL(--Time::TheBeginning(), Time::TheEnd());
    BOOST_CHECK_EQUAL(Time::TheBeginning() - 1, Time::TheEnd());

    BOOST_CHECK_EQUAL(Time::TheBeginning(), ++Time::TheEnd());
    BOOST_CHECK_EQUAL(Time::TheBeginning(), Time::TheEnd() + 1);

    BOOST_CHECK_EQUAL(Time(27), Time(4) + Time(23));
    BOOST_CHECK_EQUAL(Time(27), Time(2) + 25);

    BOOST_CHECK_EQUAL(Time(27), Time(30) - Time(3));
    BOOST_CHECK_EQUAL(Time(27), Time(29) - 2);

    BOOST_CHECK_EQUAL(static_cast<std::string>(Time(13 * 27000000000)),
                      "1969/12/31 18:05:51");
}



/**
 * Unit test for the TimeInterval class.
 */
BOOST_AUTO_TEST_CASE(TestTimeInterval)
{
    BOOST_CHECK(TimeInterval().empty());
    BOOST_CHECK(!TimeInterval(0).empty());
    BOOST_CHECK_EQUAL(TimeInterval(27).begin(), TimeInterval(27).end());
    BOOST_CHECK(!TimeInterval(0, 1).empty());
    BOOST_CHECK(TimeInterval(1, 0).empty());

    BOOST_CHECK_EQUAL(
        TimeInterval(
            static_cast<CBTF_Protocol_TimeInterval>(TimeInterval(0, 27))
            ),
        TimeInterval(0, 27)
        );

    BOOST_CHECK_LT(TimeInterval(0, 13), TimeInterval(1, 13));
    BOOST_CHECK_LT(TimeInterval(0, 13), TimeInterval(0, 27));
    BOOST_CHECK_GT(TimeInterval(1, 13), TimeInterval(0, 13));
    BOOST_CHECK_GT(TimeInterval(0, 27), TimeInterval(0, 13));
    BOOST_CHECK_EQUAL(TimeInterval(0, 13), TimeInterval(0, 13));
    BOOST_CHECK_NE(TimeInterval(0, 13), TimeInterval(0, 27));

    BOOST_CHECK_EQUAL(TimeInterval(0, 13) | TimeInterval(7, 27),
                      TimeInterval(0, 27));
    BOOST_CHECK_EQUAL(TimeInterval(7, 27) | TimeInterval(0, 13),
                      TimeInterval(0, 27));
    BOOST_CHECK_EQUAL(TimeInterval(0, 7) | TimeInterval(13, 27),
                      TimeInterval(0, 27));
    BOOST_CHECK_EQUAL(TimeInterval(13, 27) | TimeInterval(0, 7),
                      TimeInterval(0, 27));
    BOOST_CHECK_EQUAL(TimeInterval(0, 7) | TimeInterval(),
                      TimeInterval(0, 7));
    BOOST_CHECK_EQUAL(TimeInterval() | TimeInterval(13, 27),
                      TimeInterval(13, 27));

    BOOST_CHECK_EQUAL(TimeInterval(0, 13) & TimeInterval(7, 27),
                      TimeInterval(7, 13));
    BOOST_CHECK_EQUAL(TimeInterval(7, 27) & TimeInterval(0, 13),
                      TimeInterval(7, 13));
    BOOST_CHECK((TimeInterval(0, 7) & TimeInterval(13, 27)).empty());
    BOOST_CHECK((TimeInterval(13, 27) & TimeInterval(0, 7)).empty());
    BOOST_CHECK((TimeInterval(0, 13) & TimeInterval()).empty());
    BOOST_CHECK((TimeInterval() & TimeInterval(0, 13)).empty());

    BOOST_CHECK_EQUAL(TimeInterval(0, 13).width(), Time(14));
    BOOST_CHECK_EQUAL(TimeInterval(13, 0).width(), Time(0));

    BOOST_CHECK(TimeInterval(0, 13).contains(7));
    BOOST_CHECK(!TimeInterval(0, 13).contains(27));
    BOOST_CHECK(!TimeInterval(13, 0).contains(7));
    BOOST_CHECK(!TimeInterval(13, 0).contains(27));

    BOOST_CHECK(TimeInterval(0, 27).contains(TimeInterval(7, 13)));
    BOOST_CHECK(!TimeInterval(0, 13).contains(TimeInterval(7, 27)));
    BOOST_CHECK(!TimeInterval(27, 0).contains(TimeInterval(7, 13)));
    BOOST_CHECK(!TimeInterval(13, 0).contains(TimeInterval(7, 27)));
    BOOST_CHECK(!TimeInterval(0, 27).contains(TimeInterval(13, 7)));
    BOOST_CHECK(!TimeInterval(0, 13).contains(TimeInterval(27, 7)));    
    BOOST_CHECK(!TimeInterval(27, 0).contains(TimeInterval(13, 7)));
    BOOST_CHECK(!TimeInterval(13, 0).contains(TimeInterval(27, 7)));

    BOOST_CHECK(TimeInterval(0, 27).intersects(TimeInterval(7, 13)));
    BOOST_CHECK(TimeInterval(0, 13).intersects(TimeInterval(7, 27)));
    BOOST_CHECK(!TimeInterval(27, 0).intersects(TimeInterval(7, 13)));
    BOOST_CHECK(!TimeInterval(13, 0).intersects(TimeInterval(7, 27)));
    BOOST_CHECK(!TimeInterval(0, 27).intersects(TimeInterval(13, 7)));
    BOOST_CHECK(!TimeInterval(0, 13).intersects(TimeInterval(27, 7)));    
    BOOST_CHECK(!TimeInterval(27, 0).intersects(TimeInterval(13, 7)));
    BOOST_CHECK(!TimeInterval(13, 0).intersects(TimeInterval(27, 7)));
    
    BOOST_CHECK_EQUAL(static_cast<std::string>(TimeInterval(13, 27000000000)),
                      "[1969/12/31 18:00:00, 1969/12/31 18:00:27]");

    BOOST_CHECK(!std::less<TimeInterval>()(
                    TimeInterval(0, 13), TimeInterval(7, 27)
                    ));
    BOOST_CHECK(!std::less<TimeInterval>()(
                    TimeInterval(7, 27), TimeInterval(0, 13)
                    ));
    BOOST_CHECK(!std::less<TimeInterval>()(
                    TimeInterval(7, 13), TimeInterval(7, 27)
                    ));
    BOOST_CHECK(!std::less<TimeInterval>()(
                    TimeInterval(7, 27), TimeInterval(7, 13)
                    ));
    BOOST_CHECK(!std::less<TimeInterval>()(
                    TimeInterval(0, 7), TimeInterval(7, 27)
                    ));
    BOOST_CHECK(!std::less<TimeInterval>()(
                    TimeInterval(7, 27), TimeInterval(0, 7)
                    ));
    BOOST_CHECK(std::less<TimeInterval>()(
                    TimeInterval(0, 7), TimeInterval(13, 27)
                    ));
    BOOST_CHECK(!std::less<TimeInterval>()(
                    TimeInterval(13, 27), TimeInterval(0, 7)
                    ));
}
