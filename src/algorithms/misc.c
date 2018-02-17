/*                               -*- Mode: C -*- 
 * misc.c --- tidbits
 * Author          : Marcel Turcotte
 * Created On      : Wed Jul 13 14:51:03 2005
 * Last Modified By: Marcel Turcotte
 * Last Modified On: Mon Aug  8 20:14:55 2005
 *
 * This copyrighted source code is freely distributed under the terms
 * of the GNU General Public License. 
 * See the files COPYRIGHT and LICENSE for details.
 */

#include "seed.h"
#include <time.h>

#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR 3600

/*****************************************************************
 * time_limit_exceeded -                                         *
 *****************************************************************/

int
time_limit_exceeded( param_t *params )
{
  time_t now;

  if ( params->time_limit == 0 )
    return FALSE;

  ( void ) time( &now );

  int res = ( ( ( now - params->start_time ) / SECONDS_PER_MINUTE ) >= params->time_limit );

  if ( res )
    dev_log( 1, "[ time limit exceeded ]" );

  return res;
}

/*****************************************************************
 * asc_cpu_time - returns a character representation of the time *
 * elapsed.                                                      *
 *****************************************************************/

char *
asc_cpu_time( char **buf, param_t *params )
{
  int hours, minutes, seconds, delta;

  time_t now;

  ( void ) time( &now );

  delta = now - params->start_time;
  
  hours = delta / SECONDS_PER_HOUR;
  delta = delta % SECONDS_PER_HOUR;
  minutes = delta / SECONDS_PER_MINUTE;
  seconds = delta % SECONDS_PER_MINUTE;

  *buf = dev_malloc( 64 );

  if ( hours > 0 )
    sprintf( *buf, "%d hours, %d minutes, %d seconds", hours, minutes, seconds );
  else if ( minutes > 0 )
    sprintf( *buf, "%d minutes, %d seconds", minutes, seconds );
  else
    sprintf( *buf, "%d seconds", seconds );
    
  return *buf;
}
