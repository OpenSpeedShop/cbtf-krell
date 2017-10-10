////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012-2016 Krell Institute. All Rights Reserved.
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

#if HAVE_CRAYALPS && (defined(RUNTIME_PLATFORM_CRAYXK) || defined(RUNTIME_PLATFORM_CRAYXE) || defined(RUNTIME_PLATFORM_CRAY) || defined(CN_RUNTIME_PLATFORM_CRAY))

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

/* 
Return true if the string (str) starts with the substring (pre),
else return false.
Credit to T.J. Crowder: https://stackoverflow.com/questions/4770985/how-to-check-if-a-string-starts-with-another-string-in-c
*/
static bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

std::string extract_ranges(std::string nodeListNames) {
  boost::char_separator<char> sep(",");
  boost::tokenizer<boost::char_separator<char> > btokens(nodeListNames, sep);
  std::set< int64_t> numericNames;
  std::set< int64_t>:: iterator setit;
  std::string S = "";
  std::string outputList = "";
  int current, count=0, next ;
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

#if defined(HAVE_CRAYALPS) && (defined(RUNTIME_PLATFORM_CRAYXK) || defined(RUNTIME_PLATFORM_CRAYXE) || defined(RUNTIME_PLATFORM_CRAY) || defined(CN_RUNTIME_PLATFORM_CRAY))
    // alps.h defines ALPS_XT_NID to be the file containing the nid.
    // it's /proc/cray_xt/nid for the machines we've seen so far
    std::ifstream ifs( ALPS_XT_NID );
    if( ifs.is_open() ) {
        ifs >> nid;
        ifs.close();
    } else {
        // ALPS_XT_NID is not set but we need to check for the nid value in /proc/cray_xt/nid
        // on Cray's that don't set ALPS_XT_NID but still need to read up the nid value.
        //std::cerr << " LOOKING AT /proc/cray_xt/nid" << std::endl;
        std::fstream myfile("/proc/cray_xt/nid", std::ios_base::in);
        myfile >> nid;
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
    bool leadingZero = false;
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
                    /* Is a leading zero padding required on this platform based     */
                    /* on the node name having a leading zero character in its name? */
                    if (numString[0] == '0') {
                       leadingZero = true;
                    }
                    num2 = atoi(numString);
                    // There is no std::abs for unsigned int on Intel. Static cast num2 to a long long first. 
                    // The unsigned bit should be 32-bit and long long 64-bit. The latter is always big enough 
                    // to hold the former. So you won't loose anything by doing long long calculations. 
                    // Might need to cast back again when you are done.
                    // Was: int tLength = floor(log10(abs(num2)))+1;
		    int tLength = floor(log10(abs(static_cast<long long>(num2))))+1;
                    for (j = num1; j <= num2; j++) {
			if (getIsCray()) {
                            dm_nodelist.push_back(formatCrayNid(baseNodeName,j));
			} else {
			    std::ostringstream ostr;
                            ostr << baseNodeName;

			    // If we determined above that a leading zero character
			    // was found in the character string representing the node name.
			    // Then we need to pad the node names used in the topology file.
                            if (leadingZero) {
                               for (int jl = 0 ; jl < minLength - tLength; jl++) {
                                   // pad with leading 0's as needed
                                   ostr << 0;
                               }
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

void CBTFTopology::processNodeFile(const std::string& nodeFile)
{
	unsigned int total_cpus = 0;
	std::set<std::string> nodenames;
	std::ifstream inFile(nodeFile.data());
	if(inFile.is_open()) {
	    std::string line;
	    while(std::getline(inFile, line))
	    {
		nodenames.insert(line);
		++total_cpus;
	    }
	}

	unsigned int ppn = total_cpus/nodenames.size();
	if (total_cpus%nodenames.size() > 0)
	    ++ppn;
	if (dm_procs_per_node < ppn) {
	    dm_procs_per_node = ppn;
	}
#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
	    std::cerr << "CBTFTopology::processNodeFile  nodefile:" << nodeFile
		<< " dm_procs_per_node:" << dm_procs_per_node
		<< " nodenames size:" << nodenames.size()
		<< " total_cpus:" << total_cpus << std::endl;
	}
#endif

	dm_job_tasks = total_cpus;
	if (nodenames.size() > 0) {
	    dm_num_nodes = nodenames.size();
	    dm_num_app_nodes = nodenames.size();
	}

	std::set<std::string>::const_iterator nodesiter;
	if (getIsCray()) {
            for (nodesiter = nodenames.begin();
	         nodesiter != nodenames.end(); nodesiter++) {
                long t;
                char *ptr;
                // If the nodes already have the nid prefix in the node name (DOD Cray platforms)
                // then skip the formatCrayNid call because it is not required.
                if (startsWith("nid", nodesiter->c_str())) {
                    dm_nodelist.push_back(nodesiter->c_str());
                } else {
                    // If the nodes do NOT already have the nid prefix in the node name (non-DOD Cray platforms)
                    // then call the formatCrayNid call to add the nid prefix.
                    t = strtol(nodesiter->c_str(), &ptr, 10); 
                    dm_nodelist.push_back(formatCrayNid("nid", t));
                } 
	    }
	} else {
            // VERFIY
            for (nodesiter = nodenames.begin();
	         nodesiter != nodenames.end(); nodesiter++) {
                  std::string mynodename = *nodesiter;
                  std::string delim(".");
                  size_t current;
                  size_t next = -1;
                  std::string nodename;
                  /* Loop through node names and only use the hostname */
                  /* On some platforms the PBS supplied node names are host.dns.domain */
                  /* and the front-end node name is only a host name.  This causes problems */
                  /* when using the two differing node name forms.  So, here we reduce */
                  /* the PBS supplied names to match the host only form so we have a */
                  /* successful topology creation.  */
                  do
                  {
                    current = next + 1;
                    next = mynodename.find_first_of( delim, current );
                      nodename = mynodename.substr( current, next - current );
                     // Only want first name from something like localhost.localdomain.
                    if (!nodename.empty())
                        break;
                  } while (next != std::string::npos);
                  dm_nodelist.push_back(nodename);
            } 
	}
}

void CBTFTopology::processEnv()
{
	int desiredDepth;
	// desiredMaxFanout should ultimately be configurable.
	int desiredMaxFanout = 32;

    bool ok_to_process =
	(is_jobid_valid && (is_pbs_valid || is_slurm_valid || is_lsf_valid));
    if (ok_to_process && dm_num_nodes > 0) {
	// always need at least 1 cp!
	long needed_cps = 0;
	if (desiredMaxFanout != 0) {
	    needed_cps = getNumBE() / desiredMaxFanout;
	    if (getNumBE() % desiredMaxFanout > 0 ) ++needed_cps;
	} else {
	    needed_cps = dm_job_tasks / dm_procs_per_node;
	    if (dm_job_tasks % dm_procs_per_node > 0 ) ++needed_cps;
	}
	
	long numcpnodes = needed_cps / dm_procs_per_node;
	if (needed_cps % dm_procs_per_node > 0 )
	     numcpnodes++;

	int num_nodes_for_app;
        int remainder_for_app_node;
	if (isAttachBEMode()) {
	    if (getNumBE() > dm_procs_per_node) {
	        // initialize to enough nodes to handle number of request BEs.
		num_nodes_for_app = getNumBE()/dm_procs_per_node;
                // We must add an additional application node, if there are remaining BEs
                // from the division above.  We determine this by finding the remainder.
                remainder_for_app_node = getNumBE() % dm_procs_per_node;
                if (remainder_for_app_node > 0) {
                   num_nodes_for_app++  ;
                }
	    } else {
	        // initialize to at least one node.
		num_nodes_for_app = 1;
	    }
	} else {
	    // initialize to all nodes in allocation for daemonTools.
	    num_nodes_for_app = dm_num_nodes;
	    setNumBE(dm_num_nodes);
	}

	int procsNeeded = 0;
	int new_fanout = 0;

	// For the AttachBEMode we compute the needed fanout at the level
	// of the leaf CP's. At the leafCP level we expect each leafCP
	// can handle desiredMaxFanout ltwt BE processes. We increase
	// the depth such that the computed new_fanout at the new depth
	// is less than or equal to the desiredMaxFanout.  
	// We use num_nodes_for_app to compute the desired fanout so the
	// leafCP's are equally distributed across the nodes (non-cray case).
	for (desiredDepth = 1; desiredDepth < 1024; desiredDepth++) {
	    new_fanout = (int)ceil(pow((float)num_nodes_for_app, (float)1.0 / (float)desiredDepth));
	    if (new_fanout <= desiredMaxFanout)
		break; 
	}

	// For the AttachBEMode we are expecting each leafCP at the lowest level
	// of the tree to connect to up to desiredMaxFanout ltwt BE's.
	// If a depth 1 topology was computed and the coumputed fanout is
	// less than or equal to desiredMaxFanout then we need a minimum of
	// needed_cps at depth 1.
	// If the computed value of needed_cps (leafCP's) is greater than the
	// desiredMaxFanout we must increase the depth of the tree to 2 and allow
	// for intermediate reduction CPs (ICP).
	int adjust_depth = needed_cps/desiredMaxFanout;
	if (needed_cps%desiredMaxFanout > 0) ++adjust_depth;

	if (desiredDepth == 1) {
	    if (adjust_depth > 1) {
		desiredDepth = 2;
	    } else {
		// should not get here now...
	        new_fanout = needed_cps;
	    }
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
	   std::cerr << "CBTFTopology::processEnv dm_num_nodes " << dm_num_nodes << " dm_procs_per_node " << dm_procs_per_node << std::endl;
	   std::cerr << "CBTFTopology::processEnv needed_cps " << needed_cps << std::endl;
	   std::cerr << "CBTFTopology::processEnv numcpnodes " << numcpnodes << std::endl;
	   std::cerr << "CBTFTopology::processEnv dm_num_app_nodes " << dm_num_app_nodes << std::endl;
	   std::cerr << "CBTFTopology::processEnv fanout " << dm_top_fanout << std::endl;
	   std::cerr << "CBTFTopology::processEnv dm_cp_nodelist size " << dm_cp_nodelist.size() << std::endl;
	   std::cerr << "CBTFTopology::processEnv dm_app_nodelist size " << dm_app_nodelist.size() << std::endl;
        }
#endif
    }
}

void CBTFTopology::parseEnv()
{
    char *ptr, *envval;
    long t;
    bool has_pbs,has_slurm,has_lsf,has_jobid;
    bool has_numnodes,has_numppn,has_np,has_nodelist;

    has_pbs = has_slurm = has_lsf = has_jobid = false;
    has_numnodes = has_numppn = has_np = has_nodelist = false;

    // is there a valid job id? 
    if ((envval = getenv("PBS_JOBID")) != NULL) {
	has_pbs = true;
	has_jobid = true;
    } else if ((envval = getenv("SLURM_JOB_ID")) != NULL) {
	has_slurm = true;
	has_jobid = true;
    } else if ((envval = getenv("LSB_JOBID")) != NULL) {
	has_lsf = true;
	has_jobid = true;
    }

    if (has_jobid) {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_jobid = false;
	} else {
	    dm_jobid = t;
	}
    }

    // Find if num nodes is specfied.
    if ((envval = getenv("PBS_NUM_NODES")) != NULL) {
    	has_numnodes = true;
    } else if ((envval = getenv("BC_NODE_ALLOC")) != NULL) {
    	has_numnodes = true;
    } else if ((envval = getenv("SLURM_JOB_NUM_NODES")) != NULL) {
    	has_numnodes = true;
    } else if ((envval = getenv("SLURM_NNODES")) != NULL) {
	has_numnodes = true;
    } else {
    	has_numnodes = false;
    }

    if (has_numnodes) {
	// TODO: need to parse slurm spec better.  can have values like:
	// 2(x8)
	// 16
	// 16,2(x8),1
	// We will take the first in the specification.  Hopefully
	// the nodes will have homogenous cpu counts.
	if (has_slurm) {
	    std::string strval = envval;
	    int loc = strval.find_first_of("(,");
	    std::string val = strval.substr(0,loc);
	    t = strtol(val.c_str(), &ptr, 10);
	} else {
	    t = strtol(envval, &ptr, 10);
	}

	if (ptr == envval || t < 0) {
	    // problem
	    has_numnodes = false;
	    // default to at least 1 node for localhost.
	    dm_num_nodes = 1;
	    dm_num_app_nodes = 1;
	} else {
	    dm_num_nodes = t;
	    dm_num_app_nodes = t;
	}
    } else {
	// default to at least 1 node for localhost.
	dm_num_nodes = 1;
	dm_num_app_nodes = 1;
    }

    // If dm_num_nodes is 0 (ie. we found no node number in env) then we
    // need to see if a nodelist file was specified.

    // Find if num cores per node is specfied.
    if ((envval = getenv("PBS_NUM_PPN")) != NULL) {
    	has_numppn = true;
    } else if ((envval = getenv("BC_CORES_PER_NODE")) != NULL) {
    	has_numppn = true;
    } else if ((envval = getenv("SLURM_JOB_CPUS_PER_NODE")) != NULL) {
    	has_numppn = true;
    } else {
    	has_numppn = false;
    }

    if (has_numppn) {
	std::string strval = envval;
	int loc = strval.find_first_of("(,");
	std::string val = strval.substr(0,loc);
	t = strtol(val.c_str(), &ptr, 10);
	if (ptr == val || t < 0) {
	    // problem
	    has_numnodes = false;
	    // Default to at least 1 proc.
	    dm_procs_per_node = 1;
	} else {
	    dm_procs_per_node = t;
	}
    } else {
	// Default to at least 1 proc.
	dm_procs_per_node = 1;
    }

    // Find if num tasks is specfied.
    // $PBS_NP is simply a variable that holds the number of cores requested.
    // eg. If requested 2 nodes and 6 cores each, $PBS_NP or $BC_MPI_TASKS_ALLOC 
    // will be 12.
    // SLURM_TASKS_PER_NODE is very misleading.  It really represents the
    // total number of cpus on all the nodes in the allocation.
    if ((envval = getenv("PBS_NP")) != NULL) {
    	has_np = true;
    } else if ((envval = getenv("BC_MPI_TASKS_ALLOC")) != NULL) {
    	has_np = true;
    } else if ((envval = getenv("SLURM_TASKS_PER_NODE")) != NULL) {
    	has_np = true;
    } else {
    	has_np = false;
    }

    if (has_np) {
	t = strtol(envval, &ptr, 10);
	if (ptr == envval || t < 0) {
	    // problem
	    has_np = false;
	} else {
	    dm_job_tasks = t;
	}
    }

    // Is there a PBS nodelist file available?
    if ((envval = getenv("PBS_NODEFILE")) == NULL) {
    	has_nodelist = false;
    } else {
        std::string nodefile(envval);
	processNodeFile(nodefile);
    }

    // Is there a SLURM nodelist available?
    if ((envval = getenv("SLURM_JOB_NODELIST")) == NULL) {
	has_slurm = false;
    	has_nodelist = false;
    } else {
	setNodeList(envval);
    }

    // Is there a LSF nodelist available?
    // This node list is essentially the same as the PBS_NODEFILE
    // So, the processing is the same
    if ((envval = getenv("LSB_DJOB_RANKFILE")) == NULL) {
       has_lsf = false;
       has_nodelist = false;
    } else {
       has_lsf = true;
       std::string nodefile(envval);
       processNodeFile(nodefile);
    }

    is_pbs_valid = has_pbs;
    is_slurm_valid = has_slurm;
    is_lsf_valid = has_lsf;
    is_jobid_valid = has_jobid;
    if (has_slurm) {
      setSlurmValid(true);
      setPBSValid(false);
      setLSFValid(false);
    } else if (has_pbs) {
      setSlurmValid(false);
      setPBSValid(true);
      setLSFValid(false);
    } else if (has_lsf) {
      setSlurmValid(false);
      setPBSValid(false);
      setLSFValid(true);
    } else {
      setSlurmValid(false);
      setPBSValid(false);
      setLSFValid(false);
    }
#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
	   std::string env_type;
	   if (isSlurmValid())
		env_type = "SLURM";
	   else if (isPBSValid())
		env_type = "PBS";
	   else if (isLSFValid())
		env_type = "LSF";
	   else
		env_type = "LOCALHOST";
	   std::cerr << "CBTFTopology::parseEnv " << env_type << ": dm_num_nodes " << dm_num_nodes << " dm_procs_per_node " << dm_procs_per_node << std::endl;
	   std::cerr << "CBTFTopology::parseEnv dm_num_app_nodes " << dm_num_app_nodes << std::endl;
	   std::cerr << "CBTFTopology::parseEnv getFanout " << getFanout() << std::endl;
	   std::cerr << "CBTFTopology::parseEnv getDepth " << getDepth() << std::endl;
	   std::cerr << "CBTFTopology::parseEnv dm_cp_nodelist size " << dm_cp_nodelist.size() << std::endl;
	   std::cerr << "CBTFTopology::parseEnv dm_app_nodelist size " << dm_app_nodelist.size() << std::endl;
        }
#endif
}

// The mode parameter can be accommodate launching to attaching of backends. This
// has implications on the topology file created.  For attach modes, the leafs
// of the tree will be mrnet communication processes that backends with attach to.
// For the start modes, mrnet launches all processes and the leafs of the generated
// topology will represent mrnet backend processes.
// In general, the number of mrnet backends can be inferred from the environment
// but the user can specify them directly. The depth parameter specifies the
// depth of the mrnet tree. It must be at least 2 to represent 1 FE level and 1 BE level
// for a tool that launches backends and at least 1 for a tool that attaches to backends.
// For depths greater than noted above (2 for the start mode case, 1 for attach mode),
// the topology will create levels to host mrnet_commnode processes).
//
void CBTFTopology::autoCreateTopology(const MRNetStartMode& mode, const int& numBE,
				      const unsigned int& fanout, const unsigned int& depth)
{
    // need to handle cray and bluegene separately...
    if (mode == BE_ATTACH) {
	setAttachBEMode(true);
    } else if (mode == BE_CRAY_ATTACH) {
	setAttachBEMode(true);
	setIsCray(true);
	setColocateMRNetProcs(false);
	setFanout(0);
    } else if (mode == BE_START) {
	setAttachBEMode(false);
	setFanout(fanout);
	setDepth(depth);
    } else if (mode == BE_CRAY_START) {
	setAttachBEMode(false);
	setFanout(fanout);
	setDepth(depth);
	setIsCray(true);
    }
    setNumBE(numBE);

#ifndef NDEBUG
    if(CBTFTopology::is_debug_topology_enabled) {
	std::cerr << "CBTFTopology::autoCreateTopology ENTERED"
	<< " mode:" << mode
	<< " numBE:" << numBE
	<< " fanout:" << fanout
	<< " depth:" << depth
	<< std::endl;
    }
#endif

    std::string fehostname;

    if (getIsCray()) {
	// On a cray, the node names are always "nid".
	// The method getCrayFENid() should get the correct nid
	// based on the /proc/cray_xt/nid contents.
#if defined(HAVE_CRAYALPS)
	fehostname = formatCrayNid("nid",getCrayFENid());
#else
	fehostname = getLocalHostName();
#endif
    } else {
	fehostname = getLocalHostName();
    }
    setFENodeStr(fehostname);


    parseEnv();
    processEnv();
    if (isAttachBEMode()) {
	if (getDepth() < 1) {
		setDepth(1);
	}
    } else {
	if (getDepth() < 2) {
		setDepth(2);
	}
    }
    
    // Better not be both slurm and pbs in env! (unlikely though it may be).
    if (isSlurmValid()) {
	std::cout << "Creating topology file for SLURM frontend node " << fehostname << std::endl;
    } else if (isPBSValid()) {
	std::cout << "Creating topology file for PBS frontend node " << fehostname << std::endl;
    } else if (isLSFValid()) {
	std::cout << "Creating topology file for LSF frontend node " << fehostname << std::endl;
    } else {
	// default to the localhost simple toplogy.
	std::cout << "Creating topology file for frontend host " << fehostname << std::endl;
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
    unsigned int i, j;
    int layer;
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
    unsigned int desiredDepth = getDepth();
    unsigned int desiredMaxFanout = getFanout();
    unsigned int procsNeeded = 0;

#ifndef NDEBUG
    if(CBTFTopology::is_debug_topology_enabled) {
	std::cerr << "CBTFTopology::createTopology PASSED topologyspec:" << topologyspec
	<< " desiredDepth from getDepth():" << getDepth()
	<< " desiredMaxFanout from getFanout():" << getFanout()
	<< std::endl;
    }
#endif

    // FE will launch BE's. i.e. daemon type tools.
    // TODO: This is always forcing daemon tools to be at least
    // a depth of 3 and if desiredMaxFanout was not computed anywhere
    // earlier to a value greater than 0 to be the default of
    // 32.  This fanout default should be configureable for user.
    if (!isAttachBEMode()) {
	// always at least 2 for a daemonTool.  needs to ba able
	// to increase if need be...
	if (desiredDepth < 2) {
	    desiredDepth = 2;
	}
	dm_num_app_nodes = dm_cp_nodelist.size();
	if (desiredMaxFanout == 0) {
	    desiredMaxFanout = 32;
	}
    }

    // Set topology format and compute depth and fanout.
    // NOTE: parseEnv and processEnv really should have computed
    // the values for desiredDepth and desiredMaxFanout and made
    // them available to the topology class via getDepth and getFanout.
    // So we should trust them.
    if (topologyspec.empty()) {
        unsigned int fanout = 0;
        if (desiredDepth == 0) {
            // Compute desired depth based on the fanout and number of app nodes.
            for (desiredDepth = 1; desiredDepth < 1024; desiredDepth++) {
                fanout = (int)ceil(pow((float)dm_num_app_nodes, (float)1.0 / (float)desiredDepth));
                if (fanout <= desiredMaxFanout)
                    break;
            }
        } else {
	    fanout = desiredMaxFanout;
	}

	// For daemon type tools where the clients start the mrnet backends
	// we need one more level of depth to specify the backends.
	if (!isAttachBEMode()) {
	    ++desiredDepth;
        }

#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
	    std::cerr << "CBTFTopology::createTopology: computed desiredDepth:" << desiredDepth
		<< " dm_top_depth:" << getDepth()
		<< " dm_num_app_nodes:" << dm_num_app_nodes
		<< " dm_cp_nodelist.size:" << dm_cp_nodelist.size()
		<< " computed fanout:" << fanout
		<< " desiredMaxfanout:" << desiredMaxFanout
		<< " dm_top_fanout:" << getFanout() << std::endl;
	}
#endif

        // Determine the number of mrnet processes needed
        // For a dameonTool, the last spec value should be the number of nodes
        // where a BE will run.  ie all the nodes in the partition at this time...
        std::ostringstream ostr;
	if (isAttachBEMode()) {
#ifndef NDEBUG
	  if(CBTFTopology::is_debug_topology_enabled) {
	    std::cerr << "CBTFTopology::createTopology isAttachBEMode: desiredDepth:" << desiredDepth
	    << " fanout:" << fanout << std::endl;
	  }
#endif
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
	  // for deamonTools
#ifndef NDEBUG
	  if(CBTFTopology::is_debug_topology_enabled) {
	    std::cerr << "CBTFTopology::createTopology isSTARTBEMode: desiredDepth:" << desiredDepth
	    << " fanout:" << fanout << std::endl;
	  }
#endif
	  std::vector<int> nodecount;
	  int numnodes = dm_num_app_nodes;
          for (i = 1; i <= desiredDepth; i++) {
            if (i == 1) {
		int val = numnodes/fanout;
		if (numnodes%fanout > 0 ) ++val;
                procsNeeded += val;
		nodecount.push_back(val);
		//std::cerr << "level:" << i << " " << ostr.str() << "nodes val:" << val << std::endl;
	    } else if ( i == desiredDepth) {
		nodecount.push_back(getNumBE());
                procsNeeded += getNumBE();
		//std::cerr << "level:" << i << " " << ostr.str() << "nodes val:" << numnodes << std::endl;
            } else {
		int val = numnodes/fanout;
		if (numnodes%fanout > 0 ) ++val;
                procsNeeded += val;
		nodecount.push_back(val);
		//std::cerr << "level:" << i << " " << ostr.str() << "nodes val:" << val << std::endl;
            }
          }
	  //std::cerr << "procesNeeded:" << procsNeeded << std::endl;
	  for (std::vector<int>::iterator k = nodecount.begin(); k != nodecount.end() ; ++k) {
	     //std::cerr << " level: " << *k << std::endl;
	     if (k == nodecount.begin()) {
		ostr << " " << *k;
	     } else if (*k > 0) {
		// if there are no nodes at this level, do not add.
		ostr << "-" << *k;
	     }
	  }
	  topologyspec = ostr.str();
	}

#ifndef NDEBUG
	if(CBTFTopology::is_debug_topology_enabled) {
	    std::cerr << "CBTFTopology::createTopology CREATED topologyspec:" << ostr.str() << std::endl;
	    std::cerr << "num cp nodes:" << dm_cp_nodelist.size()
		<< " cp procs available:" << dm_cp_nodelist.size() * dm_procs_per_node
		<< " cp procsNeeded:" << procsNeeded << std::endl;
	}
#endif

        if (procsNeeded <= dm_cp_nodelist.size() * dm_procs_per_node) {
            //  We have enough CPs, so we can have our desired depth
#ifndef NDEBUG
	    if(CBTFTopology::is_debug_topology_enabled) {
	        std::cerr << "desired depth OK" << std::endl;
	    }
#endif
        } else {
            // There aren't enough CPs, so make a 2-deep tree with as many CPs as we have
            std::ostringstream nstr;
	    nstr << (dm_cp_nodelist.size() * dm_procs_per_node);
	    topologyspec = nstr.str();
#ifndef NDEBUG
	    if(CBTFTopology::is_debug_topology_enabled) {
	        std::cerr << "Not enough CPs for desired depth " << desiredDepth
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
    festr << getFENodeStr().c_str() << "-io:0";
#else
    festr << getFENodeStr().c_str() << ":0";
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

            if ((*CPNodeListIter) == getFENodeStr()) {
                cpstr << (*CPNodeListIter).c_str() << ":" << i+1;
		//std::cerr << "cp on FE NODE " << cpstr.str() << std::endl;
            } else {
                cpstr << (*CPNodeListIter).c_str() << ":" << i;
		//std::cerr << "cp on COMPUTE NODE " << cpstr.str() << std::endl;
	    }
            treeList.push_back(cpstr.str());
	    //std::cerr << "PUSH treelist " << cpstr.str() << std::endl;
        }
	if (counter == procsNeeded) break;
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
        unsigned int parentCount = 1;
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
