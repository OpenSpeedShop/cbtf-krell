/*******************************************************************************
** Copyright (c) 2007-2011 The Krell Institute. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * runtime functions for papi
 *
 */

#include "KrellInstitute/Services/PapiAPI.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <pthread.h>

typedef int oss_boolean;
#undef false
#undef true
#define false 0
#define true 1

/**
 * Method: CBTF_init_papi()
 * 
 * xxx.
 *     
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void CBTF_init_papi()
{
    int rval;

    /* Initialize the PAPI library */

    rval = PAPI_library_init(PAPI_VER_CURRENT);

    if (rval != PAPI_VER_CURRENT && rval > 0) {
	fprintf(stderr,"PAPI library version mismatch!\n");
	char error_str[PAPI_MAX_STR_LEN];
#if (PAPI_VERSION_MAJOR(PAPI_VERSION) >= 5)
	PAPI_perror(error_str);
#else
	PAPI_perror(rval,error_str,PAPI_MAX_STR_LEN);
#endif
	fprintf(stderr,"PAPI_error %d: %s\n",rval,error_str);
	exit(1);
    }

    if (rval < 0) {
	/* test rval to see if something is wrong */
	fprintf(stderr,"PAPI rval < 0 !\n");
	char error_str[PAPI_MAX_STR_LEN];
#if (PAPI_VERSION_MAJOR(PAPI_VERSION) >= 5)
	PAPI_perror(error_str);
#else
	PAPI_perror(rval,error_str,PAPI_MAX_STR_LEN);
#endif
	fprintf(stderr,"PAPI_error %d: %s\n",rval,error_str);
	fprintf(stderr,"SYSTEM error: %s\n", strerror(errno));
	return;
    }

    hw_info = PAPI_get_hardware_info() ;
    if (hw_info == NULL) {
	fprintf(stderr, "PAPI_get_hardware_info failed\n") ;
	fprintf(stderr, "At line %d in file %s\n", __LINE__, __FILE__) ;
    }

#if defined(BUILD_TARGETED)
#if defined(HAVE_TARGET_POSIX_THREADS)

    if (PAPI_thread_init(( unsigned long ( * )( void ) ) ( pthread_self )) != PAPI_OK ) {
	fprintf(stderr, "PAPI_thread_init failed\n") ;
    }
#else
#if defined(HAVE_POSIX_THREADS)
    if (PAPI_thread_init(( unsigned long ( * )( void ) ) ( pthread_self )) != PAPI_OK ) {
	fprintf(stderr, "PAPI_thread_init failed\n") ;
    }
#endif
#endif
#else
#if defined(HAVE_POSIX_THREADS)
    if (PAPI_thread_init(( unsigned long ( * )( void ) ) ( pthread_self )) != PAPI_OK ) {
	fprintf(stderr, "PAPI_thread_init failed\n") ;
    }
#endif
#endif




    /* init papi for multiplexing events */
    if (PAPI_multiplex_init() != PAPI_OK) {
        fprintf(stderr, "PAPI_multiplex_init failed\n") ;
    }
}

#ifdef USE_ALLOC_VALUES
/**
 * Method: allocate_test_space()
 * 
 * xxx.
 *     
 * @param   num_tests - xxx.
 * @param   num_events - xxx.
 *
 * @return  long_long **
 *
 * @todo    Error handling.
 *
 */
long_long **allocate_test_space(int num_tests, int num_events)
{
   long_long **values;
   int i;

   values = (long_long **) malloc(num_tests * sizeof(long_long *));
   if (values == NULL)
      exit(1);
   memset(values, 0x0, num_tests * sizeof(long_long *));

   for (i = 0; i < num_tests; i++) {
      values[i] = (long_long *) malloc(num_events * sizeof(long_long));
      if (values[i] == NULL)
         exit(1);
      memset(values[i], 0x00, num_events * sizeof(long_long));
   }
   return (values);
}
#endif

/**
 * Method: print_hw_info()
 * 
 * Print out the hardward information for.
 * the target platform.
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void print_hw_info()
{

  hw_info = PAPI_get_hardware_info() ;
  if (hw_info == NULL) {
    fprintf(stderr, "PAPI_get_hardware_info failed\n") ;
    fprintf(stderr, "At line %d in file %s\n", __LINE__, __FILE__) ;
  }

  printf("PAPI hardware information\n") ;
  printf("  # of CPUs in SMP node: %d\n", hw_info->ncpu) ;
  printf("  # of SMP nodes:        %d\n", hw_info->nnodes) ;
  printf("  Total CPUs in system:  %d\n", hw_info->totalcpus) ;
  printf("  Vendor:                %d\n", hw_info->vendor) ;
  printf("  Vendor string:         %s\n", hw_info->vendor_string) ;
  printf("  CPU model:             %d\n", hw_info->model) ;
  printf("  CPU model string:      %s\n", hw_info->model_string) ;
  printf("  Revision of CPU:       %9.4f\n", hw_info->revision) ;
  printf("  Cycle time MHz:        %9.4f\n", hw_info->mhz) ;
}

/**
 * Method: get_papi_exe_info()
 * 
 * xxx.
 *     
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void get_papi_exe_info ()
{
    const PAPI_exe_info_t *exeinfo = NULL;


    if ((exeinfo = PAPI_get_executable_info()) == NULL)
      exit(1);

    printf("Path+Program: %s\n",exeinfo->fullname);
    printf("Program: %s\n",exeinfo->address_info.name);
    printf("Text start: %p, Text end: %p\n",
	exeinfo->address_info.text_start,exeinfo->address_info.text_end);
    printf("Data start: %p, Data end: %p\n",
	exeinfo->address_info.data_start,exeinfo->address_info.data_end);
    printf("Bss start: %p, Bss end: %p\n",
	exeinfo->address_info.bss_start,exeinfo->address_info.bss_end);
}

/**
 * Method: print_papi_events()
 * 
 * Scan for all supported native events on this platform.
 *     
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void print_papi_events ()
{

    int i = PAPI_PRESET_MASK;;
    PAPI_event_info_t info;
    printf("Name                  Code      Description\n");
    do
    {
        int retval = PAPI_get_event_info(i, &info);
        if (retval == PAPI_OK) {
            printf("%-30s 0x%-10x %s\n", info.symbol, info.event_code, info.long_descr);
        }
    } while (PAPI_enum_event(&i, PAPI_PRESET_ENUM_AVAIL) == PAPI_OK);
}

/**
 * Method: get_papi_name()
 * 
 * Get the name of an event represented by a
 * numerical code.
 *     
 * @param   code - xxx.
 * @param   ecstr - xxx.
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void get_papi_name (int code, char *ecstr)
{
    char EventCodeStr[PAPI_MAX_STR_LEN];
    if (PAPI_event_code_to_name(code, EventCodeStr) != PAPI_OK)
    {
        fprintf(stderr,"get_papi_name: PAPI_event_code_to_name failed.\n");
    }
    strncpy(ecstr,EventCodeStr,sizeof(EventCodeStr));
}

/**
 * Method: get_papi_eventcode()
 * 
 * Return the numerical code represented by a
 * given event string.
 *     
 * @param   eventname - xxx.
 *
 * @return  int EventCode
 *
 * @todo    Error handling.
 *
 */
int get_papi_eventcode (char* eventname)
{
    int EventCode = PAPI_NULL;
    char* strptr;
    char ename[PAPI_MAX_STR_LEN];
    strcpy(ename,eventname);
    strptr = ename;
    if (PAPI_event_name_to_code(strptr,&EventCode) != PAPI_OK) {
        fprintf(stderr,"get_papi_eventcode: PAPI_event_name_to_code failed!\n");
        return PAPI_NULL;
    }
    return EventCode;
}

/**
 * Method: print_papi_error()
 * 
 * xxx.
 *     
 * @param   errcode - xxx.
 *
 * @return  void
 *
 * @todo    Error handling.
 *
 */
void print_papi_error (int errcode)
{
    fprintf(stderr,"print_papi_error: ENTERED!\n");
}


void CBTF_PAPIerror (int rval, const char *where)
{
	char error_str[PAPI_MAX_STR_LEN];
#if (PAPI_VERSION_MAJOR(PAPI_VERSION) >= 5)
	PAPI_perror(error_str);
#else
	PAPI_perror(rval,error_str,PAPI_MAX_STR_LEN);
#endif
	unsigned long mytid = PAPI_thread_id();
	fprintf(stderr,"CBTF_PAPIerror:%s, %d in %lu: %s\n",where,rval,mytid,error_str);
}

static oss_boolean CBTF_event_exists (int event)
{
	int rval = PAPI_query_event(event);
	if (rval != PAPI_OK) {
	    fprintf(stderr,"The event %#x does not seem to be supported\n",
		event);
	    CBTF_PAPIerror(rval,"CBTF_event_exists");
	    return false;
	}

	PAPI_event_info_t info;

	rval = PAPI_get_event_info(event,&info);
	if (rval != PAPI_OK) {
	    fprintf(stderr,"The event %#x does not seem to be supported\n",
		event);
	    CBTF_PAPIerror(rval,"CBTF_event_exists");
	    return false;
	}

#if 0
	if (info.count > 0) {
	    fprintf (stderr, "%s (%s) is available on this hardware.\n",
		info.symbol ? info.symbol : "",
		info.short_descr ? info.short_descr : "");
	}

	if (info.count > 1) {
	    fprintf (stderr,"%s is a derived event on this hardware.\n",
		info.symbol ? info.symbol : "");
	}
#endif
	return true;
}

void CBTF_Create_Eventset(int *EventSet)
{
    int rval = PAPI_OK;
    if (*EventSet != PAPI_NULL) {
	*EventSet = PAPI_NULL;
    }

    rval = PAPI_create_eventset(EventSet);
    if (rval != PAPI_OK) {
	CBTF_PAPIerror(rval,"CBTF_Create_Eventset");
        return;
    }

}

void CBTF_AddEvent(int EventSet, int event)
{
    CBTF_event_exists(event);

    int rval = PAPI_add_event(EventSet,event);

    if ( rval != PAPI_OK) {
	CBTF_PAPIerror(rval,"CBTF_AddEvent");
        return;
    }
}

void CBTF_Overflow(int EventSet, int event, int threshold , void *hwcPAPIHandler)
{
    if (EventSet == PAPI_NULL) {
        fprintf(stderr,"CBTF_Overflow:EventSet is PAPI_NULL!\n");
    }

    int rval = PAPI_overflow(EventSet,event,threshold, 0, hwcPAPIHandler);

    if (rval != PAPI_OK) {
        fprintf(stderr,"PAPI_overflow() for event %#x FAILED!\n", event);
	CBTF_PAPIerror(rval,"CBTF_Overflow");
    }
}

void CBTF_Start(int EventSet)
{
    int status = 0;
    int rval = 0;

    rval = PAPI_state(EventSet, &status);
    if (rval != PAPI_OK) {
	CBTF_PAPIerror(rval,"CBTF_Start");
    }

    // Do not start PAPI if already running
    if (!(status & PAPI_RUNNING)) {
      rval = PAPI_start(EventSet);
      if (rval != PAPI_OK) {
	CBTF_PAPIerror(rval,"CBTF_Start");
      }
    }
}

/* if PAPI_ENOTRUN then just return. may want to return rval here...*/
void CBTF_Stop(int EventSet, long_long* evalues)
{
    if (EventSet == PAPI_NULL) {
        return;
    }

    int status = 0;
    int rval = 0;

    rval = PAPI_state(EventSet, &status);
    if (rval != PAPI_OK) {
	CBTF_PAPIerror(rval,"CBTF_Stop");
    }

    // Do not stop PAPI if already stopped
    if (! (status & PAPI_STOPPED) ) {
      rval = PAPI_stop(EventSet,evalues);

      if (rval == PAPI_ENOTRUN) {
	CBTF_PAPIerror(rval,"CBTF_Stop");
	return;
      }

      if (rval != PAPI_OK) {
	CBTF_PAPIerror(rval,"CBTF_Stop");
      }
    }
}

/* if PAPI_ENOTRUN then just return. may want to return rval here...*/
void CBTF_HWCAccum(int EventSet, long_long* evalues)
{
    if (EventSet == PAPI_NULL) {
        return;
    }

    int rval = PAPI_accum(EventSet,evalues);

    if (rval == PAPI_ENOTRUN) {
	CBTF_PAPIerror(rval,"CBTF_HWCAccum");
	return;
    }

    if (rval != PAPI_OK) {
	CBTF_PAPIerror(rval,"CBTF_HWCAccum");
    }
}

/* if PAPI_ENOTRUN then just return. may want to return rval here...*/
void CBTF_HWCRead(int EventSet, long_long* evalues)
{
    if (EventSet == PAPI_NULL) {
        return;
    }

    int rval = PAPI_read(EventSet,evalues);

    if (rval == PAPI_ENOTRUN) {
	CBTF_PAPIerror(rval,"CBTF_HWCRead");
	return;
    }

    if (rval != PAPI_OK) {
	CBTF_PAPIerror(rval,"CBTF_HWCRead");
    }
}
