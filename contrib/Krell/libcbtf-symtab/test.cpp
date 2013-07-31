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

#include <boost/test/unit_test.hpp>
#include <KrellInstitute/SymbolTable/Address.hpp>
#include <KrellInstitute/SymbolTable/AddressRange.hpp>
#include <KrellInstitute/SymbolTable/AddressSpace.hpp>
#include <KrellInstitute/SymbolTable/FileName.hpp>
#include <KrellInstitute/SymbolTable/Function.hpp>
#include <KrellInstitute/SymbolTable/LinkedObject.hpp>
#include <KrellInstitute/SymbolTable/Statement.hpp>
#include <KrellInstitute/SymbolTable/Time.hpp>
#include <KrellInstitute/SymbolTable/TimeInterval.hpp>

#include "AddressBitmap.hpp"

using namespace KrellInstitute::SymbolTable;



/** Anonymous namespace hiding implementation details. */
namespace {

    // ...

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
    // ...
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
                      "[ 0x000000000000000D, 0x000000000000001B ]");

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
    // ...
}



/**
 * Unit test for the FileName class.
 */
BOOST_AUTO_TEST_CASE(TestFileName)
{
    // ...
}



/**
 * Unit test for the Function class.
 */
BOOST_AUTO_TEST_CASE(TestFunction)
{
    // ...
}



/**
 * Unit test for the LinkedObject class.
 */
BOOST_AUTO_TEST_CASE(TestLinkedObject)
{
    // ...
}



/**
 * Unit test for the Statement class.
 */
BOOST_AUTO_TEST_CASE(TestStatement)
{
    // ...
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
                      "[ 1969/12/31 18:00:00, 1969/12/31 18:00:27 ]");

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
