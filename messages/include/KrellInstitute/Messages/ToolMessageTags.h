/*******************************************************************************
** Copyright (c) 2007,2008 William Hachfeld. All Rights Reserved.
** Copyright (c) 2011 The Krell Institute. All Rights Reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 59 Temple
** Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

/** @file
 *
 * Specification of the frontend/backend communication message tags.
 *
 */



/**
 * Message tags.
 *
 * Integer tag values, associated with each message, that are used to determine
 * that message's type (and associated data structure).
 *
 * @note    All of the following message tags must have a value larger than
 *          FIRST_APPL_TAG as specified in "MRNet.h" (currently "200"). Since
 *          this file is meant to stand alone, it doesn't include "MRNet.h"
 *          directly but implicitly enforces this criterion instead.
 */

#define CBTF_PROTOCOL_TAG_ATTACH_TO_THREADS                  ((int)1100)
#define CBTF_PROTOCOL_TAG_ATTACHED_TO_THREADS                ((int)1101)
#define CBTF_PROTOCOL_TAG_CHANGE_THREADS_STATE               ((int)1102)
#define CBTF_PROTOCOL_TAG_CREATE_PROCESS                     ((int)1103)
#define CBTF_PROTOCOL_TAG_CREATED_PROCESS                    ((int)1104)
#define CBTF_PROTOCOL_TAG_DETACH_FROM_THREADS                ((int)1105)
#define CBTF_PROTOCOL_TAG_EXECUTE_NOW                        ((int)1106)
#define CBTF_PROTOCOL_TAG_EXECUTE_AT_ENTRY_OR_EXIT           ((int)1107)
#define CBTF_PROTOCOL_TAG_EXECUTE_IN_PLACE_OF                ((int)1108)
#define CBTF_PROTOCOL_TAG_GET_GLOBAL_INTEGER                 ((int)1109)
#define CBTF_PROTOCOL_TAG_GET_GLOBAL_STRING                  ((int)1110)
#define CBTF_PROTOCOL_TAG_GET_MPICH_PROC_TABLE               ((int)1111)
#define CBTF_PROTOCOL_TAG_GLOBAL_INTEGER_VALUE               ((int)1112)
#define CBTF_PROTOCOL_TAG_GLOBAL_JOB_VALUE                   ((int)1113)
#define CBTF_PROTOCOL_TAG_GLOBAL_STRING_VALUE                ((int)1114)
#define CBTF_PROTOCOL_TAG_INSTRUMENTED			     ((int)1115)
#define CBTF_PROTOCOL_TAG_LOADED_LINKED_OBJECT               ((int)1116)
#define CBTF_PROTOCOL_TAG_REPORT_ERROR                       ((int)1117)
#define CBTF_PROTOCOL_TAG_SET_GLOBAL_INTEGER                 ((int)1118)
#define CBTF_PROTOCOL_TAG_STDERR                             ((int)1119)
#define CBTF_PROTOCOL_TAG_STDIN                              ((int)1120)
#define CBTF_PROTOCOL_TAG_STDOUT                             ((int)1121)
#define CBTF_PROTOCOL_TAG_STOP_AT_ENTRY_OR_EXIT              ((int)1122)
#define CBTF_PROTOCOL_TAG_SYMBOL_TABLE                       ((int)1123)
#define CBTF_PROTOCOL_TAG_THREADS_STATE_CHANGED              ((int)1124)
#define CBTF_PROTOCOL_TAG_UNINSTRUMENT                       ((int)1125)
#define CBTF_PROTOCOL_TAG_UNLOADED_LINKED_OBJECT             ((int)1126)
#define CBTF_PROTOCOL_TAG_MPI_STARTUP                        ((int)1127)

#define CBTF_PROTOCOL_TAG_PERFORMANCE_DATA                   ((int)10000)

#define CBTF_PROTOCOL_TAG_PCSAMP_PARAMETERS                  ((int)10010)
#define CBTF_PROTOCOL_TAG_PCSAMP_ARGS                        ((int)10011)
#define CBTF_PROTOCOL_TAG_PCSAMP_DATA                        ((int)10012)

#define CBTF_PROTOCOL_TAG_USERTIME_PARAMETERS                ((int)10013)
#define CBTF_PROTOCOL_TAG_USERTIME_ARGS                      ((int)10014)
#define CBTF_PROTOCOL_TAG_USERTIME_DATA                      ((int)10015)

#define CBTF_PROTOCOL_TAG_IO_PARAMETERS                      ((int)10016)
#define CBTF_PROTOCOL_TAG_IO_ARGS                            ((int)10017)
#define CBTF_PROTOCOL_TAG_IOTRACE_DATA                       ((int)10018)
#define CBTF_PROTOCOL_TAG_IOPROFILE_DATA                     ((int)10019)

#define CBTF_PROTOCOL_TAG_HWC_PARAMETERS                     ((int)10020)
#define CBTF_PROTOCOL_TAG_HWC_ARGS                           ((int)10021)
#define CBTF_PROTOCOL_TAG_HWC_DATA                           ((int)10022)
