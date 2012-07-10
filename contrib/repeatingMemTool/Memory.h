#ifndef MEMORY_H
#define MEMORY_H
#include <sstream>

class NodeMemory {
    private:
        float total;
        float application;
        std::string id;

    public:
        NodeMemory();
        void setTotal(float total);
        float getTotal() const;

        void setApplication(float application);
        float getApplication() const;

        void setId(std::string id);
        void setId(char * id);
        std::string getId() const;

        int setApplicationPercent(float ap);

        /* This can be used for both sorting
         * and statistics generation.
         */
        bool operator < (NodeMemory nm) const;
        bool operator > (NodeMemory nm) const;
        NodeMemory operator + (NodeMemory nm) const;
        NodeMemory operator * (NodeMemory nm) const;
        long double operator / (long long int pop) const;

        std::string toString() const;
};

#endif
