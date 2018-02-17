/*                               -*- Mode: C -*- 
 * list.c --- Truong Nguyen and singly linked list of generic elements
 * Author          : Marcel Turcotte
 * Created On      : Fri Jul  8 09:17:50 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Wed Jul 13 15:46:21 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "libdev.h"
#include "list.h"

/*****************************************************************
 * dev_new_list - allocates and initialises a new list           *
 *****************************************************************/

list_t *
dev_new_list()
{
  list_t *l;

  l = ( list_t * ) dev_malloc( sizeof( list_t ) );

  l->head = l->tail = NULL;
  l->count = 0;

  return l;
}

/*****************************************************************
 * dev_list_size - returns the size of this list                 *
 * l : input list                                                *
 *****************************************************************/

int
dev_list_size( list_t *l )
{
  return l->count;
}

/*****************************************************************
 * dev_list_is_empty - returns if this list is empty             *
 * l : input list                                                *
 *****************************************************************/

int
dev_list_is_empty( list_t *l )
{
  return l->count == 0;
}

/*****************************************************************
 * dev_list_get - returns the element at the specified index     *
 * l : input list                                                *
 * index : index into this list                                  *
 *****************************************************************/

void *
dev_list_get( list_t *l, int index )
{
  elem_t *p = l->head;
  int pos = 0;

  assert( index >= 0 && index < l->count );
  
  while ( pos++ < index ) {
    p=p->next;
  }

  return p->value;
}

/*****************************************************************
 * dev_list_get_first - returns the first element                *
 * l : input list                                                *
 *****************************************************************/

void *
dev_list_get_first( list_t *l )
{
  assert( l->count > 0 );

  return l->head->value;
}

/*****************************************************************
 * dev_list_get_last - returns the last element                  *
 * l : input list                                                *
 *****************************************************************/

void *
dev_list_get_last( list_t *l )
{
  assert( l->count > 0 );

  return l->tail->value;
}

/*****************************************************************
 * dev_list_set - sets the element at the specified index        *
 * l : input list                                                *
 * index : index into this list                                  *
 * elem : the element to be added at position index              *
 *****************************************************************/

void
dev_list_set( list_t *l, int index, void *elem )
{
  elem_t *p;
  int pos=0;

  assert( index >= 0 && index < l->count );
  
  while ( pos++ < index ) {
    p=p->next;
  }

  p->value = elem;
}

/*****************************************************************
 * new_elem -                                                    *
 * elem : the element to be added                                *
 * next : successor of this elem                                 *
 *****************************************************************/

elem_t *
new_elem( void *e, elem_t *next )
{
  elem_t *elem = ( elem_t * ) dev_malloc( sizeof( elem_t ) );
  elem->value = e;
  elem->next = next;
  return elem;
}

/*****************************************************************
 * dev_list_add - adds an element at the end of this list        *
 * l : input list                                                *
 * elem : the element to be added                                *
 *****************************************************************/

void
dev_list_add( list_t *l, void *elem )
{
  elem_t *new = new_elem( elem, NULL );

  if ( l->head == NULL ) {
    l->head = l->tail = new;
  } else {
    l->tail->next = new;
    l->tail = new;
  }

  l->count++;
}

/*****************************************************************
 * dev_list_insert - adds an element at the specified position   *
 * l : input list                                                *
 * pos : specified position                                      *
 * elem : the element to be added                                *
 *****************************************************************/

void
dev_list_insert( list_t *l, int index, void *elem )
{

  assert( index >= 0 && index <= l->count );

  if ( index == 0 ) {

    l->head = new_elem( elem, l->head );

    if ( l->tail == NULL )
      l->tail = l->head;

  } else {

    elem_t *p = l->head;
    int pos = 0;

    while ( pos++ < ( index-1 ) ) {
      p=p->next;
    }

    p->next = new_elem( elem, p->next );

    if ( p->next->next == NULL )
      l->tail = p->next;
  }

  l->count++;
}

/*****************************************************************
 * dev_list_remove - removes the last element                    *
 * l : input list                                                *
 * removes and returns the last element                          *
 *****************************************************************/

void *
dev_list_remove( list_t *l )
{
  void *elem;

  assert( l->count > 0 );

  elem = l->tail->value;

  if ( l->count == 1 ) {

    dev_free( l->tail );
    l->head = l->tail = NULL;

  } else {

    elem_t *p;

    p = l->head;

    while ( p->next != l->tail )
      p = p->next;

    dev_free( l->tail );
    l->tail = p;
    l->tail->next = NULL;

  }

  l->count--;

  return elem;
}

/*****************************************************************
 * dev_list_serve - removes and returns the first element        *
 * l : input list                                                *
 * removes and returns the first element                         *
 *****************************************************************/

void *
dev_list_serve( list_t *l )
{
  elem_t *tmp;
  void *elem;

  assert( l->count > 0 );

  elem = l->head->value;

  tmp = l->head;

  l->head = l->head->next;

  if ( l->head == NULL )
    l->tail = NULL;

  dev_free( tmp );

  l->count--;

  return elem;
}

/*****************************************************************
 * dev_list_to_array - returns an array containing all the       *
 * elments in this list.                                         *
 * l : input list                                                *
 * returns an array containing all the elements of this list     *
 *****************************************************************/

void **
dev_list_to_array( list_t *l )
{
  elem_t *p;
  int pos = 0;
  void **elems = ( void ** ) dev_malloc( l->count * sizeof( void * ) );

  p = l->head;

  while ( p != NULL ) {
    elems[ pos++ ] = p->value;
    p = p->next;
  }

  return elems;
}

/*****************************************************************
 * dev_free_list - frees memory space                            *
 * l : input list                                                *
 *****************************************************************/

void
dev_free_list( list_t *l, void ( *free_elem )( void * ) )
{
  while ( l->head != NULL ) {
    elem_t *tmp = l->head;
    l->head = l->head->next;
    free_elem( tmp->value );
    dev_free( tmp );
  }

  dev_free( l );
}
