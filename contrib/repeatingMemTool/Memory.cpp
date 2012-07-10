#include "Memory.h"

NodeMemory::NodeMemory() {
    this->total = 0;
    this->application = 0;
    this->id = std::string("Unknown");
}

int NodeMemory::setApplicationPercent(float percent) {
    if (this->total != 0) {
        this->application = percent/100.0 * this->total;
        return 0;
    }

    //Unable to properly set application size
    return 1;
}

void NodeMemory::setTotal(float total) {
    this->total = total;
}

float NodeMemory::getTotal() const {
    return(this->total);
}

void NodeMemory::setApplication(float application) {
    this->application = application;
}

float NodeMemory::getApplication() const {
    return(this->application);
}

void NodeMemory::setId(std::string id) {
    this->id = id;
}

void NodeMemory::setId(char * id) {
    this->id = std::string(id);
}

std::string NodeMemory::getId() const {
    return(this->id);
}

bool NodeMemory::operator < (NodeMemory nm) const {
    return (this->application < nm.getApplication());
}

bool NodeMemory::operator > (NodeMemory nm) const {
    return (this->application > nm.getApplication());
}

NodeMemory NodeMemory::operator + (NodeMemory nm) const {
    NodeMemory result;

    result.setTotal(this->total);
    result.setApplication(this->application + nm.getApplication());
    result.setId(this->id + ":" + nm.getId());

    return result;
}

NodeMemory NodeMemory::operator * (NodeMemory nm) const {
    NodeMemory result;

    result.setTotal(this->total);
    result.setApplication(this->application * nm.getApplication());
    result.setId(this->id + ":" + nm.getId());

    return result;
}

long double NodeMemory::operator / (long long int pop) const {
    long double result;

    result = (long double) this->application/(float) pop;

    return result;
}

std::string NodeMemory::toString() const {
    std::stringstream ss;
    ss << "ID: "
        << this->id
        << "\nApplication Memory: "
        << this->application
        << "\nTotal Memory: "
        << this->total
        << "\n";
    return ss.str();
}
