////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012 Krell Institute. All Rights Reserved.
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

/** @file topology support. */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cstring>
#include <string>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <stdio.h>
#include <typeinfo>
#include <limits.h>

#include <mrnet/MRNet.h>
#include <mrnet/Tree.h>
#include "KrellInstitute/Core/CBTFTopology.hpp"

using namespace KrellInstitute::Core;

#if 0
CBTFTopology::CBTFTopology() :
    nodes(NULL),
    topologyStr(NULL)
{
}
#endif

CBTFTopology::CBTFTopology()
{
}

CBTFTopology::~CBTFTopology()
{
}

void
CBTFTopology::AssignTopologyIndices( TopologyNode* n, 
                        std::map<std::string, unsigned int>& indexMap )
{
    // assign index to the current node
    n->index = indexMap[n->hostname];
    indexMap[n->hostname]++;
    
    // recursively assign indices to this node's descendants
    for( std::vector<TopologyNode*>::iterator iter = n->children.begin();
        iter != n->children.end();
        iter++ )
    {
        AssignTopologyIndices( *iter, indexMap );
    }
}

void
CBTFTopology::PrintTopology( TopologyNode* n, std::ostringstream& ostr )
{
    // we do not need to add anything if we are processing a leaf
    if( n->children.empty() )
    {
        return;
    }

    ostr << n->hostname << ":" << n->index << " =>\n";

    for( std::vector<TopologyNode*>::iterator iter = n->children.begin();
        iter != n->children.end();
        iter++ )
    {
        ostr << "    " << (*iter)->hostname << ':' << (*iter)->index << '\n';
    }
    ostr << "    ;\n";

    for( std::vector<TopologyNode*>::iterator iter = n->children.begin();
        iter != n->children.end();
        iter++ )
    {
        PrintTopology( *iter, ostr );
    }
}

void
CBTFTopology::BuildFlattenedTopology( unsigned int fanout,
                        int myNid, 
                        std::set<std::string>& nodes )
{
    // start with level for leaves of the tree network
    std::vector<TopologyNode*>* currLevel = new std::vector<TopologyNode*>;
    for( std::set<std::string>::const_iterator iter = nodes.begin();
         iter != nodes.end();
         iter++ )
    {
std::cerr << "CBTFTopology::BuildFlattenedTopology push a node" << std::endl;
        currLevel->push_back( new TopologyNode( *iter ) );
    }

    bool done = false;
    while( !done )
    {
        // start a new level
        std::vector<TopologyNode*>* oldLevel = currLevel;
        currLevel = new std::vector<TopologyNode*>;

        unsigned int nNodesThisLevel = (oldLevel->size() / fanout);
std::cerr << "CBTFTopology::BuildFlattenedTopology nNodesThisLevel " << nNodesThisLevel << std::endl;
        if( (oldLevel->size() % fanout) != 0 )
        {
            // we need one extra node for the remainder
            nNodesThisLevel++;
        }

        for( unsigned int i = 0; i < nNodesThisLevel; i++ )
        {
            TopologyNode* newNode = new TopologyNode( (*oldLevel)[i*fanout]->hostname );
            for( unsigned int j = 0; j < fanout; j++ )
            {
                if( (i*fanout + j) < oldLevel->size() )
                {
                    newNode->children.push_back( (*oldLevel)[i*fanout + j] );
                }
                else
                {
                    // we're done with this level
                    break;
                }
            }
            currLevel->push_back( newNode );
        }

        if( currLevel->size() == 1 )
        {
            // we have reached the root
            done = true;
        }
        delete oldLevel;    // don't delete the nodes in the old level vector!
    }
    assert( currLevel->size() == 1 );

    // replace root node (which above algorithm places on a backend node)
    // with FE node
    std::ostringstream ostr;
    ostr << "nid" 
            << std::setw( 5 ) 
            << std::setfill('0') 
            << myNid;
    (*currLevel)[0]->hostname = ostr.str();

std::cerr << "CBTFTopology::BuildFlattenedTopology FE node " << ostr.str() << std::endl;

    // now assign per-host indices to processes
    std::map<std::string, unsigned int> indexMap;
    for( std::set<std::string>::const_iterator iter = nodes.begin();
         iter != nodes.end();
         iter++ )
    {
        assert( indexMap.find(*iter) == indexMap.end() );
        indexMap[*iter] = 0;
    }
    AssignTopologyIndices( (*currLevel)[0], indexMap );

    // now dump the resulting topology
    std::ostringstream topoStr;
    PrintTopology( (*currLevel)[0], topoStr );

    // we're done with the topology structure
    delete (*currLevel)[0];
    delete currLevel;

    std::string topologyStr = topoStr.str();
    setTopologyStr(topologyStr);

std::cerr << "CBTFTopology::BuildFlattenedTopology topologyStr\n" << topologyStr << std::endl;
}



void CBTFTopology::setCommNodeList(const char *nodeList)
{
    char numString[BUFSIZE], nodeName[BUFSIZE], *nodeRange;
    unsigned int num1 = 0, num2, startPos, endPos, i, j;
    bool isRange = false;
    std::string baseNodeName, nodes, list;
    std::string::size_type openBracketPos, closeBracketPos, commaPos, currentPos, finalPos;

    list = nodeList;
    currentPos = 0;
    while (true) {
        openBracketPos = list.find_first_of("[");
        closeBracketPos = list.find_first_of("]");
        commaPos = list.find_first_of(",");

        if (openBracketPos == std::string::npos && commaPos == std::string::npos)
            finalPos = list.length(); /* Last one, just a single node */
        else if (commaPos < openBracketPos)
            finalPos = commaPos; /* just a single node */
        else  
            finalPos = closeBracketPos + 1;

        nodes = list.substr(0, finalPos);
        openBracketPos = nodes.find_first_of("[");
        closeBracketPos = nodes.find_first_of("]");
        commaPos = nodes.find_first_of(",");

        if (openBracketPos == std::string::npos && closeBracketPos == std::string::npos) {
            /* This is a single node */
            strncpy(nodeName, nodes.c_str(), BUFSIZE);
            dm_cp_nodelist.push_back(nodeName);
        } else {
            /* This is a list of nodes */
            /* Parse the node list string string e.g.: xxx[0-15,12,23,26-35] */
            nodeRange = strdup(nodes.substr(openBracketPos + 1, closeBracketPos -
				(openBracketPos + 1)).c_str());
            if (nodeRange == NULL) {
                return;
            }

            /* Get the machine name */
            baseNodeName = nodes.substr(0, openBracketPos);

            /* Decode the node list string  */
            for (i = 0; i < strlen(nodeRange); i++) {
                if (nodeRange[i] == ',') {
                    continue;
                } else if (isdigit(nodeRange[i])) {
                    startPos = i;
                    while (isdigit(nodeRange[i])) {
                        i++;
                        if (i >= strlen(nodeRange))
                            break;
                    }    
                    endPos = i - 1;
                } else {
                    return;
                }

                memcpy(numString, nodeRange + startPos, endPos-startPos + 1);
                numString[endPos-startPos + 1] = '\0';

                if (isRange) {
                    isRange = false;
                    num2 = atoi(numString);
                    for (j = num1; j <= num2; j++) {
                        snprintf(nodeName, BUFSIZE, "%s%u", baseNodeName.c_str(), j);
                        //if (checkNodeAccess(nodeName))
                        dm_cp_nodelist.push_back(nodeName);
                    }
                } else {
                    num1 = atoi(numString);
                    if (i < strlen(nodeRange)) {
                        if (nodeRange[i] == '-') {
                            isRange = true;
                            continue;
                        }
                    }
                    snprintf(nodeName, BUFSIZE, "%s%u", baseNodeName.c_str(), num1);
                    //if (checkNodeAccess(nodeName))
                    dm_cp_nodelist.push_back(nodeName);
                }
                i = i - 1;
            }

            if (nodeRange != NULL)
                free(nodeRange);
        }
        if (finalPos >= list.length())
            break;
        else    
            list = list.substr(finalPos + 1, list.length() - 1);
    }
}



void CBTFTopology::parseSlurmEnv()
{

    char *ptr;
    long t;
    std::string slurmjobid = getenv("SLURM_JOB_ID");
    if (slurmjobid.empty()) {
    } else {
	t = strtol(slurmjobid.c_str(), &ptr, 10);
	if (ptr == slurmjobid.c_str() /*|| !xstring_is_whitespace(ptr)*/ || t < 0) {
	    // problem
	    ;
	} else {
	}
    }
    
    std::string slurmnumnodes = getenv("SLURM_JOB_NUM_NODES");
    if (slurmnumnodes.empty()) {
    } else {
	t = strtol(slurmnumnodes.c_str(), &ptr, 10);
	if (ptr == slurmnumnodes.c_str() /*|| !xstring_is_whitespace(ptr)*/ || t < 0) {
	    // problem
	    ;
	} else {
	}
    }

    std::string slurmcpuspernode = getenv("SLURM_JOB_CPUS_PER_NODE");
    if (slurmcpuspernode.empty()) {
    } else {
	t = strtol(slurmcpuspernode.c_str(), &ptr, 10);
	if (ptr == slurmcpuspernode.c_str() /*|| !xstring_is_whitespace(ptr)*/ || t < 0) {
	    // problem
	    ;
	} else {
	}
    }

    std::string slurmnodelist = getenv("SLURM_JOB_NODELIST");
    if (slurmcpuspernode.empty()) {
    } else {
	setCommNodeList(slurmnodelist.c_str());
    }
}



void CBTFTopology::createTopology(char *topologyFileName, CBTFTopologyType topologyType,
				 char *topologySpecification, char *nodeList)
{
    FILE *file;
    char tmp[BUFSIZE], *topology = NULL;
    int parentCount, desiredDepth = 0, desiredMaxFanout = 0, procsNeeded = 0;
    unsigned int i, j, counter, layer, parentIter, childIter;
    unsigned int depth = 0, fanout = 0;
    std::vector<std::string> treeList;
    std::list<std::string>::iterator CPNodeListIter;
    std::multiset<std::string>::iterator applicationNodeSetIter;
    std::string topoIter, current;
    std::string::size_type dashPos, lastPos;

    /* Set parameters based on requested topology */
    if (topologyType == CBTF_TOPOLOGY_DEPTH) {
        desiredMaxFanout = CBTF_MAX_FANOUT;
        desiredDepth = atoi(topologySpecification);
    } else if (topologyType == CBTF_TOPOLOGY_FANOUT)
        desiredMaxFanout = atoi(topologySpecification);
    else if (topologyType == CBTF_TOPOLOGY_USER)
        topology = strdup(topologySpecification);
    else
        desiredMaxFanout = CBTF_MAX_FANOUT;

    /* Set the communication node list if we're not using a flat 1 to N tree */
    if ((desiredMaxFanout < dm_app_procs && desiredMaxFanout > 0) ||
	  topology != NULL || desiredDepth != 0) {
        if (nodeList == NULL) {
            // Better pass a node list.
            //statError = setNodeListFromConfigFile(&nodeList);
        }
        setCommNodeList(nodeList);
    }

    /* Set the requested topology and check if there are enough CPs specified */
    // If there is not a valid toplogy file...
    if (topology == NULL) {
        /* Determine the depth and fanout */
        if (desiredDepth == 0) {
            /* Find the desired depth based on the fanout and number of app nodes */
            for (desiredDepth = 1; desiredDepth < 1024; desiredDepth++) {
                fanout = (int)ceil(pow((float)dm_num_app_nodes, (float)1.0 / (float)desiredDepth));
                if (fanout <= desiredMaxFanout)
                    break;
            }
        } else
            fanout = (int)ceil(pow((float)dm_num_app_nodes, (float)1.0 / (float)desiredDepth));

        /* Determine the number of processes needed */
        procsNeeded = 0;
        for (i = 1; i < desiredDepth; i++) {
            if (i == 1) {
                topology = (char *)malloc(BUFSIZE);
                snprintf(topology, BUFSIZE, "%d", fanout);
            } else {
                snprintf(tmp, BUFSIZE, "-%d", (int)ceil(pow((float)fanout, (float)i)));
                strcat(topology, tmp);
            }
            procsNeeded += (int)ceil(pow((float)fanout, (float)i));
        }
        if (procsNeeded <= dm_cp_nodelist.size() * dm_procs_per_node) {
            /* We have enough CPs, so we can have our desired depth */
            depth = desiredDepth;
        } else {
            /* There aren't enough CPs, so make a 2-deep tree with as many CPs as we have */
            topology = (char *)malloc(BUFSIZE);
            if (topology == NULL) {
                perror("malloc failed to allocate for topology\n");
                return;
            }
            snprintf(topology, BUFSIZE, "%d", dm_cp_nodelist.size() * dm_procs_per_node);
        }
    } else {
        procsNeeded = 0;
        topoIter = topology;
        while(true) {
            dashPos = topoIter.find_first_of("-");
            if (dashPos == std::string::npos)
                lastPos = topoIter.length();
            else
                lastPos = dashPos;    
            current = topoIter.substr(0, lastPos);
            layer = atoi(current.c_str());
            procsNeeded += layer;
            if (lastPos >= topoIter.length())
                break;
            topoIter = topoIter.substr(lastPos + 1);
        }
        if (procsNeeded > dm_cp_nodelist.size() * dm_procs_per_node) {
            if (topology != NULL)
                free(topology);
            topology = (char *)malloc(BUFSIZE);
            if (topology == NULL) {
                perror("malloc failed to allocate for topology\n");
                return ;
            }
            snprintf(topology, BUFSIZE, "%d", dm_cp_nodelist.size() * dm_procs_per_node);
        }
    }

    /* Check if tool FE hostname is in application list and the communication 
       node list, then we will later add it to the comm nodes */

    /* Add the FE to the root of the tree */
#ifdef BGL
    /* On BlueGene systems we need the network interface with the IO nodes */
    snprintf(tmp, BUFSIZE, "%s-io", dm_fe_node.c_str());
    snprintf(hostname_, BUFSIZE, "%s", tmp);
#endif
    snprintf(tmp, BUFSIZE, "%s:0", dm_fe_node.c_str());
    treeList.push_back(tmp);

    /* Make sure the dm_procs_per_node is set */
    if (dm_procs_per_node <= 0)
    {
        return;
    }

    /* Add the nodes and IDs to the list of hosts */
    counter = 0;
    for (i = 0; i < dm_procs_per_node; i++) {
        for (CPNodeListIter = dm_cp_nodelist.begin();
	     CPNodeListIter != dm_cp_nodelist.end(); CPNodeListIter++) {
            counter++;
            if ((*CPNodeListIter) == dm_fe_node)
                snprintf(tmp, BUFSIZE, "%s:%d", (*CPNodeListIter).c_str(), i + 1);
            else
                snprintf(tmp, BUFSIZE, "%s:%d", (*CPNodeListIter).c_str(), i);
            treeList.push_back(tmp);
        }
    }

    /* Create the topology file */
    //snprintf(topologyFileName, BUFSIZE, "%s/%s.top", outDir_, filePrefix_);
    file = fopen(dm_topology_filename.c_str(), "w");
    if (file == NULL) {
        perror("fopen failed to create topology file");
        return;
    }
   
    /* Initialized vector iterators */
    parentIter = 0;
    childIter = 1;
    if (topology == NULL) { /* Flat topology */
        fprintf(file, "%s;\n", treeList[0].c_str());
    } else {
        /* Create the topology file from specification */
        topoIter = topology;
        parentIter = 0;
        parentCount = 1;
        childIter = 1;

        /* Parse the specification and create the topology */
        while(true) {
            dashPos = topoIter.find_first_of("-");
            if (dashPos == std::string::npos)
                lastPos = topoIter.length();
            else
                lastPos = dashPos;    
            current = topoIter.substr(0, lastPos);
            layer = atoi(current.c_str());

            /* Loop around the parent's for this layer */
            for (i = 0; i < parentCount; i++) {
                if (parentIter >= treeList.size())
                {
                    return ;
                }
                fprintf(file, "%s =>", treeList[parentIter].c_str());

                /* Add the children for this layer */
                for (j = 0; j < (layer / parentCount) + (layer % parentCount > i ? 1 : 0); j++) {
                    if (childIter >= treeList.size()) {
                        return ;
                    }
                    fprintf(file, "\n\t%s", treeList[childIter].c_str());
                    childIter++;
                }
                fprintf(file, ";\n\n");
                parentIter++;
            }

            parentCount = layer;
            if (lastPos >= topoIter.length())
                break;
            topoIter = topoIter.substr(lastPos + 1);
        }
    }
    
    fclose(file);
    if (topology != NULL)
        free(topology);
    return ;
}
