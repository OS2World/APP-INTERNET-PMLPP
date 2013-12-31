/*
PMLPP - A POP3 mail "peeker" for OS/2<tm>.
Copyright (C) 1997 James R. Louvau

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

You may contact the author of PMLPP at:

E-mail     : jim@vwm.com
Snail-mail : Jim Louvau
             3437 335th St.
             West Des Moines, IA 50266
*/

#include "pmlpp.h"

#include <process.h>
#include <time.h>
#include <stdarg.h>

#include <types.h>
#include <sys\socket.h>
#include <netdb.h>
#include <utils.h>
#include <netinet\in.h>
#include <sys\ioctl.h>

#include "md5.h"

#ifdef DEBUG
static void
LogErr( char * fmt,
        ...        )
/******************/
{
   struct sockaddr_in sin = { 0 };
   int                sock;

   sock = socket( PF_INET, SOCK_DGRAM, 0 );
   if ( sock < 0 )
   {
      return;
   }

   sin.sin_family      = AF_INET;
   sin.sin_port        = htons( 514 );
   sin.sin_addr.s_addr = INADDR_ANY;

   if ( connect( sock, ( struct sockaddr * ) &sin, sizeof( sin )) >= 0 )
   {
      char                 buf[ 100 ];
      char               * p;
      va_list              ap;
      time_t               now;

      time( &now );

      p = buf;

      p += sprintf( p, "<3>%.15s PMLPP: ", ctime( &now ) + 4 );

      va_start( ap, fmt );
      p += vsprintf( p, fmt, ap );
      va_end( ap );

      *p = '\0';

      send( sock, buf, strlen( buf ) + 1, 0 );
   }
   
   soclose( sock );
}
#endif /* DEBUG */

static int
Transact( int      sock,
          char   * obuf,
          char   * ibuf,
          int      ilen )
/***********************/
{
   static char   buffer[ 4096 ];
   static char * bufptr = buffer;
   static char * bufend = buffer;
   int           bytes;
   int           socks[ 1 ];

   if (( obuf != NULL ) && *obuf )
   {
      int olen = strlen( obuf );

      if (( ioctl( sock, FIONREAD, ( caddr_t ) &bytes, sizeof( bytes )) >= 0 ) && bytes )
      {
         for ( ; ; )
         {
            socks[ 0 ] = sock;

            if ( select( socks, 1, 0, 0, 500 ) <= 0 )
            {
               break;
            }

            if ( recv( sock, buffer, sizeof( buffer ), 0 ) <= 0 )
            {
               break;
            }
         }
      }

      while ( olen > 0 )
      {
         bytes = send( sock, obuf, olen, 0 );
         if ( bytes <= 0 )
         {
#ifdef DEBUG
            LogErr(  "Transact()::send() returned %d", sock_errno( ));
#endif
            return ( bytes );
         }

         olen -= bytes;
         obuf += bytes;
      }
   }

   if (( ibuf != NULL ) && ( ilen > 0 ))
   {
      int    c;
      int    lastc = '\0';
      char * iptr  = ibuf;

      bufptr = bufend = buffer;

      *iptr = '\0';

      while ( --ilen > 0 )
      {
         if ( bufptr == bufend )
         {
            bytes = recv( sock, buffer, sizeof( buffer ), 0 );
            if ( bytes <= 0 )
            {
#ifdef DEBUG
               LogErr(  "Transact()::recv() returned %d", sock_errno( ));
#endif
               *iptr = '\0';
               return ( bytes );
            }

            bufptr = buffer;
            bufend = buffer + bytes;
         }

         c = *bufptr++;

         if (( c == '\r' ) || ( c == '\0' ))
         {
            lastc = c;
            ++ilen;
            continue;
         }

         if (( c == '\n' ) && ( lastc == '\r' ))
         {
            lastc = '\n';
            break;
         }

         lastc = c;
         *iptr++ = c;
      }

      *iptr = '\0';

      if ( lastc != '\n' )
      {
         for ( ; ; )
         {
            socks[ 0 ] = sock;

            if ( select( socks, 1, 0, 0, 500 ) <= 0 )
            {
               break;
            }

            if ( recv( sock, buffer, sizeof( buffer ), 0 ) <= 0 )
            {
               break;
            }
         }
      }

      return ( *ibuf );
   }

   return ( '+' );
}

static BOOL
Logon( PSZ     pszSrvr,
       PCONFIG pcfg    )  
/**********************/
{
   CHAR szBuf[ 4096 ];

   if ( pcfg->bUseAPOP )
   {
      char * p;

      p = strchr( pszSrvr, '<' );
      if ( p != NULL )
      {
         strcpy( szBuf, p );

         p = strchr( szBuf, '>' );
         if ( p != NULL )
         {
            MD5_CTX   context;
            int       n;

            *( p + 1 ) = '\0';

            strcat( szBuf, pcfg->szPassword );

            MD5Init( &context );
            MD5Update( &context, szBuf, strlen( szBuf ));
            MD5Final( &context );

            p = szBuf;

            p += sprintf( p, "APOP %s ", pcfg->szUser );

            for ( n = 0; n < 16; n++ )
            {
               p += sprintf( p, "%02.2x", context.digest[ n ] );
            }

            sprintf( p, "\r\n" );

            if ( Transact( pcfg->sock, szBuf, szBuf, sizeof( szBuf )) == '+' )
            {
               return ( TRUE );
            }
         }
      }
   }

   sprintf( szBuf, "USER %s\r\n", pcfg->szUser );

   if ( Transact( pcfg->sock, szBuf, szBuf, sizeof( szBuf )) == '+' )
   {
      sprintf( szBuf, "PASS %s\r\n", pcfg->szPassword );
   
      if ( Transact( pcfg->sock, szBuf, szBuf, sizeof( szBuf )) == '+' )
      {
         return ( TRUE );
      }
#ifdef DEBUG
      else
      {
         LogErr(  "Logon()[PASS] got \"%s\"", szBuf );
      }
#endif
   }
#ifdef DEBUG
   else
   {
      LogErr(  "Logon()[USER] got \"%s\"", szBuf );
   }
#endif

   return ( FALSE );
}

static long
GetMsgCnt( PCONFIG pcfg )
/***********************/
{
   struct sockaddr_in   sa  = { 0 };
   struct hostent     * he;
   CHAR                 szBuf[ 512 ];
   long                 lMsgCnt = 0;
   int                  noblock;
   u_long               ip = 0;

   sa.sin_family = AF_INET;

   if ( pcfg->usPort != 0 )
   {
      sa.sin_port   = htons( pcfg->usPort );
   }
   else
   {
      struct servent * pse = getservbyname( "pop3", "tcp" );
      if ( pse == NULL )
      {
         sa.sin_port = htons( 110 );
      }
      else
      {
         sa.sin_port = pse->s_port;
      }
   }
   
   he = gethostbyname( pcfg->szServer );
   if ( he == NULL )
   {
      ip = inet_addr( pcfg->szServer );

      he = gethostbyaddr(( char * ) &ip, sizeof( ip ), AF_INET );
      if ( he == NULL )
      {
         if  (( ip == 0 ) || ( ip == 0xffffffff ) || ( ! pcfg->bForceIP ))
         {
#ifdef DEBUG
            LogErr( "GetMsgCnt()::gethostby*() returned %d", sock_errno( ));
#endif
            return ( -1 ); /* can't resolve address */
         }
      }
   }

   if ( he != NULL )
   {
      sa.sin_addr.s_addr = *(( u_long * ) he->h_addr );
   }
   else
   {
      /* 
       * Use raw IP even though DNS lookup failed.
       * Only happens if bForceIP is TRUE.
       */
      sa.sin_addr.s_addr = ip;
   }

   pcfg->sock = socket( PF_INET, SOCK_STREAM, 0 );
   if ( pcfg->sock == ( -1 ))
   {
#ifdef DEBUG
      LogErr(  "GetMsgCnt()::socket() returned %d", sock_errno( ));
#endif
      return ( -2 ); /* no socket */
   }

   noblock = 0;
   ioctl( pcfg->sock, FIONBIO, ( caddr_t ) &noblock, sizeof( noblock ));

   if ( connect( pcfg->sock, ( struct sockaddr * ) &sa, sizeof( sa )) >= 0 )
   {
      if ( Transact( pcfg->sock, NULL, szBuf, sizeof( szBuf )) == '+' )
      {
         if ( Logon( szBuf, pcfg ))
         {
            if ( Transact( pcfg->sock, "STAT\r\n", szBuf, sizeof( szBuf )) == '+' )
            {
               char * p;

               p = strchr( szBuf, ' ' );
               if ( p != NULL )
               {
                  lMsgCnt = strtol( p, NULL, 10 );
               }

               Transact( pcfg->sock, "QUIT\r\n", szBuf, sizeof( szBuf ));
            }
            else
            {
#ifdef DEBUG
               LogErr(  "GetMsgCnt()::Transact()[STAT] got \"%s\"", szBuf );
#endif
               lMsgCnt = ( -7 ); /* STAT failed */
            }
         }
         else
         {
            lMsgCnt = ( -6 );
         }
      }
      else
      {
#ifdef DEBUG
         LogErr(  "GetMsgCnt()::Transact()[connecting] got \"%s\"", szBuf ); 
#endif
         lMsgCnt = ( -4 ); /* server won't talk to us */
      }
   }
   else
   {
#ifdef DEBUG
      LogErr(  "GetMsgCnt()::connect() returned %d", sock_errno( ));
#endif
      lMsgCnt = ( -3 ); /* no connect */
   }

   soclose( pcfg->sock );
   pcfg->sock = ( -1 );

   return ( lMsgCnt );
}

static void
PollThread( void * pvParm )
/*************************/
{
   PCONFIG pcfg = ( PCONFIG ) pvParm;
   ULONG   ulJunk;
   LONG    cbMsgs;
   HAB     hab;
   int     tries;

   hab = WinInitialize( 0 );

   pcfg->bAlive = TRUE;

   while ( pcfg->bAlive )
   {
      if (( DosWaitEventSem( pcfg->hev, SEM_INDEFINITE_WAIT ) != NO_ERROR ) ||
           ( ! pcfg->bAlive )) 
      {
         continue;
      }

      cbMsgs = GetMsgCnt( pcfg );

      for ( tries = 0; tries < 10; tries++ )
      {
         if ( WinPostMsg( pcfg->hwndNotify, WM_USER, MPFROMLONG( cbMsgs ), MPVOID ))
         {
            break;
         }
         
         DosSleep( 1000 );
      }

      DosResetEventSem( pcfg->hev, &ulJunk );
   }

   WinTerminate( hab );

   _endthread( );
}

BOOL
StartPolling( PCONFIG pcfg )
/**************************/
{
   if ( pcfg->bAlive )
   {
      return ( TRUE );
   }

   if ( DosCreateEventSem( NULL, &( pcfg->hev ), DC_SEM_SHARED, TRUE ) == NO_ERROR )
   {
      if ( DosStartTimer( 1000L * pcfg->ulInterval, ( HSEM ) pcfg->hev, &( pcfg->htimer ))
           == NO_ERROR )
      {
         pcfg->tid = _beginthread( PollThread, NULL, THREADSTKSZ, pcfg );
         if ( pcfg->tid != ( -1 ))
         {
            return ( TRUE );
         }

         DosStopTimer( pcfg->htimer );
      }

      DosCloseEventSem( pcfg->hev );
   }

   return ( FALSE );
}

void
StopPolling( PCONFIG pcfg )
/*************************/
{
   if ( ! pcfg->bAlive )
   {
      return;
   }

   DosSuspendThread( pcfg->tid );

   pcfg->bAlive = FALSE;

   DosPostEventSem( pcfg->hev );

   if ( pcfg->sock != ( -1 ))
   {
      so_cancel( pcfg->sock );
      shutdown( pcfg->sock, 2 );
   }

   DosResumeThread( pcfg->tid );

   DosWaitThread( &( pcfg->tid ), DCWW_WAIT );

   DosStopTimer( pcfg->htimer );
   DosCloseEventSem( pcfg->hev );
}

void
PollNow( PCONFIG pcfg )
/*********************/
{
   if ( pcfg->bAlive )
   {
      DosPostEventSem( pcfg->hev );
   }
};

