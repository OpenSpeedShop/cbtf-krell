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
#include <unistd.h> 
#include <string> 
#include <sys/param.h>
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

struct SlurmEnvInfo
{
    std::string job_id;
    std::string num_nodes;
    std::string node_list;
    std::string max_cpus_per_node;

};

#define CBTF_MAX_FANOUT 64

typedef enum {
    BE_ATTACH = 0,
    BE_START,
    BE_CRAY_ATTACH
} MRNetStartMode;

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

	    void setNodeList(const std::string&);

	    std::list<std::string> getNodeList() {
		return dm_nodelist;
	    };

	    void setCPNodeList(const std::list<std::string>& l) {
		dm_cp_nodelist = l;
	    };


	    void autoCreateTopology(const MRNetStartMode&);
	    void createTopology();

	    int getCrayFENid( void );

	    void setIsCray( const bool& flag) {
		dm_is_cray = flag;
	    }

	    bool getIsCray(void) {
		return dm_is_cray;
	    }

	    std::string formatCrayNid(const std::string& nidstr, const int& nid) {
		// all cray nodes are named beginning with "nid" and then
		// followed by a 5 digit integer.
		std::string craynid("nid");
	        std::ostringstream ostr;
		ostr << craynid << std::setw( 5 ) << std::setfill('0') << nid;
		return ostr.str();
	    };

	    void parseSlurmEnv();

	    void setTopologyStr(const std::string& topology) {
		dm_topology = topology;
	    };

	    std::string  getTopologyStr() {
		return dm_topology;
	    };

	    void setTopologyFileName(const std::string& tname) {
		dm_topology_filename = tname;
	    };

	    std::string  getTopologyFileName() {
		return dm_topology_filename;
	    };

	    void setTopologySpec(const std::string& spec) {
		dm_topology_spec = spec;
	    };

	    std::string  getTopologySpec() {
		return dm_topology_spec;
	    };

	    std::string getLocalHostName1() {
		char lhostname[MAXHOSTNAMELEN];
		gethostname(lhostname, MAXHOSTNAMELEN);
		return lhostname;
	    };

	    std::string getLocalHostName() {
		char lhostname[MAXHOSTNAMELEN];
		gethostname(lhostname, MAXHOSTNAMELEN);
		std::string tname(lhostname);
		std::string delim(".");
		size_t current;
		size_t next = -1;
		std::string nodename;
		do
		{
		    current = next + 1;
		    next = tname.find_first_of( delim, current );
		    nodename = tname.substr( current, next - current );
		    // Only want first name from something like localhost.localdomain.
		    //
		    if (!nodename.empty())
		        break;
		} while (next != std::string::npos);
		return nodename;
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

	    void setNumProcsPerNode(const int& val) {
		dm_procs_per_node = val;
	    };

	    int  getNumProcsPerNode() {
		return dm_procs_per_node;
	    };

	    void setNumAppNodes(const int& val) {
		dm_num_app_nodes = val;
	    };

	    int  getNumAppNodes() {
		return dm_num_app_nodes;
	    };

	    void setDepth(const int& val) {
		dm_top_depth = val;
	    };

	    int  getDepth() {
		return dm_top_depth;
	    };

	    void setFanout(const int& val) {
		dm_top_fanout = val;
	    };

	    int  getFanout() {
		return dm_top_fanout;
	    };

	    void setNumCPNodes(const int& val) {
		dm_num_cp_nodes = val;
	    };

	    int  getNumCPNodes() {
		return dm_num_cp_nodes;
	    };

	    bool isSlurmValid() {
		return is_slurm_valid;
	    };

	    void setAttachBEMode(const bool& val) {
		attach_be_mode = val;
	    };

	    bool isAttachBEMode() {
		return attach_be_mode;
	    };

	    void setColocateMRNetProcs(const bool& val) {
		dm_colocate_mrnet_procs = val;
	    };

	    bool colocateMRNetProcs() {
		return dm_colocate_mrnet_procs;
	    };

	    typedef enum {
		BALANCED,
		KNOMIAL,
		GENERIC
	    } TreeType;

	private:
	    std::set<std::string> * dm_nodes;
	    std::string dm_topology_filename;
	    std::string dm_topology_spec;
	    std::string dm_topology;
	    std::string dm_fe_node;
	    int dm_top_depth, dm_top_fanout;
	    MRN::Tree * dm_tree;
	    int dm_max_procs, dm_app_procs, dm_be_max_procs, dm_cp_max_procs,
		dm_num_app_nodes, dm_num_cp_nodes, dm_procs_per_node;
	    bool is_slurm_valid, dm_is_cray;
	    bool attach_be_mode, dm_colocate_mrnet_procs;
	    long dm_slurm_jobid, dm_slurm_num_nodes, dm_slurm_job_tasks;
	    std::list<std::string> dm_cp_nodelist;
	    std::list<std::string> dm_app_nodelist;
	    std::list<std::string> dm_nodelist;

};

} }
#endif
