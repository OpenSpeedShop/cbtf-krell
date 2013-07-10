////////////////////////////////////////////////////////////////////////////////
// Memory.h used for Memory.cpp
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
