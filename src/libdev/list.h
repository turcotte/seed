/*                               -*- Mode: C -*- 
 * list.h --- Truong Nguyen and singly linked list of generic elements
 * Author          : Marcel Turcotte
 * Created On      : Fri Jul  8 09:55:41 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:46:27 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#ifndef LIST_H
#define LIST_H

/*****************************************************************
 * node_t -                                                      *
 *****************************************************************/

typedef struct elem_s {
  void *value;
  struct elem_s *next;
} elem_t;

/*****************************************************************
 * list_t - singly linked list                                   *
 *****************************************************************/

typedef struct {
  elem_t *head;
  elem_t *tail;
  int count;
} list_t;

/*****************************************************************
 * Interface (exported)                                          *
 *****************************************************************/

extern list_t *dev_new_list();

extern int dev_list_size( list_t *l );

extern int dev_list_is_empty( list_t *l );

extern void *dev_list_get( list_t *l, int index );

extern void *dev_list_get_first( list_t *l );

extern void *dev_list_get_last( list_t *l );

extern void dev_list_set( list_t *l, int index, void *elem );

extern void dev_list_add( list_t *l, void *elem );

extern void *dev_list_remove( list_t *l );

extern void *dev_list_serve( list_t *l );

extern void **dev_list_to_array( list_t *l );

extern void dev_free_list( list_t *l, void ( *free_elem )( void * ) );

#endif
