/*******************************************************************************
** Copyright (c) 2016  The Krell Institute. All Rights Reserved.
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
 * Definition of the stacktrace binary search tree (bst).
 *
 **/

#include <inttypes.h>
#include <pthread.h>
#include "collector.h"  /*MaxFramesPerStackTrace*/

/**
 * binary search tree struct describing a parallel region
 * or a task (implicit?).
 * maps a region id to a calling context for a parallel region.
 **/
typedef struct CBTF_bst_node {
  uint64_t ID;
  uint64_t stacktrace[MaxFramesPerStackTrace];
  unsigned stacktrace_size;
  struct CBTF_bst_node *l;
  struct CBTF_bst_node *r;
  pthread_mutex_t mutex;
} CBTF_bst_node;

extern CBTF_bst_node* CBTF_bst_insert_node(CBTF_bst_node *node, uint64_t ID, uint64_t *stacktrace, unsigned stacktrace_size);
extern CBTF_bst_node* CBTF_bst_remove_node(CBTF_bst_node *node, uint64_t ID);
extern CBTF_bst_node* CBTF_bst_find_node(CBTF_bst_node *node, uint64_t ID);
extern CBTF_bst_node* CBTF_bst_min_node(CBTF_bst_node *node);
