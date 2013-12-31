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

#ifndef __PMLPP_H
#  define __PMLPP_H

#  pragma once

#  include <stdlib.h>
#  include <stddef.h>
#  include <stdio.h>
#  include <string.h>

#  define  INCL_DOS
#  define  INCL_DOSERRORS
#  define  INCL_PM
#  include <os2.h>

#  include "controls.h"

#  define  MRZERO      MRFROMSHORT( 0 )
#  define  MRTRUE      MRFROMSHORT( TRUE )
#  define  MRFALSE     MRFROMSHORT( FALSE )
#  define  THREADSTKSZ 32768

#  pragma pack( 1 )

   typedef struct _config 
   {  
      BOOL     bSound;
      ULONG    ulInterval;
      USHORT   usPort;
      CHAR     szServer[ 256 ];
      CHAR     szUser[ 32 ];
      CHAR     szPassword[ 32 ];
      CHAR     szProgram[ 256 ];
      CHAR     szOptions[ 256 ];
      CHAR     szDirectory[ 256 ];
      POINTL   ptl;
      BOOL     bForceIP;
      BOOL     bSavePW;
      BOOL     bUseAPOP;

      /* everything above is saved to the INI file */
#     define   CFGSAVESIZE offsetof( struct _config, sock )

      int      sock;
      HWND     hwndNotify;
      TID      tid;
      HEV      hev;
      HTIMER   htimer;
      BOOL     bAlive;

   }  CONFIG, *PCONFIG;

#  pragma pack( )

   extern BOOL StartPolling( PCONFIG pcfg );
   extern void StopPolling( PCONFIG pcfg );
   extern void PollNow( PCONFIG pcfg );

   extern BOOL LoadProfile( HWND hwnd, PCONFIG pprf );
   extern void SaveProfile( HWND hwnd, PCONFIG pprf );
   extern void Configure( HWND hwnd, PCONFIG pprf );

#endif


