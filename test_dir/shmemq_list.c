//----------------------------------------------------------------------------------
//  File : shmemq_list.c
//
//  Shared memory doubly linked list library. Note that the free list size is statically
//  defined and threading of the free list is done during shared memory initialization.
//
//----------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                               Modification History
//
//  Revision   Date       Who    Why
//
//  New        09-22-10   GJ   tt57105 Rudementally generic shared memory link list
//                             lib to support linked list operations in shared mem.
//  tt62643    07/01/11   GJ   Option parent/child issue link list relationship -
//                             make list list library (shmemq_list) generic and
//                             moving shmemq_list to libos32.
//----------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bool.h"
#include "dlogdefs.h"
#include "fail_status.h"
#include "shmemq_list.h"

//-----------------------------------------------------------------------------
// Get a node from the specified free list. If free list exhausted, a null ptr 
// will be returned. The 1st node in the free list is dequeued from the free list
// and returned to the caller
//-----------------------------------------------------------------------------
node_t * get_free_node( root_t * root )
{
  node_t * node_to_return;

  if ( root->head == NULL )
    {
      return NULL;   // Free list exhausted
    }
  node_to_return = root->head;
  root->head     = node_to_return->next;
  if ( root->head == NULL )
    {
      // free list is now exhausted
      root->tail = NULL;
    }
  else
    {
      root->head->prev = NULL;
    }
  root->capacity--;
  return( node_to_return );
}

//-----------------------------------------------------------------------------
// Add node to a linked list (mapped mktid or free). The node is added to
// the end of the linked list.
//-----------------------------------------------------------------------------
int add_node_to_list(  root_t * root, node_t * node )
{
  node_t * last_node;

  if ( root->head != NULL && root->tail != NULL )
    {
      // add to end of list
      last_node       = root->tail;
      last_node->next = node;
      node->prev      = last_node;
      node->next      = NULL;
      root->tail      = node;
    }
  else if ( root->head == NULL && root->tail == NULL )
    {
      // list currently empty
      root->head = node;
      root->tail = node;
      node->next = NULL;
      node->prev = NULL;
    }
  else
    {
      // corruption! 
      return FAIL_STATUS_PREPROCESSING;
    }
  return FAIL_STATUS_NONE;
}

//-----------------------------------------------------------------------------
// Dequeue a node from an active mapped mktid list and return it to the end 
// of its respective free list
//-----------------------------------------------------------------------------
void remove_node_from_list(  root_t * root, root_t * free_list, node_t * node )
{
  node_t * temp_next_node;
  node_t * temp_prev_node;

  if ( node->next != NULL && node->prev != NULL )
    {
      // node in middle of list 
      temp_next_node       = node->next;
      temp_prev_node       = node->prev;
      temp_prev_node->next = temp_next_node;
      temp_next_node->prev = temp_prev_node;
    }
  else if ( node->next != NULL && node->prev == NULL )
    {
      // node 1st on list
      temp_next_node       = node->next;
      root->head           = temp_next_node;
      temp_next_node->prev = NULL;
    }
  else if ( node->next == NULL && node->prev == NULL )
    {
      // node only one on list
      root->head = NULL;
      root->tail = NULL;
    }
  else // node->next == NULL && node->prev != NULL
    {
      // node last on list
      temp_prev_node       = node->prev;
      temp_prev_node->next = NULL;
      root->tail           = temp_prev_node;
    }

  add_node_to_list( free_list, node );
}

//-----------------------------------------------------------------------------
// Return ptr to 1st node on list but do not remove it. Just for viewing...
//-----------------------------------------------------------------------------
node_t * get_first_node_from_list( root_t * root )
{
  return root->head;
}

//-----------------------------------------------------------------------------
// Return ptr to next node on list for viewing...
//-----------------------------------------------------------------------------
node_t * get_next_node_from_list( node_t * node )
{
  return node->next;
}
