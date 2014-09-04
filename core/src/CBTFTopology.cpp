////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012-2014 Krell Institute. All Rights Reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cstring>
#include <string>
#include <map>

#include <iterator>
#include <cstddef>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <stdio.h>
#include <typeinfo>
#include <limits.h>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

#if defined(TARGET_OS_CRAYXK) || defined(TARGET_OS_CRAYXE)
#include <alps/alps.h>
#endif

#include <mrnet/MRNet.h>
#include <mrnet/Tree.h>
#include "KrellInstitute/Core/CBTFTopology.hpp"

using namespace KrellInstitute::Core;

#ifndef NDEBUG
/** Flag indicating if debuging for topology creationis enabled. */
bool CBTFTopology::is_debug_topology_enabled =
    (getenv("CBTF_DEBUG_TOPOLOGY") != NULL);
bool CBTFTopology::is_debug_topology_details_enabled =
    (getenv("CBTF_DEBUG_TOPOLOGY_DETAILS") != NULL);
#endif

CBTFTopology::CBTFTopology()
{
    dm_top_depth = 2;
    dm_top_fanout = 0;
    dm_topology_spec = "";
    dm_topology_filename = "./cbtfAutoTopology";
    dm_colocate_mrnet_procs = true;
    dm_is_cray = false;
    is_slurm_valid = false;
}

CBTFTopology::~CBTFTopology()
{
}

std::string extract_ranges(std::string nodeListNames) {
  boost::char_separator<char> sep(",");
  boost::tokenizer<boost::char_separator<char> > btokens(nodeListNames, sep);
  std::set< int64_t> numericNames;
  std::set< int64_t>:: iterator setit;
  std::string S = "";
  std::string outputList = "";
  int current, count, next ;
  bool first_time = true;

  // Get the numeric values into a set which will be automatically sorted
  for ( boost::tokenizer<boost::char_separator<char> >::iterator it = btokens.begin(); it != btokens.end(); ++it) {
      current = boost::lexical_cast<int>(*it);
      numericNames.insert(current);
  }

  // Print out the numeric values on start-up when debug is turned on
#ifndef NDEBUG
  if(CBTFTopology::is_debug_topology_details_enabled) {
    std::cerr << "CBTFTopology: numericNames.size() " << numericNames.size() << std::endl;
    for (setit=numericNames.begin(); setit != numericNames.end(); setit++) {
       std::cerr << "CBTFTopology: numericNames value=" << *setit << std::endl;
    }
  }
#endif
  
  // Loop through the list of numeric values and create range sets when possible
  for (setit=numericNames.begin(); setit != numericNames.end(); setit++) {

   if (first_time) {

      current = *setit;

      // so bottom of loop reassignment does an effective no op.
      next = current;
      outputList.append(boost::lexical_cast<std::string>(current));
      count = 1;
      first_time = false;

    } else {
  
      next = *setit;
  
      if (next == current+1) {
        ++count;
      } else {
        if (count >= 2) {
          outputList.append("-");
        } else {
          outputList.append(",");
        }
  
        if (count > 1) {
          outputList.append(boost::lexical_cast<std::string>(current));
          outputList.append(",");
        }
        outputList.append(boost::lexical_cast<std::string>(next));
        count = 1;
      }
   } // first time

   current = next;

  } // end foreach

  if (count >= 2) {
    outputList.append("-");
    outputList.append(boost::lexical_cast<std::string>(current));
  } else if (count > 1) {
    outputList.append(",");
    outputList.append(boost::lexical_cast<std::string>(current));
  }

  return outputList;

}

std::string CBTFTopology::createCSVstring(std::list<std::string>& list )
{
    std::list<std::string>::iterator ListIter;
    std::string outString;
    for (ListIter = list.begin(); ListIter != list.end(); ListIter++) {
        if (ListIter != list.begin()) outString += ",";
        if (getIsCray()) {
            outString += unFormatCrayNid(*ListIter);
        } else {
            outString += *ListIter;
        }
    }
    //std::cerr << " outString=" << outString << std::endl;
    return outString;
}


std::string CBTFTopology::createRangeCSVstring(std::list<std::string>& list )
{
    std::list<std::string>::iterator ListIter;
    std::string outString;
    for (ListIter = list.begin(); ListIter != list.end(); ListIter++) {
        if (ListIter != list.begin()) outString += ",";
        if (getIsCray()) {
            outString += unFormatCrayNid(*ListIter);
        } else {
            outString += *ListIter;
        }
    }
    //std::cerr << " outString=" << outString << std::endl;
    std::string rangeVersion = extract_ranges(outString);
    //std::cerr << " rangeVersion=" << rangeVersion << std::endl;
    return rangeVersion ;
}

int CBTFTopology::getCrayFENid( void )
{
    int nid = -1;

#if defined(TARGET_OS_CRAYXK) || defined(TARGET_OS_CRAYXE)
    // alps.h defines ALPS_XT_NID to be the file containing the nid.
    // it's /proc/cray_xt/nid for the machines we've seen so far
    std::ifstream ifs( ALPS_XT_NID );
    if( ifs.is_open() ) {
        ifs >> nid;
        ifs.close();
    }
#else
    // just for testing on a non-cray...
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
// use std::list for nodes.  no need for mynid if this
// can work for more than cray.  maybe no need for fanout...
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
	//std::cerr << "CBTFTopology::BuildFlattenedTopology push a node" << std::endl;
        currLevel->push_back( new TopologyNode( *iter ) );
    }

    bool done = false;
    while( !done )
    {
        // start a new level
        std::vector<TopologyNode*>* oldLevel = currLevel;
        currLevel = new std::vector<TopologyNode*>;

        unsigned int nNodesThisLevel = (oldLevel->size() / fanout);
	//std::cerr << "CBTFTopology::BuildFlattenedTopology nNodesThisLevel " << nNodesThisLevel << std::endl;
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

    //std::cerr << "CBTFTopology::BuildFlattenedTopology FE node " << ostr.str() << std::endl;

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

    //std::cerr << "CBTFTopology::BuildFlattenedTopology topologyStr\n" << topologyStr << std::endl;
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

	    // Determine the minumum string length needed to represent
	    // a node.  used to pad leading 0 characters for these cases:
	    // mu[0001-1594] or mu[0001-2,0010,0100,1000-1002]
	    int minLength = 0, tmp = 0;
            for (i = 0; i < strlen(nodeRange); i++) {
		if (nodeRange[i] == ',' || nodeRange[i] == '-' ) {
		   if (minLength < tmp)
			minLength = tmp;
		   tmp = 0;
		} else {
		   tmp++;
		}
	    }

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
		    int tLength = floor(log10(abs(num2)))+1;
                    for (j = num1; j <= num2; j++) {
			if (getIsCray()) {
                            dm_nodelist.push_back(formatCrayNid(baseNodeName,j));
			} else {
			    std::ostringstream ostr;
                            ostr << baseNodeName;
			    for (int jl = 0 ; jl < minLength - tLength; jl++) {
				// pad with leading 0's as needed
				ostr << 0;
			    }
			    ostr << j;
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
                        ostr << baseNodeName << numString;
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

void CBTFTopology::parsePBSEnv()
{
    char *ptr, *envval;
    long t;
    bool has_pbs = true;
    bool has_pbs_numnodes = true;
    bool has_pbs_numppn = true;
    bool has_pbs_np = true;
    envval = getenv("PBS_JOBID");
    if (envval == NULL) {
	has_pbs = false;
    } else {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_pbs = false;
	} else {
	    dm_pbs_jobid = t;
	}
    }

    envval = getenv("PBS_NUM_NODES");
    if (envval == NULL) {
	has_pbs = false;
	has_pbs_numnodes = false;
    } else {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_pbs = false;
	} else {
	    dm_pbs_num_nodes = t;
	    dm_num_app_nodes = t;
	}
    }

    envval = getenv("PBS_NUM_PPN");
    if (envval == NULL) {
	has_pbs = false;
	has_pbs_numppn = false;
    } else {
	std::string strval = envval;
	int loc = strval.find_first_of("(,");
	std::string val = strval.substr(0,loc);
	t = strtol(val.c_str(), &ptr, 10);
	if (ptr == val || t < 0) {
	    // problem
	    has_pbs = false;
	} else {
	    dm_procs_per_node = t;
	}
    }

    // $PBS_NP is simply a variable that holds the number of cores requested.
    // eg. If requested 2 nodes and 6 cores each, $PBS_NP will be 12.
    envval = getenv("PBS_NP");
    if (envval == NULL) {
	has_pbs = false;
	has_pbs_np = false;
    } else {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_pbs = false;
	} else {
	    dm_pbs_job_tasks = t;
	}
    }

    envval = getenv("PBS_NODEFILE");
    if (envval == NULL) {
	has_pbs = false;
    } else {
	unsigned int total_cpus = 0;
	std::set<std::string> nodenames;
        std::string pbsnodefile(envval);
	std::ifstream inFile(pbsnodefile.data());
	if(inFile.is_open()) {
	    std::string line;
	    while(std::getline(inFile, line))
	    {
		nodenames.insert(line);
		++total_cpus;
	    }
	}

#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
	    std::cerr << "CBTFTOPOLOGY PBS: pbsnodefile:" << pbsnodefile
		<< " nodenames size:" << nodenames.size()
		<< " total_cpus:" << total_cpus << std::endl;
	}
#endif

	if (!has_pbs_np) {
	    dm_pbs_job_tasks = total_cpus;
	}

	if (!has_pbs_numnodes) {
	    dm_pbs_num_nodes = nodenames.size();
	    dm_num_app_nodes = nodenames.size();
	}

	if (!has_pbs_numppn) {
	    dm_procs_per_node = dm_pbs_job_tasks/dm_pbs_num_nodes;
	}

	if (dm_pbs_num_nodes > 0 && dm_pbs_job_tasks > 0 && dm_procs_per_node > 0) {
	    has_pbs = true;
	}

	std::set<std::string>::const_iterator nodesiter;
	if (getIsCray()) {
            for (nodesiter = nodenames.begin();
	         nodesiter != nodenames.end(); nodesiter++) {
                long t;
                char *ptr;
                t = strtol(nodesiter->c_str(), &ptr, 10); 
                dm_nodelist.push_back(formatCrayNid("nid", t));
	    }
	} else {
            // VERFIY
            for (nodesiter = nodenames.begin();
	         nodesiter != nodenames.end(); nodesiter++) {
                  dm_nodelist.push_back(*nodesiter);
	    }
	}
    }


    is_pbs_valid = has_pbs;

    if (is_pbs_valid && dm_pbs_num_nodes > 0) {
	long needed_cps = 0;
	if (dm_top_fanout != 0) {
	    needed_cps = dm_procs_per_node / dm_top_fanout;
	} else {
	    needed_cps = dm_pbs_job_tasks / dm_procs_per_node;
	}
	
	long numcpnodes = needed_cps / dm_procs_per_node;
	if (needed_cps % dm_procs_per_node > 0 )
	     numcpnodes++;

	int num_nodes_for_app;
	if (isAttachBEMode()) {
	    if (getNumBE() > dm_procs_per_node) {
	        // initialize to enough nodes to handle number of request BEs.
		num_nodes_for_app = getNumBE()/dm_procs_per_node;
	    } else {
	        // initialize to at least one node.
		num_nodes_for_app = 1;
	    }
	} else {
	    // initialize to all nodes in allocation for daemonTools.
	    num_nodes_for_app = dm_pbs_num_nodes;
	}

	int desiredDepth,fanout;
	// desiredMaxFanout should ultimately be configurable.
	int desiredMaxFanout = 32;
	int procsNeeded = 0;
	int new_fanout = 0;
	for (desiredDepth = 1; desiredDepth < 1024; desiredDepth++) {
	    new_fanout = (int)ceil(pow((float)num_nodes_for_app, (float)1.0 / (float)desiredDepth));
	    if (new_fanout <= desiredMaxFanout)
		break; 
	}
	
	for (int i = 1; i <= desiredDepth; i++) {
	    procsNeeded += (int)ceil(pow((float)new_fanout, (float)i));
	}

	long real_numcpnodes = procsNeeded/dm_procs_per_node;
	if (procsNeeded%dm_procs_per_node > 0) {
	    ++real_numcpnodes;
	}

	if (!dm_colocate_mrnet_procs) {
	    dm_num_app_nodes -= real_numcpnodes;
	    numcpnodes = real_numcpnodes;
	} else {
	    numcpnodes = dm_num_app_nodes;
	}


	dm_num_app_nodes = num_nodes_for_app;
	setFanout(new_fanout);
	setDepth(desiredDepth);

	std::list<std::string>::iterator NodeListIter;
	int counter = 0;
	bool need_cp_node = true;

	for (NodeListIter = dm_nodelist.begin();
	     NodeListIter != dm_nodelist.end(); NodeListIter++) {

	    if (need_cp_node /*|| !dm_colocate_mrnet_procs*/) {
		//std::cerr << "NEED CP || dm_colocate_mrnet_procs dm_cp_nodelist.push_back " << *NodeListIter << std::endl;
                dm_cp_nodelist.push_back(*NodeListIter);
		if (dm_colocate_mrnet_procs) {
                    dm_app_nodelist.push_back(*NodeListIter);
		}
	    } else {
                dm_app_nodelist.push_back(*NodeListIter);
		//std::cerr << "dm_app_nodelist.push_back " << *NodeListIter << std::endl;
	    }

	    counter++;
	    if (counter == numcpnodes ) {
		need_cp_node = false;
	    }
	    if (counter == numcpnodes + dm_num_app_nodes ) {
		break;
	    }
	}
#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
	   std::cerr << "CBTFTopology::parsePBSEnv dm_pbs_num_nodes " << dm_pbs_num_nodes << " dm_procs_per_node " << dm_procs_per_node << std::endl;
	   std::cerr << "CBTFTopology::parsePBSEnv needed_cps " << needed_cps << std::endl;
	   std::cerr << "CBTFTopology::parsePBSEnv numcpnodes " << numcpnodes << std::endl;
	   std::cerr << "CBTFTopology::parsePBSEnv dm_num_app_nodes " << dm_num_app_nodes << std::endl;
	   std::cerr << "CBTFTopology::parsePBSEnv fanout " << dm_top_fanout << std::endl;
	   std::cerr << "CBTFTopology::parsePBSEnv dm_cp_nodelist size " << dm_cp_nodelist.size() << std::endl;
	   std::cerr << "CBTFTopology::parsePBSEnv dm_app_nodelist size " << dm_app_nodelist.size() << std::endl;
        }
#endif
    }
}

// parse and validate a slurm environment for creating a topology.
void CBTFTopology::parseSlurmEnv()
{

    char *ptr, *envval;
    long t;
    bool has_slurm = true;
    bool has_slurm_num_nodes = true;
    bool has_slurm_job_cpus_node = true;
    bool has_slurm_tasks_node = true;
    envval = getenv("SLURM_JOB_ID");
    if (envval == NULL) {
	has_slurm = false;
    } else {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_slurm = false;
	} else {
	    dm_slurm_jobid = t;
	}
    }
    
    envval = getenv("SLURM_JOB_NUM_NODES");
    if (envval == NULL) {
	has_slurm = has_slurm_num_nodes = false;
    } else {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_slurm = has_slurm_num_nodes = false;
	} else {
	    dm_slurm_num_nodes = t;
	    dm_num_app_nodes = t;
	}
    }

// 
// Would be preferred if the cpu count was consistent for
// all nodes in a partion.  The goal is to determine how many
// CP's we need to handle the ltwt mrnet BE's we connect
// assuming one CP can handle as many as 64 ltwt BE's.
// If for example one node has 16 cpus, we can place 16 CP's
// on that node.

    envval = getenv("SLURM_JOB_CPUS_PER_NODE");
    // need to parse this one.  can have values like:
    // 2(x8)
    // 16
    // 16,2(x8),1
    // We will take the first in the specification.  Hopefully
    // the nodes will have homogenous cpu counts.

    if (envval == NULL) {
	has_slurm = has_slurm_job_cpus_node = false;
    } else {
	std::string strval = envval;
	int loc = strval.find_first_of("(,");
	std::string val = strval.substr(0,loc);
	t = strtol(val.c_str(), &ptr, 10);
	if (ptr == val || t < 0) {
	    // problem
	    has_slurm = has_slurm_job_cpus_node = false;
	} else {
	    dm_procs_per_node = t;
	}
    }

    envval = getenv("SLURM_TASKS_PER_NODE");
    // this is very misleading.  It really represents the
    // total number of cpus on all the nodes in the allocation.
    if (envval == NULL) {
	//has_slurm = false;
	has_slurm_num_nodes = false;
	dm_slurm_job_tasks = 0;
    } else {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_slurm = has_slurm_num_nodes = false;
	} else {
	    dm_slurm_job_tasks = t;
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
 
// PROCESS Slurm env...
//
    if (dm_slurm_job_tasks == 0) {
	dm_slurm_job_tasks = dm_procs_per_node * dm_num_app_nodes;
    }

    if (is_slurm_valid && dm_slurm_num_nodes > 0) {
	long needed_cps = 0;
	if (dm_top_fanout != 0)
	    needed_cps = dm_procs_per_node / dm_top_fanout;
	else
	    needed_cps = dm_slurm_job_tasks / dm_procs_per_node;
	
	long numcpnodes = needed_cps / dm_procs_per_node;
	long numcpnodesX = needed_cps % dm_procs_per_node;
	if (numcpnodesX > 0 )
	     numcpnodes++;

	int num_nodes_for_app;
	if (isAttachBEMode()) {
	    if (getNumBE() > dm_procs_per_node) {
	        // initialize to enough nodes to handle number of request BEs.
		num_nodes_for_app = getNumBE()/dm_procs_per_node;
	    } else {
	        // initialize to at least one node.
		num_nodes_for_app = 1;
	    }
	} else {
	    // initialize to all nodes in allocation for daemonTools.
	    num_nodes_for_app = dm_pbs_num_nodes;
	}

	int desiredDepth,fanout;
	// desiredMaxFanout should ultimately be configurable.
	int desiredMaxFanout = 32;
	int procsNeeded = 0;
	int new_fanout = 0;
	int tmp_num_app_nodes = dm_num_app_nodes - numcpnodes;
        for (desiredDepth = 1; desiredDepth < 1024; desiredDepth++) {
                new_fanout = (int)ceil(pow((float)num_nodes_for_app, (float)1.0 / (float)desiredDepth));
                if (new_fanout <= desiredMaxFanout)
                    break; 
	}

        for (int i = 1; i <= desiredDepth; i++) {
            procsNeeded += (int)ceil(pow((float)new_fanout, (float)i));
	}

#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
          if (procsNeeded <= numcpnodes * dm_procs_per_node) {
	    std::cerr << "SLURM desiredDepth OK " << desiredDepth
		<< " procsNeeded:" << procsNeeded << " numcpnodes:" << numcpnodes << std::endl;
	  }
	}
#endif

	long real_numcpnodes = procsNeeded/dm_procs_per_node;
	if (procsNeeded%dm_procs_per_node > 0) {
	    ++real_numcpnodes;
	}

	if (!dm_colocate_mrnet_procs)
	    dm_num_app_nodes -= numcpnodes;

        if (!dm_colocate_mrnet_procs) {
	    dm_num_app_nodes -= real_numcpnodes;
	    numcpnodes = real_numcpnodes;
	} else {
	    numcpnodes = dm_num_app_nodes;
	}

	dm_num_app_nodes = num_nodes_for_app;
	setFanout(new_fanout);
	setDepth(desiredDepth);

	std::list<std::string>::iterator NodeListIter;
	int counter = 0;
	bool need_cp_node = true;
        for (NodeListIter = dm_nodelist.begin();
	     NodeListIter != dm_nodelist.end(); NodeListIter++) {

	    if (need_cp_node || dm_colocate_mrnet_procs) {
                dm_cp_nodelist.push_back(*NodeListIter);
		//std::cerr << "NEED CP || dm_colocate_mrnet_procs dm_cp_nodelist.push_back " << *NodeListIter << std::endl;
	    } else {
                dm_app_nodelist.push_back(*NodeListIter);
		//std::cerr << "dm_app_nodelist.push_back " << *NodeListIter << std::endl;
	    }

	    counter++;
            if (counter == numcpnodes ) {
                need_cp_node = false;
            }
            if (counter == numcpnodes + dm_num_app_nodes ) {
                break;
            }
	}

#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
 	   std::cerr << "CBTFTopology::parseSlurmEnv dm_slurm_num_nodes " << dm_slurm_num_nodes << " dm_procs_per_node " << dm_procs_per_node << std::endl;
	   std::cerr << "CBTFTopology::parseSlurmEnv needed_cps " << needed_cps << std::endl;
	   std::cerr << "CBTFTopology::parseSlurmEnv numcpnodes " << numcpnodes << std::endl;
	   std::cerr << "CBTFTopology::parseSlurmEnv dm_num_app_nodes " << dm_num_app_nodes << std::endl;
	   std::cerr << "CBTFTopology::parseSlurmEnv getFanout " << getFanout() << std::endl;
	   std::cerr << "CBTFTopology::parseSlurmEnv getDepth " << getDepth() << std::endl;
        }
#endif

    }
}

void CBTFTopology::autoCreateTopology(const MRNetStartMode& mode, const int& numBE)
{
    // need to handle cray and bluegene separately...
    if (mode == BE_ATTACH) {
	setAttachBEMode(true);
    } else if (mode == BE_START) {
	setAttachBEMode(false);
    } else if (mode == BE_CRAY_START) {
	setAttachBEMode(false);
	setIsCray(true);
    } else if (mode == BE_CRAY_ATTACH) {
	setAttachBEMode(true);
	setIsCray(true);
	setColocateMRNetProcs(false);
	setFanout(0);
    }
    setNumBE(numBE);

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


    char *job_envval ;
    if ((job_envval = getenv("SLURM_JOB_ID")) != NULL) {
      parseSlurmEnv();
      setPBSValid(false);
      setSlurmValid(true);
    } else if ((job_envval = getenv("PBS_JOBID")) != NULL) {
      parsePBSEnv();
      setPBSValid(true);
      setSlurmValid(false);
    } else {
      setPBSValid(false);
      setSlurmValid(false);
    }
    
    // Better not be both slurm and pbs in env! (unlikely though it may be).
    if (isSlurmValid()) {
	if (isAttachBEMode()) {
	    if (getDepth() < 1) {
		setDepth(1);
	    }
	} else {
	    setDepth(3);
	}
	std::cerr << "Creating topology file for slurm frontend node " << fehostname << std::endl;

    } else if (isPBSValid()) {
	std::cerr << "Creating topology file for pbs frontend node " << fehostname << std::endl;
	if (isAttachBEMode()) {
	    if (getDepth() < 1) {
	        setDepth(1);
	    }
	} else {
	    setDepth(3);
	}
    } else {
	// default to the localhost simple toplogy.
	std::cerr << "Creating topology file for frontend host " << fehostname << std::endl;
	setNodeList(fehostname);
	setCPNodeList(getNodeList());
	setNumCPProcs(1);
	setNumAppNodes(1);
	setDepth(1);
	setFanout(1);
	setNumProcsPerNode(4);
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
    int desiredDepth = getDepth();
    int desiredMaxFanout = getFanout();
    int procsNeeded = 0;

    // FE will launch BE's. i.e. daemon type tools.
    if (!isAttachBEMode()) {
	// always at least 3 for a daemonTool.  needs to ba able
	// to increase if need be...
	desiredDepth = 3;
	dm_num_app_nodes = dm_cp_nodelist.size();
	if (desiredMaxFanout == 0) {
	    desiredMaxFanout = 32;
	}
    }

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
	    if (isAttachBEMode()) {
                fanout = (int)ceil(pow((float)dm_num_app_nodes, (float)1.0 / (float)desiredDepth));
	    } else {
		fanout = desiredMaxFanout;
	    }
	}

#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
	    std::cerr << "CBTFTopology::createTopology: computed desiredDepth  " << desiredDepth
		<< " dm_top_depth  " << getDepth()
		<< " dm_num_app_nodes  " << dm_num_app_nodes
		<< " dm_cp_nodelist.size  " << dm_cp_nodelist.size()
		<< " computed fanout  " << fanout
		<< " desiredMaxfanout  " << desiredMaxFanout
		<< " dm_top_fanout  " << getFanout() << std::endl;
	}
#endif

        // Determine the number of mrnet processes needed
        // For a dameonTool, the last spec value should be the number of nodes
        // where a BE will run.  ie all the nodes in the partition at this time...
        std::ostringstream ostr;
	if (isAttachBEMode()) {
          for (i = 1; i <= desiredDepth; i++) {
            if (i == 1) {
		ostr << fanout;
		//std::cerr << "level:" << i << " " << ostr.str() << std::endl;
            } else {
		int val = (int)ceil(pow((float)fanout, (float)i));
		ostr << "-" << val;
		//std::cerr << "level:" << i << " " << ostr.str() << std::endl;
            }
            procsNeeded += (int)ceil(pow((float)fanout, (float)i));
          }
	  topologyspec = ostr.str();
	} else {
	  // using dm_num_app_nodes
	  std::vector<int> nodecount;
	  int numnodes = dm_num_app_nodes;
          for (i = desiredDepth; i > 0; --i) {
            if (i == 1) {
		//std::cerr << "level:" << i << " " << ostr.str() << std::endl;
		int val = numnodes/fanout;
		if (numnodes%fanout > 0 ) ++val;
		nodecount.push_back(val);
	    } else if ( i == desiredDepth) {
		nodecount.push_back(numnodes);
		//std::cerr << "level:" << i << " " << ostr.str() << std::endl;
            } else {
		int val = numnodes/fanout;
		if (numnodes%fanout > 0 ) ++val;
		numnodes = val;
		//std::cerr << "level:" << i << " " << ostr.str() << std::endl;
		nodecount.push_back(val);
            }
            procsNeeded += (int)ceil(pow((float)fanout, (float)i));
          }
	  for (std::vector<int>::reverse_iterator k = nodecount.rbegin(); k != nodecount.rend() ; ++k) {
	     //std::cerr << " level: " << *k << std::endl;
	     if (k == nodecount.rbegin()) {
		ostr << " " << *k;
	     } else {
		ostr << "-" << *k;
	     }
	  }
	  topologyspec = ostr.str();
	}

#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
	    std::cerr << "CBTFTopology::createTopology topologyspec " << ostr.str() << std::endl;
	    std::cerr << "num cp nodes:" << dm_cp_nodelist.size()
		<< " cp procs available:" << dm_cp_nodelist.size() * dm_procs_per_node
		<< " cp procsNeeded:" << procsNeeded << std::endl;
	}
#endif

        if (procsNeeded <= dm_cp_nodelist.size() * dm_procs_per_node) {
            //  We have enough CPs, so we can have our desired depth
            depth = desiredDepth;
#ifndef NDEBUG
	    if(CBTFTopology::is_debug_topology_enabled) {
	        std::cerr << "depth OK" << std::endl;
	    }
#endif
        } else {
            // There aren't enough CPs, so make a 2-deep tree with as many CPs as we have
            std::ostringstream nstr;
	    nstr << (dm_cp_nodelist.size() * dm_procs_per_node);
	    topologyspec = nstr.str();
#ifndef NDEBUG
	    if(CBTFTopology::is_debug_topology_enabled) {
	        std::cerr << "Not enough CPs for desired dpeth " << desiredDepth
		<< " topologyspec: " << topologyspec
		<< std::endl;
	    }
#endif
        }

#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
	    std::cerr << "computed CP procsNeeded: " << procsNeeded
	    << " desiredDepth: " << desiredDepth
	    << " topologyspec: " << topologyspec << std::endl;
	}
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
	    //std::cerr << "PUSH treelist " << cpstr.str() << std::endl;
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

#ifndef NDEBUG
    if(CBTFTopology::is_debug_topology_enabled) {
	std::cerr << "using topologyspec " << topologyspec << std::endl;
    }
#endif

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
