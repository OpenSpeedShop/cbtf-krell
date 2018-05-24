
#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <string>
#include <cxxabi.h>

#include "monitor.h"
#include "overview_connector.h"

static KernelOverviewConnectorInfo* currentKernel;
static std::unordered_map<std::string, KernelOverviewConnectorInfo*> domain_map;
static uint64_t nextKernelID;

extern "C" void kokkosp_init_library(const int loadSeq,
	const uint64_t interfaceVer,
	const uint32_t devInfoCount,
	void* deviceInfo) {

    nextKernelID = 0;
    std::cerr << "[" << getpid() << "," << monitor_get_thread_num() << "] "
    << "KokkosP: Overview Connector sequence:" << loadSeq << " version:" << interfaceVer  << std::endl;

}

extern "C" void kokkosp_finalize_library() {

    std::cerr << "[" << getpid() << "," << monitor_get_thread_num() << "] "
    << "KokkosP: Finalization of Overview Connector. Complete."  << std::endl;

}

extern "C" void kokkosp_begin_parallel_for(const char* name, const uint32_t devID, uint64_t* kID) {
    *kID = nextKernelID++;
    std::string nameStr(name);
    std::cerr << "[" << getpid() << "," << monitor_get_thread_num() << "] "
    << "kokkosp_begin_parallel_for: name:" << nameStr << " kID:" << *kID << " devID:" << devID << std::endl;
}

extern "C" void kokkosp_end_parallel_for(const uint64_t kID) {
    currentKernel = NULL;
    std::cerr << "[" << getpid() << "," << monitor_get_thread_num() << "] "
    << "kokkosp_end_parallel_for:  kID:" << kID << std::endl;
}

extern "C" void kokkosp_begin_parallel_scan(const char* name, const uint32_t devID, uint64_t* kID) {
    *kID = nextKernelID++;
    std::string nameStr(name);
    std::cerr << "[" << getpid() << "," << monitor_get_thread_num() << "] "
    << "kokkosp_begin_parallel_scan: name:" << nameStr << " kID:" << *kID << " devID:" << devID << std::endl;

}

extern "C" void kokkosp_end_parallel_scan(const uint64_t kID) {
    currentKernel = NULL;
    std::cerr << "[" << getpid() << "," << monitor_get_thread_num() << "] "
    << "kokkosp_end_parallel_scan:  kID:" << kID << std::endl;
}

extern "C" void kokkosp_begin_parallel_reduce(const char* name, const uint32_t devID, uint64_t* kID) {
    *kID = nextKernelID++;
    std::string nameStr(name);
    std::cerr << "[" << getpid() << "," << monitor_get_thread_num() << "] "
    << "kokkosp_begin_parallel_reduce: name:" << nameStr << " kID:" << *kID << " devID:" << devID << std::endl;
}

extern "C" void kokkosp_end_parallel_reduce(const uint64_t kID) {
    currentKernel = NULL;
    std::cerr << "[" << getpid() << "," << monitor_get_thread_num() << "] "
    << "kokkosp_end_parallel_reduce:  kID:" << kID << std::endl;
}


// REGIONS
extern "C" void kokkosp_push_profile_region(char* regionName) {
    std::cerr << "[" << getpid() << "," << monitor_get_thread_num() << "] "
    << "kokkosp_push_profile_region: " << regionName << std::endl;
}

extern "C" void kokkosp_pop_profile_region() {
    std::cerr << "[" << getpid() << "," << monitor_get_thread_num() << "] "
    << "kokkosp_pop_profile_region" << std::endl;
}
