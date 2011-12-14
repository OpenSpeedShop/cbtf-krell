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

/** @file Plugin that converts input to statistics (mean, standard deviation,
  * high water mark, and low water mark)
  */
#pragma once

#include <boost/bind.hpp>
#include <boost/operators.hpp>
#include <mrnet/Packet.h>
#include <typeinfo>
#include <algorithm>

#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/Version.hpp>
#include <KrellInstitute/CBTF/Impl/MRNet.hpp>

#include "Statistics.h"
#include "ByteTransform.h"

#define TOTAL_CHILDREN Impl::TheTopologyInfo.NumChildren

//Just a tag for passing stats packets via MRNet
//XXX Not sure if this is the correct way to do it
#define STATS_TAG 0

namespace KrellInstitute { namespace CBTF {


/**
 * Component that takes a single value and adds it to the running
 * statistics.
 */

template <typename T>
class __attribute__ ((visibility ("hidden"))) ValueToStatistics:
//Standard component header stuff
public Component {
	public:
		static Component::Instance factoryFunction() {
			return Component::Instance(
					reinterpret_cast<Component*>(new ValueToStatistics())
					);
		}
	private:
		//Statistics object created from the data collected
		Statistics<T> stats;
		//How many children have reported in so far
		int numChildren;

		ValueToStatistics():

		Component(Type(typeid(ValueToStatistics)), Version(0, 0, 1)) {
			numChildren = 0;
			stats = Statistics<T>();
			declareInput<T>(
					"ValueIn", boost::bind(&ValueToStatistics::inHandler, this, _1)
					);
			declareOutput< Statistics<T> >("StatisticsOut");
		}

		void inHandler(const T & value) {
			numChildren++;

			//add a datapoint to our current stats
			stats.addValue(value);

			//Finally emit output if all children have reported in
			if (numChildren >= TOTAL_CHILDREN) {
				emitOutput < Statistics <T> >("StatisticsOut", stats);
			}
		}
};

/**
 * Component that aggregates data for mean and stdev info
 */

template <typename T>
class __attribute__ ((visibility ("hidden"))) StatisticsAggregator:
//Standard component header stuff
public Component {
	public:
		static Component::Instance factoryFunction() {
			return Component::Instance(
					reinterpret_cast<Component*>(new StatisticsAggregator())
					);
		}
	private:
		//We will need to keep a running tab on the stats collected
		Statistics<T> aggregateStatistics;
		/* We also need to know how many children have reported in so we
		 * know when we're done aggregating stats.
		 */
		int numChildren;

		StatisticsAggregator():

		Component(Type(typeid(StatisticsAggregator)), Version(0, 0, 1))
		{
			aggregateStatistics = Statistics<T>();
			numChildren = 0;
			declareInput< Statistics <T> >(
					"StatisticsIn", boost::bind(&StatisticsAggregator::inAggHandler, this, _1)
					);
			declareOutput< Statistics <T> >("StatisticsOut");
		}

		void inAggHandler(const Statistics<T> & statsIn) {
			//Adds the incoming stats to the aggregate stats
			aggregateStatistics.mergeStatistics(statsIn);
			numChildren++;

			/* When the number of children collected from is equal to
			 * total number of children for the node we emit an output
			 */
			if (numChildren >= TOTAL_CHILDREN) {
				emitOutput < Statistics <T> >("StatisticsOut", aggregateStatistics);
			}
		}
};



/**
 * Component that displays the statistics info 
 */
template <typename T>
class __attribute__ ((visibility ("hidden"))) DisplayStatistics:
public Component {
	public:
		static Component::Instance factoryFunction() {
			return Component::Instance(
					reinterpret_cast<Component*>(new DisplayStatistics())
					);
		}
	private:
		DisplayStatistics():
			Component(Type(typeid(DisplayStatistics)), Version(0, 0, 1))	{
				declareInput< Statistics <T> >(
						"StatisticsIn", boost::bind(&DisplayStatistics::inDisplayHandler, this, _1)
						);
				declareOutput< Statistics<T> >("StatisticsOut");
			}

		//We'll just use the Statistics toString method to display all info so far
		void inDisplayHandler(const Statistics<T> & statsIn) {
			std::cout << statsIn.toString() << std::endl;
			//Pass the information through
			emitOutput< Statistics <T> >("StatisticsOut", statsIn);
		}
};


/**
 * Component that converts stats to MRNet packets
 * Only supports primitive types and structs consisting
 * of such.
 */

template <typename T>
class __attribute__ ((visibility ("hidden"))) ConvertStatisticsToPacket :
//Standard component construction header stuff
public Component {
	public:
		static Component::Instance factoryFunction() {
			return Component::Instance(
				reinterpret_cast<Component*>(new ConvertStatisticsToPacket())
				);
		}
	private:
		ConvertStatisticsToPacket():
			Component(Type(typeid(ConvertStatisticsToPacket)), Version(0, 0, 1)) {
				declareInput< Statistics <T> >(
						"StatisticsIn", boost::bind(&ConvertStatisticsToPacket::inStatisticsHandler, this, _1)
						);
				declareOutput<MRN::PacketPtr>("PacketOut");
			}
		
		void inStatisticsHandler(const Statistics<T> & statsIn) {
			/* Take our stats info and construct a new MRNet packet
			 * and push it out.
			 */
			unsigned char * sumCharArray = ByteTransform<T>::convert(statsIn.getSum());
			unsigned char * sumSqCharArray = ByteTransform<T>::convert(statsIn.getSumSq());
			unsigned char * highWMCharArray = ByteTransform<T>::convert(statsIn.getHighWM());
			unsigned char * lowWMCharArray = ByteTransform<T>::convert(statsIn.getLowWM());

			int sizes = ByteTransform<T>::getSize();
			emitOutput<MRN::PacketPtr>(
					"PacketOut",
				       	MRN::PacketPtr(
						new MRN::Packet(0,
							STATS_TAG,
							"%auc %auc %auc %auc %uld",
							sumCharArray, sizes,
							sumSqCharArray, sizes,
							highWMCharArray, sizes,
							lowWMCharArray, sizes,
							statsIn.getPopSize())
						)
					);
		}
};


/**
 * Component that converts MRNet packets to stats
 * Only supports primitive types and structs consisting
 * of such.
 */
template <typename T>
class __attribute__ ((visibility ("hidden"))) ConvertPacketToStatistics :
//Standard component construction header stuff
public Component {
	public:
		static Component::Instance factoryFunction() {
			return Component::Instance(
				reinterpret_cast<Component*>(new ConvertPacketToStatistics())
				);
		}
	private:
		ConvertPacketToStatistics():
			Component(Type(typeid(ConvertPacketToStatistics)), Version(0, 0, 1)) {
				declareInput<MRN::PacketPtr>(
						"PacketIn", boost::bind(&ConvertPacketToStatistics::inStatisticsHandler, this, _1)
						);
				declareOutput< Statistics <T> >("StatisticsOut");
			}
		
		void inStatisticsHandler(const MRN::PacketPtr & packetIn) {
			Statistics<T> stats;
			//Buffers for each piece of stat data
			unsigned char * sumBuff = NULL;
			unsigned char * sumSqBuff = NULL;
			unsigned char * highWMBuff = NULL;
			unsigned char * lowWMBuff = NULL;
			unsigned long long popBuff = 0;
			unsigned int sumBuffSize = 0;
			unsigned int sumSqBuffSize = 0;
			unsigned int highWMBuffSize = 0;
			unsigned int lowWMBuffSize = 0;
			
			//Unpack our packet into the buffers
			packetIn->unpack("%auc %auc %auc %auc %uld",
					&sumBuff,
					&sumBuffSize,
					&sumSqBuff,
					&sumSqBuffSize,
					&highWMBuff,
					&highWMBuffSize,
					&lowWMBuff,
					&lowWMBuffSize,
					&popBuff);

			//Construct a new Statistics object from the data and push it through
			stats = Statistics<T>(ByteTransform<T>::revert(sumBuff),
					ByteTransform<T>::revert(sumSqBuff),
					popBuff,
					ByteTransform<T>::revert(highWMBuff),
					ByteTransform<T>::revert(lowWMBuff));
			emitOutput< Statistics <T> >("StatisticsOut", stats);
		}
};

}};
//TODO This is all pretty useless if I can't convert blobs to stats.
