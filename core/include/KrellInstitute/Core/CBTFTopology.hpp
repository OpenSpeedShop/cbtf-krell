////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012-2013 Krell Institute. All Rights Reserved.
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

struct PBSEnvInfo
{
    std::string job_id;
    std::string num_nodes;
    std::string node_list;
    std::string max_cpus_per_node;

};

#define CBTF_MAX_FANOUT 64
#define CBTF_DEFAULT_FANOUT 32
#define CBTF_DEFAULT_DEPTH 1

typedef enum {
    BE_ATTACH = 0,
    BE_START,
    BE_CRAY_ATTACH,
    BE_CRAY_START
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

	    // Cray specific.
	    // setup a list of application nodes for aprun.
            std::string getAprunList();

	    void setNodeList(const std::string&);

	    std::list<std::string> getNodeList() {
		return dm_nodelist;
	    };

	    void setCPNodeList(const std::list<std::string>& l) {
		dm_cp_nodelist = l;
	    };


	    // FIXME: used by daemonTool.  numBE 0 here is or should
	    // default to one BE daemon per node.  The depth 0 should
	    // mean let the topology creation code determine the value.
	    void autoCreateTopology(const MRNetStartMode& mode) {
		autoCreateTopology(mode,0,CBTF_DEFAULT_FANOUT,0);	
	    };

	    // Used by osscollect and collectionTool.
	    void autoCreateTopology(const MRNetStartMode& mode, const int& numBE) {
		autoCreateTopology(mode,numBE,CBTF_DEFAULT_FANOUT,CBTF_DEFAULT_DEPTH);	
	    };

	    // Actual imlementation of topology creation.
	    void autoCreateTopology(const MRNetStartMode& mode, const int& numBE,
				    const unsigned int& fanout, const unsigned int& depth);

	    void createTopologyFromSpec(const MRNetStartMode&, const std::string&);

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

            std::string unFormatCrayNid(const std::string& nidstr) {
                std::string out = nidstr;
                out.erase(0, out.find_first_not_of("nid"));
                out.erase(0, out.find_first_not_of("0"));
                return out;
            }; 

            std::string createCSVstring(std::list<std::string> &);
            std::string createRangeCSVstring(std::list<std::string> &);


	    void parseEnv();
	    void parseSlurmEnv();
	    void parsePBSEnv();

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
		size_t current = 0;
		size_t next = 0;
		std::string nodename;
		do
		{
		    next = tname.find_first_of( delim, current );
		    nodename = tname.substr( current, next - current );
		    current = next + 1;
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

	    void setNumCPProcs(const unsigned int& val) {
		dm_cp_max_procs = val;
	    };

	    unsigned int  getNumCPProcs() {
		return dm_cp_max_procs;
	    };

	    void setNumBEProcs(const unsigned int& val) {
		dm_be_max_procs = val;
	    };

	    unsigned int  getNumBEProcs() {
		return dm_be_max_procs;
	    };

	    void setMaxProcs(const unsigned int& val) {
		dm_max_procs = val;
	    };

	    unsigned int  getMaxProcs() {
		return dm_max_procs;
	    };

	    void setNumProcsPerNode(const unsigned int& val) {
		dm_procs_per_node = val;
	    };

	    unsigned int  getNumProcsPerNode() {
		return dm_procs_per_node;
	    };

	    void setNumAppNodes(const unsigned int& val) {
		dm_num_app_nodes = val;
	    };

	    unsigned int  getNumAppNodes() {
		return dm_num_app_nodes;
	    };

	    void setDepth(const unsigned int& val) {
		dm_top_depth = val;
	    };

	    unsigned int  getDepth() {
		return dm_top_depth;
	    };

	    void setFanout(const unsigned int& val) {
		dm_top_fanout = val;
	    };

	    unsigned int  getFanout() {
		return dm_top_fanout;
	    };

	    void setNumCPNodes(const unsigned int& val) {
		dm_num_cp_nodes = val;
	    };

	    unsigned int  getNumCPNodes() {
		return dm_num_cp_nodes;
	    };

	    void setSlurmValid( const bool& val) {
		is_slurm_valid = val;
	    };
	    bool isSlurmValid() {
		return is_slurm_valid;
	    };

	    void setPBSValid( const bool& val) {
		is_pbs_valid = val;
	    };
	    bool isPBSValid() {
		return is_pbs_valid;
	    };

	    void setLSFValid( const bool& val) {
		is_lsf_valid = val;
	    };
	    bool isLSFValid() {
		return is_lsf_valid;
	    };

	    void setAttachBEMode(const bool& val) {
		attach_be_mode = val;
	    };

	    bool isAttachBEMode() {
		return attach_be_mode;
	    };

	    void setNumBE(const unsigned int& val) {
		dm_top_numbe = val;
	    };

	    unsigned int getNumBE() {
		return dm_top_numbe;
	    };

	    void setColocateMRNetProcs(const bool& val) {
		dm_colocate_mrnet_procs = val;
	    };

	    bool colocateMRNetProcs() {
		return dm_colocate_mrnet_procs;
	    };

	    std::list<std::string> getAppNodeList() {
		return dm_app_nodelist;
	    };

	    void processNodeFile(const std::string& nodeFile);
	    void processEnv();

	    typedef enum {
		BALANCED,
		KNOMIAL,
		GENERIC
	    } TreeType;

#ifndef NDEBUG
	    static bool is_debug_topology_enabled;
	    static bool is_debug_topology_details_enabled;
#endif
	private:
	    std::set<std::string> * dm_nodes;
	    std::string dm_topology_filename;
	    std::string dm_topology_spec;
	    std::string dm_topology;
	    std::string dm_fe_node;
	    MRN::Tree * dm_tree;
	    unsigned int dm_max_procs, dm_app_procs, dm_be_max_procs, dm_cp_max_procs, dm_procs_per_node;
	    bool is_pbs_valid, is_slurm_valid, is_lsf_valid, is_jobid_valid, dm_is_cray;
	    bool attach_be_mode, dm_colocate_mrnet_procs;
	    long dm_jobid, dm_num_nodes, dm_job_tasks;

	public:
	    unsigned int dm_top_depth, dm_top_fanout, dm_top_numbe;
	    unsigned int dm_num_app_nodes, dm_num_cp_nodes;
	    std::list<std::string> dm_cp_nodelist;
 	    std::list<std::string> dm_app_nodelist;
 	    std::list<std::string> dm_nodelist;
 
};
 
} }
#endif

