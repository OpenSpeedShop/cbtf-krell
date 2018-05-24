#include "KrellInstitute/Services/Assert.h"
#include "KrellInstitute/Services/Common.h"
#include "KrellInstitute/Services/Time.h"

#include "overviewTLS.h"

#include "monitor.h"
#include <mpi.h>

{{fn func MPI_Init MPI_Init_thread}}{
    CBTF_mpip_event event;
    uint64_t start_time = CBTF_GetTime();
    TLS_start_mpi_event(&event);
    {{callfn}}
    event.time = CBTF_GetTime() - start_time;

    // always call PMPI_Comm_rank since mrnet connect will need it.
    int rank = 0;
    int size;
    PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
    PMPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Record event */
    TLS_record_mpi_event(&event, CBTF_GetAddressOfFunction(P{{func}}));
}{{endfn}}

{{fn func MPI_Finalize}}{
    CBTF_mpip_event event;
    uint64_t start_time = CBTF_GetTime();
    TLS_start_mpi_event(&event);
    {{callfn}}
    event.time = CBTF_GetTime() - start_time;
    /* Record event */
    TLS_record_mpi_event(&event, CBTF_GetAddressOfFunction(P{{func}}));
}{{endfn}}

{{fnall func MPI_Init MPI_Init_thread MPI_Finalize}} {
    CBTF_mpip_event event;
    uint64_t start_time = CBTF_GetTime();
    TLS_start_mpi_event(&event);
    {{callfn}}
    event.time = CBTF_GetTime() - start_time;

    /* Record event */
    TLS_record_mpi_event(&event, CBTF_GetAddressOfFunction(P{{func}}));
}
{{endfnall}}
