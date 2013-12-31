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

#define MAXERRORMSGS 8
static PSZ ErrorMsgs[ MAXERRORMSGS ] = 
{
   "ADDR",  /* -1 */
   "SOCK",  /* -2 */
   "CONN",  /* -3 */
   "SRVR",  /* -4 */
   "USER",  /* -5 */
   "PASS",  /* -6 */
   "STAT",  /* -7 */
   "VALU",  /* -8 */
};

static void
CenterWindow( HWND hwnd )
/***********************/
{
   RECTL rcl;
   RECTL rclParent;

   WinQueryWindowRect( hwnd, &rcl );
   WinQueryWindowRect( WinQueryWindow( hwnd, QW_PARENT ), &rclParent );

   WinSetWindowPos( hwnd, HWND_TOP, ( rclParent.xRight - rcl.xRight ) / 2,
                    ( rclParent.yTop - rcl.yTop ) / 2, 0, 0, 
                    SWP_SHOW | SWP_MOVE | SWP_ZORDER );
}

static HWND
LoadHelp( HWND hwndFrame )
/************************/
{
   PPIB     ppib;
   PTIB     ptib;
   HELPINIT hi;
   PSZ      pszExt;
   CHAR     szName[ CCHMAXPATH ];
   HWND     hwndHelp;

   DosGetInfoBlocks( &ptib, &ppib );

   DosQueryModuleName( ppib->pib_hmte, sizeof( szName ), szName );

   pszExt = strrchr( szName, '.' );
   if ( pszExt )
   {
      strcpy( pszExt, ".hlp" );
   }
   else
   {
      strcat( szName, ".hlp" );
   }

   hi.cb                       = sizeof( HELPINIT );
   hi.ulReturnCode             = 0;
   hi.pszTutorialName          = NULL;
   hi.phtHelpTable             = ( PHELPTABLE ) MAKELONG( DLG_PMLPP, 0xffff );
   hi.hmodHelpTableModule      = NULLHANDLE;
   hi.hmodAccelActionBarModule = NULLHANDLE;
   hi.idAccelTable             = 0;
   hi.idActionBar              = 0;
   hi.pszHelpWindowTitle       = "Help for PMLPP";
   hi.fShowPanelId             = CMIC_HIDE_PANEL_ID;
   hi.pszHelpLibraryName       = szName;

   hwndHelp = WinCreateHelpInstance( WinQueryAnchorBlock( hwndFrame ), &hi );
   if ( hwndHelp != NULLHANDLE )
   {
      WinAssociateHelpInstance( hwndHelp, hwndFrame );
      return ( hwndHelp );
   }
   else
   {
      return ( NULLHANDLE );
   }
}

static HWND
LoadMenu( void )
/**************/
{
   MENUITEM mi;
   HWND     hmenu = WinLoadMenu( HWND_OBJECT, NULLHANDLE, MNU_CONTEXT );
   if ( hmenu != NULLHANDLE )
   {
      WinSendMsg( hmenu, MM_QUERYITEM, MPFROM2SHORT( IDM_HELP, TRUE ),
                  MPFROMP( &mi ));

      WinSetWindowBits( mi.hwndSubMenu, QWL_STYLE, MS_CONDITIONALCASCADE,
                        MS_CONDITIONALCASCADE );

      WinSendMsg( mi.hwndSubMenu, MM_SETDEFAULTITEMID,
                  MPFROMSHORT( IDM_GENERALHELP ), MPVOID );
   }

   return ( hmenu );
}

static void
SetButtonIcon( HWND     hwndButton,
               HPOINTER hicon      )
/**********************************/
{
   WNDPARAMS wp;
   BTNCDATA  bd;

   wp.fsStatus  = WPM_CTLDATA;
   wp.cbCtlData = sizeof( BTNCDATA );
   wp.pCtlData  = &bd;

   WinSendMsg( hwndButton, WM_QUERYWINDOWPARAMS, MPFROMP( &wp ), MPVOID );
   bd.hImage = hicon;
   WinSendMsg( hwndButton, WM_SETWINDOWPARAMS, MPFROMP( &wp ), MPVOID );
}

static HWND
LaunchMailreader( HWND    hwnd,
                  PCONFIG pcfg )
/******************************/
{
   PROGDETAILS   pd = { 0 };

   pd.Length                        = sizeof( pd );
   pd.progt.progc                   = PROG_DEFAULT;
   pd.pszExecutable                 = pcfg->szProgram;
   pd.pszStartupDir                 = pcfg->szDirectory;
   pd.pszParameters                 = pcfg->szOptions;
   pd.swpInitial.hwndInsertBehind   = HWND_TOP;
   pd.swpInitial.fl                 = SWP_ACTIVATE | SWP_SHOW;
   pd.swpInitial.hwnd               = hwnd;

   return ( WinStartApp( hwnd, &pd, NULL, NULL, SAF_INSTALLEDCMDLINE ));
}

MRESULT EXPENTRY
TransparentWndProc( HWND   hwnd,
                    ULONG  msg,
                    MPARAM mp1,
                    MPARAM mp2  )
/*******************************/
{
   PFNWP pfnOrgWndProc = ( PFNWP ) WinQueryWindowPtr( hwnd, 0 );

   switch ( msg )
   {
      case WM_CONTEXTMENU:
      case WM_BUTTON1MOTIONSTART:
      case WM_BUTTON2MOTIONSTART:
      case WM_BUTTON1DBLCLK:
      case WM_BUTTON2DBLCLK:
         {
            WinSendMsg( WinQueryWindow( hwnd, QW_OWNER ), msg, mp1, mp2 );
         }
         return ( MRTRUE );

      case WM_PAINT:
         {
            if ( WinQueryWindowUShort( hwnd, QWS_ID ) == ST_STATUS )
            {
               RECTL rcl;
               HPS   hps;

               pfnOrgWndProc( hwnd, msg, mp1, mp2 );

               WinQueryWindowRect( hwnd, &rcl );

               hps = WinGetPS( hwnd );

               WinDrawBorder( hps, &rcl, 1, 1, 0, 0, DB_AREAATTRS | 0x0800 );

               WinReleasePS( hps );

               return ( MRTRUE );
            }
         }
         break;
   }

   return ( pfnOrgWndProc( hwnd, msg, mp1, mp2 ));
}

MRESULT EXPENTRY
PasswordDlgProc( HWND   hwnd,
                 ULONG  msg,
                 MPARAM mp1,
                 MPARAM mp2  )
/****************************/
{
   static PCONFIG pcfg = NULL;

   switch ( msg )
   {
      case WM_INITDLG :
         {
            pcfg = ( PCONFIG ) PVOIDFROMMP( mp2 );
         }
         return ( MRFALSE );

      case WM_COMMAND:
         {
            if ( SHORT1FROMMP( mp1 ) == DID_OK )
            {
               WinQueryDlgItemText( hwnd, EF_PASSWORD, sizeof( pcfg->szPassword ),
                                    pcfg->szPassword );
               WinDismissDlg( hwnd, 1 );
            }
         }
         return ( MRZERO );

      case WM_HELP:
         {
            HWND hwndHelp;

            hwndHelp = WinQueryHelpInstance( hwnd );
            if ( hwndHelp != NULLHANDLE )
            {
               WinSendMsg( hwndHelp, HM_DISPLAY_HELP, MPFROMSHORT( EF_PASSWORD ),
                           MPFROMSHORT( HM_RESOURCEID ));
            }
         }
         return ( MRZERO );
   }

   return ( WinDefDlgProc( hwnd, msg, mp1, mp2 ));
}

MRESULT EXPENTRY
PMLPPDlgProc( HWND   hwnd,
              ULONG  msg,
              MPARAM mp1,
              MPARAM mp2  )
/*************************/
{
   static HPOINTER hiconMain   = NULLHANDLE;
   static HPOINTER hiconMail   = NULLHANDLE;
   static HWND     hwndStatus  = NULLHANDLE;
   static HWND     hwndButton  = NULLHANDLE;
   static HWND     hwndHelp    = NULLHANDLE;
   static HWND     hmenu       = NULLHANDLE;
   static PCONFIG  pcfg        = NULL;
   static LONG     cbOldMsgs   = ( -666 );
   static BOOL     bForced     = FALSE;

   switch ( msg )
   {
      case WM_INITDLG :
         {
            pcfg = ( PCONFIG ) PVOIDFROMMP( mp2 );

            pcfg->hwndNotify = hwnd;

            hiconMain = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, DLG_PMLPP );
            hiconMail = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ICO_MAIL );
            WinSendMsg( hwnd, WM_SETICON, MPFROMLONG( hiconMain ), MPVOID );

            hwndStatus = WinWindowFromID( hwnd, ST_STATUS );

            WinSetWindowPtr( hwndStatus, 0,
                             WinSubclassWindow( hwndStatus, TransparentWndProc ));

            hwndButton = WinWindowFromID( hwnd, PB_REFRESH );

            WinSetWindowPtr( hwndButton, 0,
                             WinSubclassWindow( hwndButton, TransparentWndProc ));

            hmenu = LoadMenu( );

            hwndHelp = LoadHelp( hwnd );

            if ( LoadProfile( hwnd, pcfg ))
            {
               WinSetWindowPos( hwnd, HWND_TOP, pcfg->ptl.x, pcfg->ptl.y, 0, 0,
                                SWP_MOVE | SWP_SHOW | SWP_ZORDER );

               if ( ! *( pcfg->szPassword ))
               {
                  WinDlgBox( HWND_DESKTOP, hwnd, PasswordDlgProc, NULLHANDLE, 
                             DLG_PASSWORD, pcfg );
               }

               StartPolling( pcfg );
            }
            else
            {
               CenterWindow( hwnd );
               WinPostMsg( hwnd, WM_COMMAND, MPFROMSHORT( IDM_PROPERTIES ),
                           MPFROM2SHORT( CMDSRC_OTHER, FALSE ));
            }
         }
         return ( MRFALSE );

      case WM_FORMATFRAME:
         {
            int   n;
            PSWP  pswp   = ( PSWP ) PVOIDFROMMP( mp1 );
            SHORT cbCtls = SHORT1FROMMR( WinSendMsg( hwnd, WM_QUERYFRAMECTLCOUNT,
                                                     MPVOID, MPVOID ));

            for ( n = 0; n < cbCtls; n++ )
            {
               pswp[ n ].fl &= ~( SWP_SHOW );
               pswp[ n ].cx =  0;
               pswp[ n ].cy =  0;
            }

            return ( MRFROMSHORT( cbCtls ));
         }

      case WM_MINMAXFRAME:
         {
            PSWP pswp = ( PSWP ) PVOIDFROMMP( mp1 );

            if ( pswp->fl & SWP_MAXIMIZE )
            {
               return ( MRTRUE );
            }
         }
         return ( MRFALSE );

      case WM_BUTTON1DBLCLK:
      case WM_BUTTON2DBLCLK:
         {
            WinSendMsg( hwnd, WM_COMMAND, MPFROMSHORT( IDM_MAILREADER ),
                        MPFROM2SHORT( CMDSRC_OTHER, TRUE ));
         }
         return ( MRTRUE );


      case WM_BUTTON1MOTIONSTART:
      case WM_BUTTON2MOTIONSTART:
         {
            WinSendMsg( hwnd, WM_TRACKFRAME, MPFROMSHORT( TF_MOVE | TF_SETPOINTERPOS ),
                        MPVOID );
         }
         return ( MRTRUE );

      case WM_CONTEXTMENU :
         {
            if ( hmenu != NULLHANDLE )
            {
               POINTL ptl;

               WinQueryPointerPos( HWND_DESKTOP, &ptl );

               WinPopupMenu( HWND_DESKTOP, hwnd, hmenu, ptl.x, ptl.y, IDM_MAILREADER,
                             PU_POSITIONONITEM | PU_HCONSTRAIN | PU_VCONSTRAIN | PU_NONE |
                             PU_SELECTITEM | PU_KEYBOARD | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 );
            }
         }
         return ( MRTRUE );

      case WM_COMMAND :
         {
            switch ( SHORT1FROMMP( mp1 ))
            {
               case IDM_PROPERTIES:
                  {
                     Configure( hwnd, pcfg );
                  }
                  break;

               case PB_REFRESH:
                  {
                     if (( cbOldMsgs > 0 ) && *( pcfg->szProgram ) &&
                          WinIsMenuItemEnabled( hmenu, IDM_MAILREADER ))
                     {
                        if ( LaunchMailreader( hwnd, pcfg ) == NULLHANDLE )
                        {
                           WinMessageBox( HWND_DESKTOP, hwnd, "Unable to load reader", "ERROR",
                                          0, MB_ENTER | MB_MOVEABLE | MB_ERROR );
                        }
                        else
                        {
                           WinEnableMenuItem( hmenu, IDM_MAILREADER, FALSE );
                           break;
                        }
                     }
                  }
                  /* fall through to refresh... */

               case IDM_REFRESH:
                  {
                     WinSetWindowText( hwndStatus, "--" );

                     bForced = TRUE;

                     PollNow( pcfg );
                  }
                  break;

               case IDM_MAILREADER:
                  {
                     if ( LaunchMailreader( hwnd, pcfg ) == NULLHANDLE )
                     {
                        WinMessageBox( HWND_DESKTOP, hwnd, "Unable to load reader", "ERROR",
                                       0, MB_ENTER | MB_MOVEABLE | MB_ERROR );
                     }
                     else
                     {
                        WinEnableMenuItem( hmenu, IDM_MAILREADER, FALSE );
                     }
                  }
                  break;
            }
         }
         return ( MRZERO );

      case WM_USER:
         {
            LONG cbMsgs;

            cbMsgs = LONGFROMMP( mp1 );
            if (( cbMsgs != cbOldMsgs ) || bForced )
            {
               CHAR      szMsgs[ 32 ];
               ULONG     ulColor;
               HPOINTER  hicon;

               bForced   = FALSE;
               
               if ( cbMsgs < 0 )
               {
                  cbMsgs = -( cbMsgs );

                  strcpy( szMsgs, "PMLPP - " );

                  if ( cbMsgs >= MAXERRORMSGS )
                  {
                     strcat( szMsgs, "????" );
                  }
                  else
                  {
                     strcat( szMsgs, ErrorMsgs[ cbMsgs - 1 ] );
                  }

                  ulColor = CLR_RED;
                  hicon = hiconMain;
               }
               else if ( cbMsgs == 0 )
               {
                  sprintf( szMsgs, "PMLPP - 0" );
                  ulColor = CLR_PALEGRAY;
                  hicon = hiconMain;
               }
               else
               {
                  sprintf( szMsgs, "PMLPP - %lu", cbMsgs );
                  ulColor = CLR_GREEN;
                  hicon = hiconMail;

                  if (( cbOldMsgs <= 0 ) && ( pcfg->bSound ))
                  {
                     WinAlarm( HWND_DESKTOP, WA_NOTE );
                  }
               }

               cbOldMsgs = cbMsgs;

               WinSetPresParam( hwndButton, PP_BACKGROUNDCOLORINDEX, sizeof( ulColor ),
                                &ulColor );

               WinSetWindowText( hwndStatus, szMsgs + 8 );

               if ( cbMsgs >= 0 )
               {
                  strcat( szMsgs, " Msgs" );
               }

               WinSetWindowText( hwnd, szMsgs );

               WinSendMsg( hwnd, WM_SETICON, MPFROMLONG( hicon ), MPVOID );

               SetButtonIcon( hwndButton, hicon );
            }
         }
         return ( MRZERO );

      case WM_SAVEAPPLICATION:
         {
            SWP swp;

            StopPolling( pcfg );

            if ( hwndHelp != NULLHANDLE )
            {
               WinAssociateHelpInstance( NULLHANDLE, hwnd );
               WinDestroyHelpInstance( hwndHelp );
            }

            WinQueryWindowPos( hwnd, &swp );

            if (( swp.x != pcfg->ptl.x ) || ( swp.y != pcfg->ptl.y ))
            {
               if ( WinQueryWindowULong( hwnd, QWL_STYLE ) & WS_MINIMIZED )
               {
                  pcfg->ptl.x = WinQueryWindowUShort( hwnd, QWS_XRESTORE );
                  pcfg->ptl.y = WinQueryWindowUShort( hwnd, QWS_YRESTORE );
               }
               else
               {
                  pcfg->ptl.x = swp.x;
                  pcfg->ptl.y = swp.y;
               }

               SaveProfile( hwnd, pcfg );
            }
         }
         return ( MRZERO );

      case WM_APPTERMINATENOTIFY:
         {
            WinEnableMenuItem( hmenu, IDM_MAILREADER, TRUE );
            PollNow( pcfg );
         }
         return ( MRZERO );

      case WM_HELP:
         {
            switch ( SHORT1FROMMP( mp1 ))
            {
               case IDM_HELPINDEX:
                  {
                     WinSendMsg( hwndHelp, HM_HELP_INDEX, MPVOID, MPVOID );
                  }
                  break;

               case 0: /* F1 */
               case PB_HELP:
               case IDM_GENERALHELP:
                  {
                     WinSendMsg( hwndHelp, HM_HELP_CONTENTS, MPVOID, MPVOID );
                  }
                  break;

               case IDM_USINGHELP:
                  {
                     WinSendMsg( hwndHelp, HM_DISPLAY_HELP, MPVOID, MPVOID );
                  }
                  break;

               case IDM_ABOUTPMLPP:
                  {
                     HWND hwndAbout = WinLoadDlg( HWND_DESKTOP, hwnd, WinDefDlgProc,
                                                  NULLHANDLE, DLG_ABOUT, 0 );
                     if ( hwndAbout != NULLHANDLE )
                     {
                        CenterWindow( hwndAbout );

                        WinProcessDlg( hwndAbout );

                        WinDestroyWindow( hwndAbout );
                     }
                  }
                  break;
            }
         }
         return ( MRZERO );
   }

   return ( WinDefDlgProc( hwnd, msg, mp1, mp2 ));
};

int
main( void )
/**********/
{
   HAB hab = WinInitialize( 0 );
   if ( hab != NULLHANDLE )
   {
      HMQ hmq = WinCreateMsgQueue( hab, 0 );
      if ( hmq != NULLHANDLE )
      {
         HMTX hmtx;

         if ( DosCreateMutexSem( "\\SEM32\\PMLPP", &hmtx, DC_SEM_SHARED, TRUE ) == NO_ERROR )
         {
            CONFIG cfg = { 0 };
            HWND   hwnd = WinLoadDlg( HWND_DESKTOP, HWND_DESKTOP, PMLPPDlgProc,
                                      NULLHANDLE, DLG_PMLPP, &cfg );
            if ( hwnd != NULLHANDLE )
            {
               WinProcessDlg( hwnd );
               WinDestroyWindow( hwnd );
            }

            DosCloseMutexSem( hmtx );
         }
         else
         {
            WinAlarm( HWND_DESKTOP, WA_NOTE );

            hmtx = NULLHANDLE;

            if ( DosOpenMutexSem( "\\SEM32\\PMLPP", &hmtx ) == NO_ERROR )
            {
               PID     pid = 0;
               TID     tid = 0;
               ULONG   ulJunk;

               if ( DosQueryMutexSem( hmtx, &pid, &tid, &ulJunk ) == NO_ERROR )
               {
                  HSWITCH hswitch;

                  hswitch = WinQuerySwitchHandle( NULLHANDLE, pid );
                  if ( hswitch != NULLHANDLE )
                  {
                     WinSwitchToProgram( hswitch );
                  }
               }

               DosCloseMutexSem( hmtx );
            }
         }

         WinDestroyMsgQueue( hmq );
      }

      WinTerminate( hab );
   }

   return ( 0 );
}
