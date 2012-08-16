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

#ifndef _KrellInsitute_Core_CBTFTopology_
#define _KrellInsitute_Core_CBTFTopology_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "mrnet/Tree.h"

#define BUFSIZE 1024

namespace KrellInstitute { namespace Core {

struct TopologyNode
{
    std::string hostname;
    unsigned int index;
    std::vector<TopologyNode*> children;

    TopologyNode( std::string h )
      : hostname( h ),
        index( 0 )
    { }

    ~TopologyNode( void )
    {
        for( std::vector<TopologyNode*>::iterator iter = children.begin();
            iter != children.end();
            iter++ )
        {
            delete *iter;
        }
        children.clear();
    }
};

#define CBTF_MAX_FANOUT 64

typedef enum {
    CBTF_TOPOLOGY_DEPTH = 0,
    CBTF_TOPOLOGY_FANOUT,
    CBTF_TOPOLOGY_USER,
    CBTF_TOPOLOGY_AUTO
} CBTFTopologyType;

class CBTFTopology {


	public:
	    CBTFTopology();
	    ~CBTFTopology();

	    void AssignTopologyIndices( TopologyNode*,
                        std::map<std::string, unsigned int>& );
	    void PrintTopology( TopologyNode*, std::ostringstream&);
	    void BuildFlattenedTopology( unsigned int,
                        int , std::set<std::string>& );

	    void setCommNodeList(const char *);

	    void createTopology(char *topologyFileName, CBTFTopologyType topologyType,
                                 char *topologySpecification, char *nodeList);

	    void parseSlurmEnv();

	    void setTopologyStr(const std::string& topology) {
		dm_topology = topology;
	    };

	    std::string  getTopologyStr() {
		return dm_topology;
	    };

	    void setFENodeStr(const std::string& node) {
		dm_fe_node = node;
	    };

	    std::string  getFENodeStr() {
		return dm_fe_node;
	    };

	    void setNumCPProcs(const int& val) {
		dm_cp_max_procs = val;
	    };

	    int  getNumCPProcs() {
		return dm_cp_max_procs;
	    };

	    void setNumBEProcs(const int& val) {
		dm_be_max_procs = val;
	    };

	    int  getNumBEProcs() {
		return dm_be_max_procs;
	    };

	    void setMaxProcs(const int& val) {
		dm_max_procs = val;
	    };

	    int  getMaxProcs() {
		return dm_max_procs;
	    };

	    typedef enum {
		BALANCED,
		KNOMIAL,
		GENERIC
	    } TreeType;

	private:
	    std::set<std::string> * dm_nodes;
	    std::string dm_topology_filename;
	    std::string dm_topology;
	    std::string dm_fe_node;
	    MRN::Tree * dm_tree;
	    int dm_max_procs, dm_app_procs, dm_be_max_procs, dm_cp_max_procs,
		dm_num_app_nodes, dm_num_cp_nodes, dm_procs_per_node;
	    long dm_slurm_jobid;
	    std::list<std::string> dm_cp_nodelist;

};

} }
#endif
