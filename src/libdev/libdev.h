/*                               -*- Mode: C -*- 
 * devtools.h --- Truong Nguyen and defines commonly used macros and types
 * Author          : Marcel Turcotte
 * Created On      : Tue Jun  7 13:10:06 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Tue Aug  9 18:26:07 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#ifndef LIBDEV_H
#define LIBDEV_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE 
#define FALSE 0
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef MIN
#define MIN( a, b )         ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif
#ifndef MAX
#define MAX( a, b )         ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif

#define DEFAULT_DEBUG_LEVEL 0

/* 0 - quiet
 * 1 - general traces
 * 2 - specific messages
 * 3 - memory allocation
 * 4 - display data structures
 */

/*****************************************************************
 * Functional data types - this section defines data types based *
 * on their usage.                                               *
 *                                                               *
 * Nota: for compatibility with the suffix array library (vtree) *
 * the definition of the symbols and indices must be the same.   *
 *****************************************************************/

typedef int symbol_t;
typedef int pos_t;

/*****************************************************************
 * code_t - mapping characters to integer values                 *
 *****************************************************************/

typedef struct {
  char sym; /* character representation */
  symbol_t code; /* binary representation */
} code_t;

/*****************************************************************
 * alphabet_t - allows for degenerated alphabets, i.e. more than *
 * one character can be mapped to the same integer value.  This  *
 * is useful in the case of nucleic acids in order to map T and  *
 * U to the same integer value.                                  *
 *****************************************************************/

typedef struct {
  code_t *codes; /* mapping */
  int length; /* length of the mapping table */
  int size; /* size of the digital alphabet */
} alphabet_t;

/*****************************************************************
 * dstring_t - digital string data type                          *
 *****************************************************************/

typedef struct {
  symbol_t *text;
  int length;
  alphabet_t *alphabet;
} dstring_t;

/*****************************************************************
 * Interface (exported)                                          *
 *****************************************************************/

extern void dev_init();

#ifdef HAS_MALLINFO
extern unsigned long dev_get_memory_usage();
#endif

extern char *dev_asc_memory_usage( char **buf, unsigned long mem );

#define dev_malloc( s ) _dev_malloc( __FILE__, __LINE__, s )
extern void *_dev_malloc( char *src, int line, size_t size );

#define dev_realloc( p, s ) _dev_realloc( __FILE__, __LINE__, p, s )
extern void *_dev_realloc( char *src, int line, void *p, size_t size );

extern void dev_free( void *p );

extern void dev_free_array( void **p, int size );

extern void dev_log( int level, char * format, ... );

#define dev_die( ... ) _dev_die( __FILE__, __LINE__, __VA_ARGS__ )
extern void _dev_die( char *src, int line, char *format, ... );

extern int dev_set_debug_level( int level );

extern int dev_get_debug_level( void );

#define dev_fopen( n, m ) _dev_fopen( __FILE__, __LINE__, n, m )
extern FILE *_dev_fopen( const char *src, int line, const char *name, const char *mode );

extern char *dev_new_filename( char *dir, char *name, char *ext );

extern int dev_isdir( const char *file_name );

extern void dev_trim_dirname( char *name );

extern void dev_mkdir_or_die( const char *path );

extern int dev_parse_int( const char *s );

extern float dev_parse_float( const char *s );

extern char *dev_strcpy( const char *s );

extern symbol_t *dev_symcpy( const symbol_t *s, int length );

extern int dev_isnewline( char c );

extern int dev_write_lines( char *s, int width, FILE *fh );

extern symbol_t dev_encode( alphabet_t *a, char s );

extern char dev_decode( alphabet_t *a, symbol_t c );

extern char *dev_decode_dstring( dstring_t *ds );

extern char *dev_to_string( alphabet_t *a, symbol_t *dtext );

extern dstring_t *dev_digitalize( alphabet_t *a, char *seq );

extern dstring_t *dev_new_dstring( alphabet_t *a, int n, symbol_t c );

extern dstring_t *dev_dstring_append( dstring_t *a, dstring_t *b );

extern void dev_free_dstring( dstring_t *seq );

extern int dev_isspecial( alphabet_t *a, symbol_t c );

#endif
