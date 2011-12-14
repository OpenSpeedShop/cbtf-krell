#ifndef STATS_H
#define STATS_H

#include <cstdio>
#include <cfloat>
#include <sstream>
#include <cmath>
#include <vector>

#define max(a,b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })
		   
#define min(a,b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a < _b ? _a : _b; })


/* Template for a class that gathers statistics
   on a given datapoint.  Only really supports
   elements of the real numbers, but that makes
   sense right now.
   This should mainly be used with the statistics
   plugin for CBTF.
*/
template <typename T>
class Statistics {
	private:
		// Sum of current data
		T sum;
		// Sum of the squares of current data
		T sumSq;
		//total sample size, unsigned long long
		//to support large sample populations
		unsigned long long popSize;
		//Maximal data point
		T highWM;
		//Minimal data point
		T lowWM;

		//Calculations of standard deviation based
		//on if the sample is the entire population
		long double calculateStdDevOfSamp() const;
		long double calculateSampStdDev() const;

		//Information common to all toString methods
		std::string statsHeaderString() const;

		//Finds the high and low water marks in light
		//of the fact that populations can be zero
		T findHighWM(Statistics<T> stats) const;
		T findLowWM(Statistics<T> stats) const;
	public:
		//Constructors
		Statistics();
		Statistics(T value);
		Statistics(T sum,
			T sumSq,
			unsigned long long popSize,
			T highWM,
			T lowWM);

		//Add outside data
		void addValue(T value);
		void addValueArray(T * value, int size);
		void addValueVector(std::vector<T>);
		void mergeStatistics(Statistics stats);

		//Calculate the values we're interested in
		long double calculateMean() const;
		long double calculateStdDev(bool isSamplePop) const;

		//Some potentially useful operators
		Statistics operator +(Statistics s);
		bool operator >(Statistics s);
		bool operator <(Statistics s);

		//Getters
		T getSum() const;
		T getSumSq() const;
		unsigned long long getPopSize() const;
		T getHighWM() const;
		T getLowWM() const;

		//Take output to string
		std::string toString() const;
		std::string toString(bool isSamplePop) const;
};

#include "Statistics.cpp"

#endif
