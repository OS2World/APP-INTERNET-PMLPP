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

static PSZ
GetProfileName( )
/***************/
{
   static CHAR szPath[ CCHMAXPATH ] = "";
   PTIB        ptib;
   PPIB        ppib;

   if ( ! *szPath )
   {
      char * p;

      DosGetInfoBlocks( &ptib, &ppib );
      DosQueryModuleName(( HMODULE ) ppib->pib_hmte, sizeof( szPath ), szPath );

      p = strrchr( szPath, '.' );
      if ( p )
      {
         strcpy( p, ".INI" );
      }
      else
      {
         strcat( szPath, ".INI" );
      }
   }

   return ( szPath );
}

BOOL
LoadProfile( HWND    hwnd,
             PCONFIG pprf )
/*************************/
{
   HINI hini = PrfOpenProfile( WinQueryAnchorBlock( hwnd ), GetProfileName( ));

   if ( hini != NULLHANDLE )
   {
      ULONG cbProfile = CFGSAVESIZE;
      BOOL  bGotIt    = PrfQueryProfileData( hini, "PMLPP", "Config", pprf, &cbProfile );

      PrfCloseProfile( hini );

      if ( bGotIt )
      {
         return ( TRUE );
      }
   }

   return ( FALSE );
}

void
SaveProfile( HWND    hwnd,
             PCONFIG pprf )
/*************************/
{
   HINI hini = PrfOpenProfile( WinQueryAnchorBlock( hwnd ), GetProfileName( ));
   if ( hini != NULLHANDLE )
   {
      CHAR szPassword[ 32 ];

      strcpy( szPassword, pprf->szPassword );

      if ( ! pprf->bSavePW )
      {
         memset( pprf->szPassword, 0, sizeof( pprf->szPassword ));
      }

      PrfWriteProfileData( hini, "PMLPP", "Config", pprf, CFGSAVESIZE );
      PrfCloseProfile( hini );

      strcpy( pprf->szPassword, szPassword );
   }
}

MRESULT EXPENTRY
NBPageDlgProc( HWND   hwnd,
               ULONG  msg,
               MPARAM mp1, 
               MPARAM mp2  )
/**************************/
{
   switch ( msg )
   {
      case WM_INITDLG:
         {
         }
         return ( MRTRUE );

      case WM_COMMAND:
         {
         }
         return ( MRTRUE );

      case WM_HELP:
         {  
            HWND   hwndHelp;
            HWND   hwndFocus;
            USHORT usCtlID;

            hwndHelp = WinQueryHelpInstance( hwnd );

            hwndFocus = WinQueryFocus( HWND_DESKTOP );
            
            usCtlID = WinQueryWindowUShort( hwndFocus, QWS_ID );

            WinSendMsg( hwndHelp, HM_DISPLAY_HELP, MPFROMSHORT( usCtlID ),
                        MPFROMSHORT( HM_RESOURCEID ));
         }
         return ( MRZERO );
   }

   return ( WinDefDlgProc( hwnd, msg, mp1, mp2 ));
}

MRESULT EXPENTRY
ConfigDlgProc( HWND   hwnd,
               ULONG  msg,
               MPARAM mp1, 
               MPARAM mp2  )
/**************************/
{
   static PCONFIG pcfg        = NULL;
   static HWND    hwndOptions = NULLHANDLE;
   static HWND    hwndPolling = NULLHANDLE;
   static HWND    hwndServer  = NULLHANDLE;
   static HWND    hwndMailer  = NULLHANDLE;

   switch ( msg )
   {
      case WM_INITDLG:
         {
            HWND   hwndBook;
            ULONG  ulPageID;
            HPS    hps;
            RECTL  rcl = { 0, 0, 500, 500 };
            USHORT w = 0;
            USHORT h = 0;

            pcfg = ( PCONFIG ) PVOIDFROMMP( mp2 );

            hwndBook = WinWindowFromID( hwnd, NB_CONFIGURE );

            hps = WinGetPS( hwndBook );

            /*** OPTIONS ***/
            ulPageID = LONGFROMMR( WinSendMsg( hwndBook, BKM_INSERTPAGE, MPVOID, MPFROM2SHORT( BKA_AUTOPAGESIZE | BKA_MAJOR | BKA_STATUSTEXTON, BKA_LAST )));
            hwndOptions = WinLoadDlg( hwnd, hwnd, NBPageDlgProc, NULLHANDLE, NBKP_OPTIONS, 0 );
            WinSendMsg( hwndBook, BKM_SETPAGEWINDOWHWND, MPFROMLONG( ulPageID ), MPFROMHWND( hwndOptions ));
            WinSendMsg( hwndBook, BKM_SETTABTEXT, MPFROMLONG( ulPageID ), MPFROMP( "Options" ));

            WinDrawText( hps, ( -1 ), "Options", &rcl, 0, 0, DT_QUERYEXTENT | DT_CENTER | DT_VCENTER );
            w = ( USHORT ) __max( w, rcl.xRight - rcl.xLeft );
            h = ( USHORT ) __max( h, rcl.yTop - rcl.yBottom );

            WinCheckButton( hwndOptions, CB_PLAYSOUND, ( pcfg->bSound ) ? 1 : 0 );

            /*** POLLING ***/
            ulPageID = LONGFROMMR( WinSendMsg( hwndBook, BKM_INSERTPAGE, MPVOID, MPFROM2SHORT( BKA_AUTOPAGESIZE | BKA_MAJOR | BKA_STATUSTEXTON, BKA_LAST )));
            hwndPolling = WinLoadDlg( hwnd, hwnd, NBPageDlgProc, NULLHANDLE, NBKP_POLLING, 0 );
            WinSendMsg( hwndBook, BKM_SETPAGEWINDOWHWND, MPFROMLONG( ulPageID ), MPFROMHWND( hwndPolling ));
            WinSendMsg( hwndBook, BKM_SETTABTEXT, MPFROMLONG( ulPageID ), MPFROMP( "Polling" ));

            WinDrawText( hps, ( -1 ), "Polling", &rcl, 0, 0, DT_QUERYEXTENT | DT_CENTER | DT_VCENTER );
            w = ( USHORT ) __max( w, rcl.xRight - rcl.xLeft );
            h = ( USHORT ) __max( h, rcl.yTop - rcl.yBottom );

            if (( pcfg->ulInterval < 1L ) || ( pcfg->ulInterval > 86400L ))
            {
               pcfg->ulInterval = 60L;
            }

            WinSendDlgItemMsg( hwndPolling, SPB_INTERVAL, SPBM_SETLIMITS, MPFROMLONG( 86400L ),
                               MPFROMLONG( 1L ));

            WinSendDlgItemMsg( hwndPolling, SPB_INTERVAL, SPBM_SETCURRENTVALUE,
                               MPFROMLONG( pcfg->ulInterval ), MPVOID );

            /*** SERVER ***/
            ulPageID = LONGFROMMR( WinSendMsg( hwndBook, BKM_INSERTPAGE, MPVOID, MPFROM2SHORT( BKA_AUTOPAGESIZE | BKA_MAJOR | BKA_STATUSTEXTON, BKA_LAST )));
            hwndServer = WinLoadDlg( hwnd, hwnd, NBPageDlgProc, NULLHANDLE, NBKP_SERVER, 0 );
            WinSendMsg( hwndBook, BKM_SETPAGEWINDOWHWND, MPFROMLONG( ulPageID ), MPFROMHWND( hwndServer ));
            WinSendMsg( hwndBook, BKM_SETTABTEXT, MPFROMLONG( ulPageID ), MPFROMP( "Server" ));

            WinDrawText( hps, ( -1 ), "Server", &rcl, 0, 0, DT_QUERYEXTENT | DT_CENTER | DT_VCENTER );
            w = ( USHORT ) __max( w, rcl.xRight - rcl.xLeft );
            h = ( USHORT ) __max( h, rcl.yTop - rcl.yBottom );

            WinSetDlgItemText( hwndServer, EF_SERVER, pcfg->szServer );
            WinSetDlgItemText( hwndServer, EF_USER, pcfg->szUser );
            WinSetDlgItemText( hwndServer, EF_PASSWORD, pcfg->szPassword );
            WinSetDlgItemShort( hwndServer, EF_PORT, pcfg->usPort, FALSE );
            WinCheckButton( hwndServer, CB_SAVEPASSWORD, ( pcfg->bSavePW ) ? 1 : 0 );
            WinCheckButton( hwndServer, CB_FORCEIP, ( pcfg->bForceIP ) ? 1 : 0 );
            WinCheckButton( hwndServer, CB_APOP, ( pcfg->bUseAPOP ) ? 1 : 0 );

            /*** MAILER ***/
            ulPageID = LONGFROMMR( WinSendMsg( hwndBook, BKM_INSERTPAGE, MPVOID, MPFROM2SHORT( BKA_AUTOPAGESIZE | BKA_MAJOR | BKA_STATUSTEXTON, BKA_LAST )));
            hwndMailer = WinLoadDlg( hwnd, hwnd, NBPageDlgProc, NULLHANDLE, NBKP_MAILER, 0 );
            WinSendMsg( hwndBook, BKM_SETPAGEWINDOWHWND, MPFROMLONG( ulPageID ), MPFROMHWND( hwndMailer ));
            WinSendMsg( hwndBook, BKM_SETTABTEXT, MPFROMLONG( ulPageID ), MPFROMP( "Mailer" ));

            WinDrawText( hps, ( -1 ), "Mailer", &rcl, 0, 0, DT_QUERYEXTENT | DT_CENTER | DT_VCENTER );
            w = ( USHORT ) __max( w, rcl.xRight - rcl.xLeft );
            h = ( USHORT ) __max( h, rcl.yTop - rcl.yBottom );

            WinSetDlgItemText( hwndMailer, EF_PROGRAM, pcfg->szProgram );
            WinSetDlgItemText( hwndMailer, EF_OPTIONS, pcfg->szOptions );
            WinSetDlgItemText( hwndMailer, EF_DIRECTORY, pcfg->szDirectory );

            /* Make it pretty... */
            WinReleasePS( hps );

            WinSendMsg( hwndBook, BKM_SETDIMENSIONS, MPFROM2SHORT( w + 10, h + 10 ), MPFROMSHORT( BKA_MAJORTAB ));

            WinSendMsg( hwndBook, BKM_SETDIMENSIONS, MPFROM2SHORT( 0, 0 ), MPFROMSHORT( BKA_MINORTAB ));

            WinSendMsg( hwndBook, BKM_SETNOTEBOOKCOLORS, MPFROMLONG( SYSCLR_FIELDBACKGROUND ), MPFROMSHORT( BKA_BACKGROUNDPAGECOLORINDEX ));
         }
         return ( MRFALSE );

      case WM_COMMAND:
         {
            switch ( SHORT1FROMMP( mp1 ))
            {
               case DID_OK:
                  {
                     StopPolling( pcfg );

                     /*** OPTIONS ***/
                     pcfg->bSound = ( WinQueryButtonCheckstate( hwndOptions, CB_PLAYSOUND ) ? TRUE
                                      : FALSE );

                     /*** POLLING ***/
                     WinSendDlgItemMsg( hwndPolling, SPB_INTERVAL, SPBM_QUERYVALUE, 
                                        MPFROMP( &( pcfg->ulInterval )), 
                                        MPFROM2SHORT( 0, SPBQ_UPDATEIFVALID ));

                     /*** SERVER ***/
                     WinQueryDlgItemText( hwndServer, EF_SERVER, sizeof( pcfg->szServer ),
                                          pcfg->szServer );
                     WinQueryDlgItemText( hwndServer, EF_USER, sizeof( pcfg->szUser ),
                                          pcfg->szUser );
                     WinQueryDlgItemText( hwndServer, EF_PASSWORD, sizeof( pcfg->szPassword ),
                                          pcfg->szPassword );
                     WinQueryDlgItemShort( hwndServer, EF_PORT, ( PSHORT ) &( pcfg->usPort ),
                                           FALSE );
                     pcfg->bForceIP = ( WinQueryButtonCheckstate( hwndServer, CB_FORCEIP ) ? TRUE
                                        : FALSE );

                     pcfg->bSavePW = ( WinQueryButtonCheckstate( hwndServer, CB_SAVEPASSWORD ) ? TRUE
                                       : FALSE );

                     pcfg->bUseAPOP = ( WinQueryButtonCheckstate( hwndServer, CB_APOP ) ? TRUE
                                        : FALSE );

                     /*** MAILER ***/
                     WinQueryDlgItemText( hwndMailer, EF_PROGRAM, sizeof( pcfg->szProgram ),
                                          pcfg->szProgram );
                     WinQueryDlgItemText( hwndMailer, EF_OPTIONS, sizeof( pcfg->szOptions ),
                                          pcfg->szOptions );
                     WinQueryDlgItemText( hwndMailer, EF_DIRECTORY, sizeof( pcfg->szDirectory ),
                                          pcfg->szDirectory );

                     StartPolling( pcfg );

                     WinDismissDlg( hwnd, 1 );
                  }
                  break;

               case DID_CANCEL:
                  {
                     WinDismissDlg( hwnd, 0 );
                  }
                  break;
            }
         }
         return ( MRZERO );
   }

   return ( WinDefDlgProc( hwnd, msg, mp1, mp2 ));
}

void
Configure( HWND    hwnd,
           PCONFIG pprf )
/***********************/
{
   if ( WinDlgBox( HWND_DESKTOP, hwnd, ConfigDlgProc, NULLHANDLE, DLG_CONFIGURE, pprf ))
   {
      SaveProfile( hwnd, pprf );
   }
}
