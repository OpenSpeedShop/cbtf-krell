/**
 * Statistics is a class created to handle fairly generic input and output from 
 * a CBTF experiment.  It's supposed to be used in conjunction with the
 * statistics plugin.
 */

/**
 * Helper class that allows for identification of certain values.
 * Generally useful for identifying the high and low water marks.
 */
template <typename T>
ValIdPair<T>::ValIdPair() {
    val_id.second = std::string("Unknown");
}

template <typename T>
ValIdPair<T>::ValIdPair(T value, std::string id) {
    val_id.first = value;
    val_id.second = id;
}

template <typename T>
T ValIdPair<T>::getVal() const {
    return val_id.first;
}

template <typename T>
std::string ValIdPair<T>::getId() const {
    return val_id.second;
}

template <typename T>
bool ValIdPair<T>::operator <(ValIdPair<T> p) {
    return (val_id.first < p.getVal());
}

template <typename T>
bool ValIdPair<T>::operator >(ValIdPair<T> p) {
    return (val_id.first > p.getVal());
}

template <typename T>
std::ostream& operator <<(std::ostream& out,
        const ValIdPair<T> p) {
    out << "value of "
        << p.getId()
        << " is "
        << p.getVal();
    return out;
}

//Default constructor
template <typename T>
Statistics<T>::Statistics() {
    popSize = 0;
}

//Constructor from a single datapoint
template <typename T>
Statistics<T>::Statistics(T value, std::string id) {
    sum = value;
    sumSq = value*value;
    popSize = 1;
    highWM = ValIdPair<T>(value, id);
    lowWM = ValIdPair<T>(value, id);
}

//Constructor from existing stats
//Useful when gathering data from a packet
template <typename T>
Statistics<T>::Statistics(T sum,
        T sumSq,
        unsigned long long popSize,
        T highWM,
        T lowWM,
        std::string hwmId,
        std::string lwmId) {
    this->sum = sum;
    this->sumSq = sumSq;
    this->popSize = popSize;
    this->highWM = ValIdPair<T>(highWM, hwmId);
    this->lowWM = ValIdPair<T>(lowWM, lwmId);
}

//Similar to the previous constructor, without
//explicit ids
template <typename T>
Statistics<T>::Statistics(T sum,
        T sumSq,
        unsigned long long popSize,
        ValIdPair<T> highWM,
        ValIdPair<T> lowWM) {
    this->sum = sum;
    this->sumSq = sumSq;
    this->popSize = popSize;
    this->highWM = highWM;
    this->lowWM = lowWM;
}

//"Clears" data
template <typename T>
void Statistics<T>::clear() {
    popSize = 0;
}

//Calculate the mean across the current values
template <typename T>
long double Statistics<T>::calculateMean() const {
    long double mean = 0;

    if (popSize > 0) {
        mean =  (1/((long double) popSize) * sum);
    }

    return mean;
}

// This calculates the standard deviation of the sample
// not the sample standard deviation.
template <typename T>
long double Statistics<T>::calculateStdDevOfSamp() const {
    long double stdDev = 0;
    long double mean;

    if (popSize > 0) {
        mean = calculateMean();
        stdDev = sqrt(1/((long double) popSize)*
                (long double)sumSq - mean*mean); 
    }
    return stdDev;
}

//Calculates the sample standard deviation.  Not sure if this will
//be used, depends on the experiment.
template <typename T>
long double Statistics<T>::calculateSampStdDev() const {
    long double sStdDev = 0;
    long double mean;

    if (popSize > 1) {
        mean = calculateMean();
        sStdDev = sqrt(1/((long double)popSize - 1) * 
                ((long double)sumSq - popSize*mean*mean));
    }
    return sStdDev;
}

//Gives either the sample standard deviation or the standard deviation
//of the sample based on whether or not this is a sample population.
template <typename T>
long double Statistics<T>::calculateStdDev(bool isSamplePop) const {
    long double stdDev;

    if (isSamplePop) {
        stdDev = calculateStdDevOfSamp();
    } else {
        stdDev = calculateSampStdDev();
    }
    return stdDev;
}

//Adds a single value to the current stats
template <typename T>
void Statistics<T>::addValue(T value, std::string id) {

    ValIdPair<T> newPair(value, id);
    if (popSize > 0) {
        sum = sum + value;
        sumSq = sumSq + value*value;
        highWM = MAXIMUM(highWM, newPair);
        lowWM = MINIMUM(lowWM, newPair);
    } else {
        sum = value;
        sumSq = value*value;
        highWM = newPair;
        lowWM = newPair;
    }
    popSize++;
}

//Adds a single value to the current stats
template <typename T>
void Statistics<T>::addValue(ValIdPair<T> newPair) {

    if (popSize > 0) {
        sum = sum + newPair.getVal();
        sumSq = sumSq + newPair.getVal()*newPair.getVal();
        highWM = MAXIMUM(highWM, newPair);
        lowWM = MINIMUM(lowWM, newPair);
    } else {
        sum = newPair.getVal();
        sumSq = newPair.getVal()*newPair.getVal();
        highWM = newPair;
        lowWM = newPair;
    }
    popSize++;
}

//Add an array of values to the current stats
template <typename T>
void Statistics<T>::addValueArray(ValIdPair<T> * values, int size) {
    for (int i = 0; i < size; i++) {
        addValue(values[i]);
    }
}

//Add a vector of values to the current stats
template <typename T>
void Statistics<T>::addValueVector(std::vector<ValIdPair<T> > values) {
    typename std::vector<T>::iterator it;

    for (it = values.begin(); it < values.end(); it++) {
        addValue(*it);
    }
}

//Takes an input stat and adds its values to the current stats
template <typename T>
void Statistics<T>::mergeStatistics(Statistics<T> stats) {
    if (popSize > 0) {
        sum = sum + stats.getSum();
        sumSq = sumSq + stats.getSumSq();
        highWM = findHighWM(stats);
        lowWM = findLowWM(stats);
        popSize = popSize + stats.getPopSize();
    } else {
        sum = stats.getSum();
        sumSq = stats.getSumSq();
        highWM = stats.getHighWM();
        lowWM = stats.getLowWM();
        popSize = stats.popSize;
    }
}

//This will return the high water mark between
//the current set of stats and the incoming stats
template <typename T>
ValIdPair<T> Statistics<T>::findHighWM(Statistics<T> s) const {
    //We can think of a highWM for a population
    //of 0 as being -infinity
    if (popSize == 0 && s.getPopSize() > 0) {
        return (s.getHighWM());
    } else if (popSize > 0 &&
            s.getPopSize() == 0) {
        return (highWM);
    } else if (popSize == 0 &&
            s.getPopSize() == 0) {
        /* This should be 'undefined' but in reality
           it shouldn't matter at all since neither
           value will 'win' over another Stat with
           a positive population value */
        return (highWM);
    }
    ValIdPair<T> tmp = highWM;
    return MAXIMUM(tmp, s.getHighWM());
}

//This will return the high water mark between
//the current set of stats and the incoming stats
template <typename T>
ValIdPair<T> Statistics<T>::findLowWM(Statistics<T> s) const {
    //We can think of a lowWM for a population
    //of 0 as being infinity
    if (popSize == 0 && s.getPopSize() > 0) {
        return (s.getLowWM());
    } else if (popSize > 0 &&
            s.getPopSize() == 0) {
        return (lowWM);
    } else if (popSize == 0 &&
            s.getPopSize() == 0) {
        /* This should be 'undefined' but in reality
           it shouldn't matter at all since neither
           value will 'win' over another Stat with
           a positive population value */
        return (lowWM);
    }
    ValIdPair<T> tmp = lowWM;
    return MINIMUM(tmp, s.getLowWM());
}

//Since it's so easy to add stats together this
//might come in handy at some point
template <typename T>
Statistics<T> Statistics<T>::operator +(Statistics<T> s) {
    Statistics<T> retStatistics;

    retStatistics = Statistics<T> (
            sum + s.getSum(),
            sumSq + s.getSumSq(),
            popSize + s.getPopSize(),
            findHighWM(s),
            findLowWM(s)
            );

    return retStatistics;
}

//Comparison operators might also come in
//handy at some point
template <typename T>
bool Statistics<T>::operator >(Statistics<T> s) {
    //Similar reasoning to findHighWM
    //XXX Note:this isn't even partially ordered
    if (popSize == 0 && s.getPopSize() > 0) {
        return (false);
    } else if (popSize > 0 &&
            s.getPopSize() == 0) {
        return (true);
    } else if (popSize == 0 &&
            s.getPopSize() == 0) {
        return (false);
    }
    return (highWM > s.getHighWM());
}

template <typename T>
bool Statistics<T>::operator <(Statistics<T> s) {
    //Similar reasoning to findLowWM
    //XXX Note:this isn't even partially ordered
    if (popSize == 0 && s.getPopSize() > 0) {
        return (true);
    } else if (popSize > 0 &&
            s.getPopSize() == 0) {
        return (false);
    } else if (popSize == 0 &&
            s.getPopSize() == 0) {
        return (false);
    }
    return (lowWM < s.getLowWM());
}

//Getter for sum
template <typename T>
T Statistics<T>::getSum() const {
    return sum;
}

//Getter for the sum of the squares
template <typename T>
T Statistics<T>::getSumSq() const {
    return sumSq;
}

//Getter for the current sample size
template <typename T>
unsigned long long Statistics<T>::getPopSize() const {
    return popSize;
}

//Getter for the high water mark
template <typename T>
ValIdPair<T> Statistics<T>::getHighWM() const {
    return highWM;
}

//Getter for the low water mark
template <typename T>
ValIdPair<T> Statistics<T>::getLowWM() const {
    return lowWM;
}

//Header for the toString methods
template <typename T>
std::string Statistics<T>::statsHeaderString() const {
    std::stringstream io;
    io << "Statistics: "
        << "\n\tsum: " << sum
        << "\n\tsumSq: " << sumSq
        << "\n\tpopSize: " << popSize
        << "\n\thighWM: " << highWM
        << "\n\tlowWM: " << lowWM
        << "\n\tmean: " << calculateMean();
    return io.str();
}


//Takes the current state and turns it into a string
template <typename T>
std::string Statistics<T>::toString() const {
    std::stringstream io;
    io << statsHeaderString()
        << "\n\tstandard dev. of sample: " << calculateStdDevOfSamp()
        << "\n\tsample standard dev.: " << calculateSampStdDev()
        << "\n";
    return io.str();
}

//Output stats based on whether or not this is a sample population
template <typename T>
std::string Statistics<T>::toString(bool isSamplePop) const {
    std::stringstream io;
    io << statsHeaderString()
        << "\n\tstandard dev.: " << calculateStdDev(isSamplePop)
        << "\n";
    return io.str();
}

template <typename T>
std::ostream & operator << (std::ostream& out, 
        const Statistics<T> stats) {
    out << stats.toString();
    return out;
}
