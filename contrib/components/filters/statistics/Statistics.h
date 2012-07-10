#ifndef STATS_H
#define STATS_H

#include <cstdio>
#include <cfloat>
#include <sstream>
#include <cmath>
#include <vector>

#define MAXIMUM(a,b) \
    ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MINIMUM(a,b) \
    ({ __typeof__ (a) _a = (a); \
     __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

/* Template for the helper class to Statistics.
   This will help identify high and low water
   marks.
   */
template <typename T>
class ValIdPair {
    private:
        std::pair<T, std::string> val_id;

    public:
        ValIdPair();
        ValIdPair(T value, std::string id);
        T getVal() const;
        std::string getId() const;
        bool operator >(const ValIdPair<T> p);
        bool operator <(const ValIdPair<T> p);
        template <typename C>
            friend std::ostream& operator <<(std::ostream& out,
                    const ValIdPair<C> p);
};

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
        ValIdPair<T> highWM;
        //Minimal data point
        ValIdPair<T> lowWM;

        //Calculations of standard deviation based
        //on if the sample is the entire population
        long double calculateStdDevOfSamp() const;
        long double calculateSampStdDev() const;

        //Information common to all toString methods
        std::string statsHeaderString() const;

        //Finds the high and low water marks in light
        //of the fact that populations can be zero
        ValIdPair<T> findHighWM(Statistics<T> stats) const;
        ValIdPair<T> findLowWM(Statistics<T> stats) const;

    public:
        //Constructors
        Statistics();
        Statistics(T value, std::string id);
        Statistics(T sum,
                T sumSq,
                unsigned long long popSize,
                T highWM,
                T lowWM,
                std::string hwmId,
                std::string lwmId);
        Statistics(T sum,
                T sumSq,
                unsigned long long popSize,
                ValIdPair<T> highWM,
                ValIdPair<T> lowWM);

        //"Clears" out data
        void clear();

        //Add outside data
        void addValue(T value, std::string id);
        void addValue(ValIdPair<T> newPair);
        void addValueArray(ValIdPair<T> * value, int size);
        void addValueVector(std::vector<ValIdPair<T> >);
        void mergeStatistics(Statistics<T> stats);

        //Calculate the values we're interested in
        long double calculateMean() const;
        long double calculateStdDev(bool isSamplePop) const;

        //Some potentially useful operators
        Statistics operator +(Statistics<T> s);
        bool operator >(Statistics<T> s);
        bool operator <(Statistics<T> s);

        //Getters
        T getSum() const;
        T getSumSq() const;
        unsigned long long getPopSize() const;
        ValIdPair<T> getHighWM() const;
        ValIdPair<T> getLowWM() const;

        //Take output to string
        std::string toString() const;
        std::string toString(bool isSamplePop) const;
        template <typename C>
            friend std::ostream& operator << (std::ostream& out,
                    const Statistics<C> stats);
};

#include "Statistics.cpp"

#endif
