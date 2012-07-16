#ifndef MEMORY_H
#define MEMORY_H
#include <iostream>
#include <sstream>

class NodeMemory {
    private:
        long double total;
        long double application;
        std::string id;

    public:
        NodeMemory();
        void setTotal(long double total);
        long double getTotal() const;

        void setApplication(long double application);
        long double getApplication() const;

        void setId(std::string id);
        void setId(char * id);
        std::string getId() const;

        int setApplicationPercent(long double ap);

        /* This can be used for both sorting
         * and statistics generation.
         */
        bool operator < (NodeMemory nm) const;
        bool operator > (NodeMemory nm) const;
        NodeMemory operator + (NodeMemory nm) const;
        NodeMemory operator * (NodeMemory nm) const;
        long double operator / (long long int pop) const;

        std::string toString() const;
        friend std::ostream& operator << (std::ostream& out,
                const NodeMemory nm);
};

#endif
