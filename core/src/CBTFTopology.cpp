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

#if 0
#include <alps/alps.h>
#endif

#include <mrnet/MRNet.h>
#include <mrnet/Tree.h>
#include "KrellInstitute/Core/CBTFTopology.hpp"

using namespace KrellInstitute::Core;

CBTFTopology::CBTFTopology()
{
    dm_top_depth = 0;
    dm_top_fanout = 64;
    dm_topology_spec = "";
    dm_topology_filename = "./cbtfAutoTopology";
}

CBTFTopology::~CBTFTopology()
{
}



int CBTFTopology::getCrayFENid( void )
{
    int nid = -1;

#if 0
    // alps.h defines ALPS_XT_NID to be the file containing the nid.
    // it's /proc/cray_xt/nid for the machines we've seen so far
    std::ifstream ifs( ALPS_XT_NID );
    if( ifs.is_open() ) {
        ifs >> nid;
        ifs.close();
    }
#else
    nid = 100;
#endif
    return nid;
}

// based on cray example from mrnet src.
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

// based on cray example from mrnet src.
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

// based on cray example from mrnet src.
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


// set a list of nodes.  Can handle a nodeList string that
// is in slurm format. Based on STAT example.
void CBTFTopology::setNodeList(const std::string& nodeList)
{
    char numString[BUFSIZE];
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
            dm_nodelist.push_back(nodes);
        } else {
            /* This is a list of nodes */
            /* Parse the node list string string e.g.: xxx[0-15,12,23,26-35] */
            char *nodeRange = strdup(nodes.substr(openBracketPos + 1, closeBracketPos -
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
			if (getIsCray()) {
                            dm_nodelist.push_back(formatCrayNid(baseNodeName,j));
			} else {
			    std::ostringstream ostr;
                            ostr << baseNodeName << j;
                            dm_nodelist.push_back(ostr.str());
			}
                    }
                } else {
                    num1 = atoi(numString);
                    if (i < strlen(nodeRange)) {
                        if (nodeRange[i] == '-') {
                            isRange = true;
                            continue;
                        }
                    }

		    if (getIsCray()) {
                        dm_nodelist.push_back(formatCrayNid(baseNodeName,num1));
		    } else {
		        std::ostringstream ostr;
                        ostr << baseNodeName << num1;
                        dm_nodelist.push_back(ostr.str());
		    }
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


// parse and validate a slurm environment for creating a topology.
void CBTFTopology::parseSlurmEnv()
{

    char *ptr, *envval;
    long t;
    bool has_slurm = true;
    envval = getenv("SLURM_JOB_ID");
    if (envval == NULL) {
	has_slurm = false;
    } else {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_slurm = false;
	    ;
	} else {
	    dm_slurm_jobid = t;
	}
    }
    
    envval = getenv("SLURM_JOB_NUM_NODES");
    if (envval == NULL) {
	has_slurm = false;
    } else {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_slurm = false;
	    ;
	} else {
	    dm_slurm_num_nodes = t;
	    dm_num_app_nodes = t;
	}
    }

    envval = getenv("SLURM_JOB_CPUS_PER_NODE");
    // need to parse this one.  can have values like:
    // 2(x8)
    // 16
    // 16,2(x8),1
    if (envval == NULL) {
	has_slurm = false;
    } else {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_slurm = false;
	    ;
	} else {
	    dm_procs_per_node = t;
	}
    }

    envval = getenv("SLURM_JOB_NODELIST");
    if (envval == NULL) {
	has_slurm = false;
    } else {
	std::string listval(envval);
	setNodeList(envval);
    }

    is_slurm_valid = has_slurm;
    
    if (is_slurm_valid && dm_slurm_num_nodes > 1) {
	long maxsize = dm_slurm_num_nodes * dm_procs_per_node ;
	long needed_cps = maxsize / CBTF_MAX_FANOUT;
	// std::cerr << "dm_slurm_num_nodes " << dm_slurm_num_nodes
	//     << " dm_procs_per_node " << dm_procs_per_node << std::endl;
	// std::cerr << "maxsize " << maxsize << " needed_cps "
	//     << needed_cps << std::endl;
	std::list<std::string>::iterator NodeListIter;
	int counter = 0;
        for (NodeListIter = dm_nodelist.begin();
	     NodeListIter != dm_nodelist.end(); NodeListIter++) {
            dm_cp_nodelist.push_back(*NodeListIter);
	    counter++;
	    if (counter == needed_cps) break;
	}
    }
}

void CBTFTopology::autoCreateTopology(const MRNetStartMode& mode)
{
    // need to handle cray and bluegene separately...
    if (mode == BE_ATTACH) {
	setAttachBEMode(true);
    } else if (mode == BE_CRAY_ATTACH) {
	setAttachBEMode(true);
	setIsCray(true);
    }

    std::string fehostname;
    if (getIsCray()) {
	// On a cray, the node names are always "nid".
	// The method getCrayFENid() should get the correct nid
	// based on the /proc/cray_xt/nid contents.
	fehostname = formatCrayNid("nid",getCrayFENid());
    } else {
	fehostname = getLocalHostName();
    }
    setFENodeStr(fehostname);

    parseSlurmEnv();
    if (isSlurmValid()) {
        //std::cerr << "Creating topology for slurm job" << std::endl;
	std::cerr << "Creating topology for slurm FE " << fehostname << std::endl;
        //setFENodeStr("localhost");
    } else {
	// default to the localhost simple toplogy.
	std::cerr << "Creating topology for localhost " << fehostname << std::endl;
	setNodeList(fehostname);
	setCPNodeList(getNodeList());
	setNumCPProcs(1);
	setNumAppNodes(1);
	setDepth(1);
	setFanout(1);
	setNumProcsPerNode(2);
    }

    createTopology();
}

// The basis of this code was inspired by the STAT tool FE.
void CBTFTopology::createTopology()
{
    FILE *file;
    unsigned int i, j, layer;
    unsigned int depth = 0, fanout = 0;
    std::string topoIter, current;
    std::string::size_type dashPos, lastPos;

    // The format of the topologyspec is a string representing the number
    // of mrnet CPs at each level. levels of the tree are separated by -. e.g:
    // 2  (1 level with 2 CPs)
    // 2-4-8 (first level2 CPs, second level 4 CPs, third level 8 CPs).
    std::string topologyspec;
    topologyspec = dm_topology_spec;

    //  dm_top_depth will determine the tree depth.  This is different for a
    //  client that launches BE's directly via mrnet rather than allows
    //  for BE's that attach.  i.e. in the attach case, the topology nodes
    //  below the mrnet FE level are CP nodes.
    //
    //  For BEs that are instrumented into an application, we want at least
    //  a depth of 1.  If the depth is 0, we set it to at least one for
    //  the attach case. For depth greater than 1, for the attach case,
    //  there will be additional levels of CPs in the tree.
    //  A flat 1 to N tree in this case is 1 FE in this case is the FE
    //  communicating directly with 1 level of CPs.
    //
    //  For tool daemon BE's that are launched directly by mrnet, the depth
    //  of the tree must include the nodes for the BEs.  a depth of 1 will
    //  imply no CP's. A flat 1 to N tree in this case is 1 FE communicating
    //  directly with the BEs.
    //
    int desiredDepth = dm_top_depth;
    int desiredMaxFanout = dm_top_fanout;
    int procsNeeded = 0;

    // Set topology format and compute depth and fanout.
    if (topologyspec.empty()) {
        if (desiredDepth == 0) {
            // Compute desired depth based on the fanout and number of app nodes.
            for (desiredDepth = 1; desiredDepth < 1024; desiredDepth++) {
                fanout = (int)ceil(pow((float)dm_num_app_nodes, (float)1.0 / (float)desiredDepth));
                if (fanout <= desiredMaxFanout)
                    break;
            }
        } else {
            fanout = (int)ceil(pow((float)dm_num_app_nodes, (float)1.0 / (float)desiredDepth));
	}

#if 0
	std::cerr << "computed desiredDepth  " << desiredDepth
		<< " dm_num_app_nodes  " << dm_num_app_nodes
		<< " computed fanout  " << fanout << std::endl;
#endif

        // Determine the number of mrnet processes needed
        std::ostringstream ostr;
        for (i = 1; i <= desiredDepth; i++) {
            if (i == 1) {
		ostr << fanout;
            } else {
		int val = (int)ceil(pow((float)fanout, (float)i));
		ostr << "-" << val;
            }
            procsNeeded += (int)ceil(pow((float)fanout, (float)i));
        }

	topologyspec = ostr.str();

        if (procsNeeded <= dm_cp_nodelist.size() * dm_procs_per_node) {
            //  We have enough CPs, so we can have our desired depth
            depth = desiredDepth;
        } else {
            // There aren't enough CPs, so make a 2-deep tree with as many CPs as we have
            std::ostringstream nstr;
	    nstr << (dm_cp_nodelist.size() * dm_procs_per_node);
	    topologyspec = ostr.str();
        }

#if 0
	std::cerr << "computed procsNeeded:" << procsNeeded
	    << "desiredDepth:" << desiredDepth
	    << " topologyspec:" << topologyspec << std::endl;
#endif

    } else {
        topoIter = topologyspec;
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

	// If we needed more procs than topology specified, then default
	// to one level of CPs.
        if (procsNeeded > dm_cp_nodelist.size() * dm_procs_per_node) {
            std::ostringstream nstr;
	    nstr << (dm_cp_nodelist.size() * dm_procs_per_node);
	    topologyspec = nstr.str();
        }
    }

    // FE is the root of the tree
    std::ostringstream festr;
#ifdef BGL
    // On BlueGene systems use the network interface with the IO nodes
    festr << dm_fe_node.c_str() << "-io:0";
#else
    festr << dm_fe_node.c_str() << ":0";
#endif

    std::vector<std::string> treeList;
    treeList.push_back(festr.str());

    // Make sure the dm_procs_per_node is set.
    // We could just force at least 1 here...
    if (dm_procs_per_node <= 0)
    {
        return;
    }

    // Add the nodes and IDs to the list of hosts
    unsigned int counter = 0;
    std::list<std::string>::iterator CPNodeListIter;

    for (i = 0; i < dm_procs_per_node; i++) {
        for (CPNodeListIter = dm_cp_nodelist.begin();
	     CPNodeListIter != dm_cp_nodelist.end(); CPNodeListIter++) {
            counter++;
	    std::ostringstream cpstr;

            if ((*CPNodeListIter) == dm_fe_node) {
                cpstr << (*CPNodeListIter).c_str() << ":" << i+1;
            } else {
                cpstr << (*CPNodeListIter).c_str() << ":" << i;
	    }
            treeList.push_back(cpstr.str());
        }
    }

    // Open the topology file for writing.
    file = fopen(dm_topology_filename.c_str(), "w");
    if (file == NULL) {
        perror("fopen failed to create topology file");
        return;
    }
   
    // Initialized vector iterators
    unsigned int parentIter = 0, childIter = 1;

    if (topologyspec.empty()) {
	// Flat topology with no CP.  Just an FE that can be attached
	// to by a BE (or BEs) later.  i.e. FE -> BE.
        fprintf(file, "%s;\n", treeList[0].c_str());
    } else {
        // Create the topology file from specification
        topoIter = topologyspec;
        parentIter = 0;
        int parentCount = 1;
        childIter = 1;

        // Parse the specification and create the topology
        while(true) {
            dashPos = topoIter.find_first_of("-");
            if (dashPos == std::string::npos)
                lastPos = topoIter.length();
            else
                lastPos = dashPos;    
            current = topoIter.substr(0, lastPos);
            layer = atoi(current.c_str());

            // Loop around the parent's for this layer
            for (i = 0; i < parentCount; i++) {
                if (parentIter >= treeList.size())
                {
                    return ;
                }
                fprintf(file, "%s =>", treeList[parentIter].c_str());

                // Add the children for this layer
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
}
