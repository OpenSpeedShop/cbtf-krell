/*******************************************************************************
** Copyright (c) 2014 The Krell Institue. All Rights Reserved.
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
 * Declarations for Ompt interface
 *
 */

#ifndef _CBTF_Runtime_Ompt_
#define _CBTF_Runtime_Ompt_

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    CBTF_OMPT_parallel_region_begin,
    CBTF_OMPT_parallel_region_end,
    CBTF_OMPT_task_begin,
    CBTF_OMPT_task_end,
    CBTF_OMPT_thread_begin,
    CBTF_OMPT_thread_end,
    CBTF_OMPT_barrier_begin,
    CBTF_OMPT_barrier_end,
    CBTF_OMPT_wait_barrier_begin,
    CBTF_OMPT_wait_barrier_end,
    CBTF_OMPT_implicit_task_begin,
    CBTF_OMPT_implicit_task_end,
    CBTF_OMPT_master_begin,
    CBTF_OMPT_master_end,
    CBTF_OMPT_taskwait_begin,
    CBTF_OMPT_taskwait_end,
    CBTF_OMPT_wait_taskwait_begin,
    CBTF_OMPT_wait_taskwait_end,
    CBTF_OMPT_taskgroup_begin,
    CBTF_OMPT_taskgroup_end,
    CBTF_OMPT_wait_taskgroup_begin,
    CBTF_OMPT_wait_taskgroup_end,
    CBTF_OMPT_single_others_begin,
    CBTF_OMPT_single_others_end,
    CBTF_OMPT_initial_task_begin,
    CBTF_OMPT_initial_task_end,
    CBTF_OMPT_loop_begin,
    CBTF_OMPT_loop_end,
    CBTF_OMPT_single_in_block_begin,
    CBTF_OMPT_single_in_block_end,
    CBTF_OMPT_sections_begin,
    CBTF_OMPT_sections_end,
    CBTF_OMPT_workshare_begin,
    CBTF_OMPT_workshare_end,
    CBTF_OMPT_idle_begin,
    CBTF_OMPT_idle_end,
    CBTF_OMPT_control,
    CBTF_OMPT_runtime_shutdown,
    CBTF_OMPT_wait_atomic,
    CBTF_OMPT_wait_ordered,
    CBTF_OMPT_wait_critical,
    CBTF_OMPT_wait_lock,
    CBTF_OMPT_release_atomic,
    CBTF_OMPT_release_ordered,
    CBTF_OMPT_release_critical,
    CBTF_OMPT_release_lock,
    CBTF_Ompt_Default_event
} CBTF_Ompt_Event_Type;



#ifdef  __cplusplus
}
#endif
#endif /*_CBTF_Runtime_Ompt_*/
