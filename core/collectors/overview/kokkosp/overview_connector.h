
#ifndef _H_OVERVIEW_KOKKOS_CONNECTOR_INFO
#define _H_OVERVIEW_KOKKOS_CONNECTOR_INFO

#include <stdio.h>
#include <sys/time.h>
#include <cstring>
#include <iostream>

enum KernelExecutionType {
	PARALLEL_FOR = 0,
	PARALLEL_REDUCE = 1,
	PARALLEL_SCAN = 2
};

class KernelOverviewConnectorInfo {
	public:
		KernelOverviewConnectorInfo(std::string kName, KernelExecutionType kernelType) {

			char* domainName = (char*) malloc( sizeof(char*) * (32 + kName.size()) );

			if(kernelType == PARALLEL_FOR) {
				sprintf(domainName, "ParallelFor.%s", kName.c_str());
			} else if(kernelType == PARALLEL_REDUCE) {
				sprintf(domainName, "ParallelReduce.%s", kName.c_str());
			} else if(kernelType == PARALLEL_SCAN) {
				sprintf(domainName, "ParallelScan.%s", kName.c_str());
			} else {
				sprintf(domainName, "Kernel.%s", kName.c_str());
			}

		}

		~KernelOverviewConnectorInfo() {
		}

	private:
};

#endif
