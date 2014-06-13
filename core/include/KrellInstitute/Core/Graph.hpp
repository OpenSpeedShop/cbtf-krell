////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 The Krell Institute. All Rights Reserved.
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

/** @file
 *
 * Declaration of the Graph class.
 *
 */

#ifndef _KrellInstitute_Core_Graph_
#define _KrellInstitute_Core_Graph_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/version.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/directed_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>
#include <KrellInstitute/Core/Address.hpp>
#include <KrellInstitute/Core/Path.hpp>
#include <KrellInstitute/Core/ThreadName.hpp>

// Use a directed graph.  Callstacks essentially start from main.
//
// A vertex represents a callstack address and any threadname at that
// address. an edge represents a path from a caller to a callee.
//
// Add a vertex for each new frame in a callstack.  In the case where
// we are using raw sample addresses, the last callstack entry is the address
// of the frame where a sample or trace was taken and will have a count.
//
// A callstack frame address can likely be in a range that represents a
// single line or single function.  If we where using symbols, the graph
// would certainly be smaller.
//
// Add an edge for any vertex to vertex that does not exist, else update
// an existing edge with any data as needed (eg, counts).
//

namespace {
}

namespace KrellInstitute { namespace Core {
    

    class ThreadName;
    class Address;

    typedef std::vector<ThreadName> ThreadNameVec;

    struct Edge{
	uint64_t cost;
    };

    struct Vertex{
	//int id;
        std::string name;
        Address a;
	//ThreadNameVec threads;
	//std::pair<Address,ThreadNameVec> addrThreads;
	//Address addr;
    };

    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex, Edge> DirectedGraph;
    typedef boost::graph_traits<DirectedGraph>::vertex_descriptor vertex_t;
    typedef boost::graph_traits<DirectedGraph>::edge_descriptor edge_t;


    /**
     * Graph (adjacency list).
     *
     * @sa  http://www.boost.org/doc/libs/1_36_0/libs/graph/doc/adjacency_list.html 
     *
     * @ingroup Core
     */
    class Graph
    {

    public:

	Graph();
	Graph(const Graph&);
	~Graph();
	
	Graph& operator=(const Graph&);
	
	bool addEdge(Vertex&, Vertex&);

	bool addEdge(Address&, Address&, const uint64_t&);

        void printGraph();

	bool isEmpty() const;
	DirectedGraph dm_dg;
        std::map<Address, vertex_t> addr2vertex;

    private:

    };
    

} }



#endif
