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
 * Implementation of the binary search tree with stacktraces.
 *
 */

#include "bst.h"

CBTF_bst_node*
CBTF_bst_insert_node(CBTF_bst_node *node, uint64_t ID, uint64_t *stacktrace,
		     unsigned stacktrace_size)
{
    if(node == NULL) {
        CBTF_bst_node *temp;
        temp = (CBTF_bst_node *)malloc(sizeof(CBTF_bst_node));
        temp->ID = ID;
        temp->stacktrace_size = stacktrace_size;
        int i;
        for (i = 0; i < stacktrace_size; ++i) {
            temp->stacktrace[i] = stacktrace[i];
        }
        temp->l = temp->r = NULL;
        return temp;
    }

    if(ID >(node->ID)) {
        node->r = CBTF_bst_insert_node(node->r,ID,stacktrace,stacktrace_size);
    } else if(ID < (node->ID)) {
        node->l = CBTF_bst_insert_node(node->l,ID,stacktrace,stacktrace_size);
    }
    return node;
}

CBTF_bst_node*
CBTF_bst_min_node(CBTF_bst_node *node)
{
    if(node == NULL)  return NULL;
    if(node->l) {
	/* search left sub tree to find min element */
	return CBTF_bst_min_node(node->l);
    } else {
	return node;
    }
}

CBTF_bst_node*
CBTF_bst_remove_node(CBTF_bst_node *node, uint64_t ID)
{
    CBTF_bst_node *temp;
    if(node == NULL) {
	return NULL;
    } else if(ID < (node->ID)) {
	node->r = CBTF_bst_remove_node(node->r, ID);
    } else if(ID >(node->ID)) {
	node->l = CBTF_bst_remove_node(node->l, ID);
    } else {
	/* delete this node and replace with either minimum element 
	   in the right sub tree or maximum element in the left subtree.
	*/
	if(node->r && node->l) {
	    /* Replace with minimum element in the right sub tree */
	    temp = CBTF_bst_min_node(node->r);
	    node->ID = temp->ID; 
	    /* replaced with some other node, then delete that node */
	    node->r = CBTF_bst_remove_node(node->r,temp->ID);
	} else {
	    /* If there only one or zero children then remove it from the
	       tree and connect its parent to its child
	    */
	    temp = node;
	    if(node->l == NULL)
		node = node->r;
	    else if(node->r == NULL)
		node = node->l;
	    free(temp); /* temp no longer needed */ 
	}
    }
    return node;
}

CBTF_bst_node*
CBTF_bst_find_node(CBTF_bst_node *node, uint64_t ID)
{
    if(node == NULL) {
        return NULL;
    }

    if(ID > node->ID) {
        /* Search in the right sub tree. */
        return CBTF_bst_find_node(node->r,ID);
    } else if(ID < node->ID) {
        /* Search in the left sub tree. */
        return CBTF_bst_find_node(node->l,ID);
    } else {
        /* Found it */
        return node;
    }
}
