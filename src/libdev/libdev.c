/*                               -*- Mode: C -*- 
 * devtools.c --- Truong Nguyen and defines commonly used functions
 * Author          : Marcel Turcotte
 * Created On      : Tue Jun  7 13:23:18 2005
 * Last Modified By: turcotte
 * Last Modified On: Wed Feb 21 13:10:18 2018
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 *
 * Many of these functions are simply wrappers to system calls. In
 * future releases, their definition might be updated to ease
 * debugging.
 */

#include "libdev.h"
#include "config.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef HAS_MALLINFO
#include <malloc.h>
#endif

/*****************************************************************
 * global variables                                              *
 *****************************************************************/

static int debug_level = DEFAULT_DEBUG_LEVEL;

#ifdef HAS_MALLINFO
static unsigned long memory_usage;
#endif

/*****************************************************************
 * dev_init - initializes global variables                       *
 *****************************************************************/

void
dev_init()
{
  errno = 0;
}

#ifdef HAS_MALLINFO

/*****************************************************************
 * dev_get_memory_usage - returns the peak memory usage          *
 *****************************************************************/

unsigned long
dev_get_memory_usage()
{
  return memory_usage;
}

#endif

/*****************************************************************
 * dev_asc_memory_usage - returns the string representation      *
 *****************************************************************/

#define KILO 1024
#define MEGA 1048576
#define GIGA 1073741824

char *
dev_asc_memory_usage( char **buf, unsigned long mem )
{
  unsigned long giga, mega, kilo;

  giga = mem / GIGA;
  mem  = mem % GIGA;

  mega = mem / MEGA;
  mem  = mem % MEGA;

  kilo = mem / KILO;
  mem  = mem % KILO;
  
  *buf = dev_malloc( 64 );

  if ( giga > 0 ) 
    sprintf( *buf, "%ld.%ld Gbytes", giga, mega );
  else if ( mega > 0 )
    sprintf( *buf, "%ld.%ld Mbytes", mega, kilo );
  else if ( kilo > 0 )
    sprintf( *buf, "%ld.%ld Kbytes", kilo, mem );
  else
    sprintf( *buf, "%ld bytes", mem );

  return *buf;
}

/*****************************************************************
 * dev_malloc - wrapper for malloc                               *
 * size : the number of bytes                                    *
 * return : a pointer to the newly allocated space               *
 *****************************************************************/

void *
_dev_malloc( char *name, int line, size_t size ) 
{
  void *p;

  p = malloc( size );
  
  if ( p == NULL ) {
    fprintf( stderr, "%s line %d, malloc( %lu bytes ) failed: ", name, line, (unsigned long) size );
    perror( "Memory allocation failed: " );

  }

#ifdef HAS_MALLINFO
  unsigned long current = mallinfo().uordblks;
  if ( current > memory_usage )
    memory_usage = current;
#endif

  return p;
}

/*****************************************************************
 * dev_realloc - wrapper for realloc                             *
 * ptr : memory block                                            *
 * size : the new size                                           *
 * return : a pointer to the newly allocated memory              *
 *****************************************************************/

void *
_dev_realloc( char *name, int line, void *ptr, size_t size ) 
{
  void *p;

  p = realloc( ptr, size );
  
  if ( p == NULL ) {
    fprintf( stderr, "%s line %d, realloc( %lu bytes ) failed: ", name, line, (unsigned long) size );
    perror( "Memory (re-)allocation failed: " );
  }

#ifdef HAS_MALLINFO
  unsigned long current = mallinfo().uordblks;
  if ( current > memory_usage )
    memory_usage = current;
#endif

  return p;
}

/*****************************************************************
 * dev_free - wrapper for free                                   *
 * p : pointer to the memory block                               *
 *****************************************************************/

void
dev_free( void *p )
{
  free( p );
}

/*****************************************************************
 * dev_free_array -                                              *
 * p : pointer to an array                                       *
 * size : size of the array                                      *
 *****************************************************************/

void
dev_free_array( void **p, int size )
{
  int i;

  for ( i=0; i<size; i++ )
    dev_free( p[ i ] );

  dev_free( p );
}

/*****************************************************************
 * dev_log - log facility                                        *
 * level : when to log this message                              *
 * the rest of the arguments are the same as printf              *
 *****************************************************************/

void
dev_log( int level, char * format, ... )
{
  va_list args;

  if ( debug_level < level )
    return;

  va_start( args, format );
  vprintf( format, args );
  va_end( args );
  printf( "\n" );

}

/*****************************************************************
 * _dev_die - displays an error message and exits                *
 * src : source file of the caller                               *
 * line : line of the call                                       *
 * msg1 : first message to be displayed                          *
 * msg2 : first message to be displayed                          *
 *****************************************************************/

void
_dev_die( char *src, int line, char * format, ... )
{
  va_list args;

  fprintf( stderr, "%s line %i: ", src, line );
  va_start( args, format );
  vfprintf( stderr, format, args );
  va_end( args );
  fprintf( stderr, "\n" );

  exit( EXIT_FAILURE );
}

/*****************************************************************
 * dev_set_debug_level - sets the debug level to argument        *
 * level : the new debug level                                   *
 * return : the old debug level                                  *
 *****************************************************************/

int
dev_set_debug_level( int level )
{
  int old = debug_level;
  debug_level = level;
  return old;
}

/*****************************************************************
 * dev_get_debug_level - gets the debug level to argument        *
 *****************************************************************/

int
dev_get_debug_level( void )
{
  return debug_level;
}

/*****************************************************************
 * dev_fopen - wrapper for fopen                                 *
 * name : file name                                              *
 * mode : read (r), write (w), append (a)                        *
 *****************************************************************/

FILE *
_dev_fopen( const char *src, int line, const char *name, const char *mode )
{
  FILE *fh;

  assert( src != NULL );

  fh = fopen( name, mode );

  if ( fh == NULL ) {
    fprintf( stderr, "%s line %d: Cannot open file %s: ", src, line, name );
    perror( "" );
    exit( EXIT_FAILURE );
  }

  return fh;
}

/*****************************************************************
 * dev_new_filename -                                            *
 *****************************************************************/

char *
dev_new_filename( char *dir, char *name, char *ext )
{
  assert( name != NULL );

  char *filename = dev_malloc( FILENAME_MAX );

  filename[ 0 ] = '\0';

  if ( dir != NULL )
    sprintf( filename, "%s%c", dir, DIRECTORY_SEPARATOR );

  sprintf( filename, "%s%s", filename, name );

  if ( ext != NULL )
    sprintf( filename, "%s%s", filename, ext );

  return filename;
}

/*****************************************************************
 * dev_isdir - returns true if file_name designates a directory  *
 *****************************************************************/

int
dev_isdir( const char *file_name )
{
  struct stat fbuf;

  if ( stat( file_name, &fbuf ) == -1 ) {
    dev_log( 3, "stat failed for %file_name" );
  }

  return S_ISDIR( fbuf.st_mode );
}

/*****************************************************************
 * dev_trim_dirname -                                            *
 *****************************************************************/

void
dev_trim_dirname( char *name )
{
  int pos = strlen( name ) - 1;

  while ( pos >= 0 && name[ pos ] == DIRECTORY_SEPARATOR )
    name[ pos-- ] = '\0';
}

/*****************************************************************
 * dev_mkdir -                                                   *
 *****************************************************************/

void
dev_mkdir_or_die( const char *path )
{
  if ( mkdir( path, S_IRWXU | S_IRWXG | S_IRWXO ) == -1 ) {

    char msg[ FILENAME_MAX + 64 ] = "";
    sprintf( msg, "cannot mkdir %s", path );
    perror( msg );
    exit( EXIT_FAILURE );

  }
}

/*****************************************************************
 * dev_parse_int -                                               *
 *****************************************************************/

int
dev_parse_int( const char *s )
{
  int result;

  if ( sscanf( s, "%d", &result ) != 1 )
    dev_die( "not an int %s", s );

  return result;
}

/*****************************************************************
 * dev_parse_float -                                             *
 *****************************************************************/

float
dev_parse_float( const char *s )
{
  float result;

  if ( sscanf( s, "%f", &result ) != 1 )
    dev_die( "not a float %s", s );

  return result;
}

/*****************************************************************
 * dev_strcpy - wrapper for strcpy that also allocates memory    *
 * s : input string                                              *
 * return a pointer to a fresh copy of the input string          *
 *****************************************************************/

char *
dev_strcpy( const char *s )
{
  char *new;

  if ( s == NULL  )
    return NULL;

  new = dev_malloc( strlen( s ) + 1 );
  strcpy( new, s );

  return new;
}

/*****************************************************************
 * dev_symcpy - copies the content of s to a new array           *
 * s : array of symbols                                          *
 * return a pointer to a fresh copy of the input                 *
 *****************************************************************/

symbol_t *
dev_symcpy( const symbol_t *s, int length )
{
  symbol_t *new;

  if ( s == NULL  )
    return NULL;

  new = dev_malloc( length * sizeof( symbol_t ) );

  for ( int i=0; i<length; i++ )
    new[ i ] = s[ i ];

  return new;
}

/*****************************************************************
 * dev_isnewline -                                               *
 * c : input character                                           *
 * return true if c is a new line                                *
 *                                                               *
 * DOS/Windows is using CR+LF                                    *
 * Unices are using LF                                           *
 * Mac OS < 10 was using CR                                      *
 *****************************************************************/

int
dev_isnewline( char c )
{
  return c == '\n';
}

/*****************************************************************
 * dev_write_lines -                                             *
 * s : input string                                              *
 * w : line width                                                *
 * return the number of lines written                            *
 *****************************************************************/

int
dev_write_lines( char *s, int width, FILE *fh )
{
  int i, lines = 0, len = strlen( s );

  for ( i=0; i<len; i++ ) {

    if ( i != 0 && ( i%width )==0 ) {
      fputc( '\n', fh );
      lines++;
    }

    fputc( s[ i ], fh );
  }

  fputc( '\n', fh );
  lines++;

  fflush( fh );

  return lines;
}

/*****************************************************************
 * dev_encode - encodes a character using a given alphabet       *
 * a : alphabet                                                  *
 * c : input character                                           *
 *****************************************************************/

symbol_t
dev_encode( alphabet_t *a, char s )
{
  int i;

  for ( i=0; i<a->length; i++ )
    if ( a->codes[ i ].sym == s )
      return a->codes[ i ].code;
  
  dev_die( "not a valid character %c", s );

  return -1;
}

/*****************************************************************
 * dev_decode - decodes c using a given alphabet                 *
 * a : alphabet                                                  *
 * c : code                                                      *
 *****************************************************************/

char
dev_decode( alphabet_t *a, symbol_t c )
{
  int i;

  for ( i=0; i<a->length; i++ )
    if ( a->codes[ i ].code == c)
      return a->codes[ i ].sym;
  
  dev_die( "not a valid code %d", c );

  return -1;
}

/*****************************************************************
 * dev_decode_dstring - decodes a digital string                 *
 *****************************************************************/

char *
dev_decode_dstring( dstring_t *ds )
{
  char *s = dev_malloc( ds->length );

  for ( int i=0; i< ( ds->length - 1 ); i++ )
    if ( dev_isspecial( ds->alphabet, ds->text[ i ] ) )
      s[ i ] = '$'; /* fix me */
    else
      s[ i ] = dev_decode( ds->alphabet, ds->text[ i ] );

  s[ ds->length - 1 ] = '\0';

  return s;
}

/*****************************************************************
 * dev_digitalize - transforms a string into a digital string    *
 *****************************************************************/

dstring_t *
dev_digitalize( alphabet_t *a, char *seq )
{
  int i, n = strlen( seq );
  dstring_t *dstring;

  dstring = ( dstring_t * ) dev_malloc( sizeof( dstring_t ) );

  dstring->text = ( symbol_t * ) dev_malloc( ( n+1 ) * sizeof( symbol_t ) );

  dstring->length = n+1;

  dstring->alphabet = a;

  for ( i=0; i<n; i++ )
    dstring->text[ i ] = dev_encode( a, seq[ i ] );

  dstring->text[ n ] = a->size; /* terminator */

  return dstring;
}

/*****************************************************************
 * dev_to_string - decodes sequence of digital symbols           *
 *****************************************************************/

char *
dev_to_string( alphabet_t *a, symbol_t *dtext )
{
  assert( dtext != NULL );

  int length = 0;

  while ( dtext[ length ] < a->size )
    length ++;

  char *text = dev_malloc( length+1 );

  for ( int i=0; i < ( length ); i++ )
    if ( dev_isspecial( a, dtext[ i ] ) )
      text[ i ] = '$'; /* fix me */
    else
      text[ i ] = dev_decode( a, dtext[ i ] );

  text[ length ] = '\0';

  return text;
}

/*****************************************************************
 * dev_new_dstring -                                             *
 *****************************************************************/

dstring_t *
dev_new_dstring( alphabet_t *a, int n, symbol_t c )
{
  dstring_t *dstring;

  dstring = ( dstring_t * ) dev_malloc( sizeof( dstring_t ) );

  dstring->text = ( symbol_t * ) dev_malloc( ( n+1 ) * sizeof( symbol_t ) );

  dstring->length = n;

  dstring->alphabet = a;

  for ( int i=0; i<(n-1); i++ )
    dstring->text[ i ] = c;

  dstring->text[ n-1 ] = a->size; /* terminator */

  return dstring;
}

/*****************************************************************
 * dev_dstring_append -                                          *
 *****************************************************************/

dstring_t *
dev_dstring_append( dstring_t *a, dstring_t *b )
{
  int m = a->length, n = b->length;
  dstring_t *dstring;
  pos_t pos;

  assert( a->alphabet == b->alphabet );

  dstring = ( dstring_t * ) dev_malloc( sizeof( dstring_t ) );

  dstring->text = ( symbol_t * ) dev_malloc( ( m+n-1 ) * sizeof( symbol_t ) );

  dstring->length = m+n-1;

  dstring->alphabet = a->alphabet;

  pos = 0;

  for ( int i=0; i<(m-1); i++ )
    dstring->text[ pos++ ] = a->text[ i ];

  /* dstring->text[ pos++ ] = a->alphabet->size; */
  
  for ( int i=0; i<(n-1); i++ )
    dstring->text[ pos++ ] = b->text[ i ];

  dstring->text[ pos++ ] = a->alphabet->size;
  
  return dstring;
}

/*****************************************************************
 * dev_free_dstring -                                            *
 * Note: does not free the space associated with the alphabet.   *
 *****************************************************************/

void
dev_free_dstring( dstring_t *ds )
{
  dev_free( ds->text );
  dev_free( ds );
}

/*****************************************************************
 * dev_isspecial - special characters include the terminators    *
 *****************************************************************/

int
dev_isspecial( alphabet_t *a, symbol_t c )
{
  return c >= a->size;
}


