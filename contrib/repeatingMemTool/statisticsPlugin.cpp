////////////////////////////////////////////////////////////////////////////////
// statisticsPlugin.cpp statistics functions
// LACC #:  LA-CC-13-051
// Copyright (c) 2013 Anthony J. (TJ) Machado; HPC-3, LANL
// Copyright (c) 2013 Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
////////////////////////////////////////////////////////////////////////////////


#include "../components/filters/statistics/statisticsPlugin.hpp"

KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ValueToStatistics<long double>)
KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(StatisticsAggregator<long double>)
KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertStatisticsToPacket<long double>)
KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(ConvertPacketToStatistics<long double>)
KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(CreateValIdPair<long double>)
KRELL_INSTITUTE_CBTF_REGISTER_FACTORY_FUNCTION(DisplayStatistics<long double>)
