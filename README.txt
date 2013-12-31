"Little Pop Peek for PM" (PMLPP)

A historical (hysterical?) note: I threw this together after
becoming quite frustrated with a couple of the other alternatives
available. While the others looked nice, the only two that were
really small/fast "applets" refused to talk to both my local pop3
server (PowerWeb++) or my ISP's (some *nix variant on DEC hardware).
At least one author in his readme mentioned looking for a "hello"
string or somesuch - I suggest he go back and look at the RFC's and
actually read the pop3 spec next time instead of just peeking at the
examples. Anyway, this one's probably got bugs too, but at least
they're different that the other guys; i.e., this one works where
theirs didn't AFAIK, the reverse is true :-) The big difference is
that if mine doesn't work for you, *YOU* (not me... what do you
expect for free? :) have the source and can fix it.

Installation:

   Copy pmlpp.exe and pmlpp.hlp to any convenient directory and run
   it. A configuration dialog will come up automatically the first
   time you run it. Context-sensitive help is available. You can
   access the config dialog at any time by popping up the
   system/context menu (usually a mouse-button-2 over the app window).
   
This software is "Copyleft'ed" (GNU general license) and includes 'C'
source code. The included executable was compiled with Watcom C 11.0,
optimized with LXOPT and compressed. The makefile is for Watcom, but
everything should be trivial and generic enough to easily recompile
it with VAC++ or Borland (I've done it with the former, Watcom's
EXE was about 11K smaller :-). 

NOTE: PMLPP keeps all of its configuration in PMLPP.INI, which it
places and looks-for in the same directory as the executable. It
looks for PMLPP.HLP (its "help" file) in the same place. So, if you
move the executable, move pmlpp.hlp and pmlpp.ini also, or things
won't work right :-)

Changes in version 1.10:
-----------------------

To upgrade, simply copy pmlpp.exe over your old version.

1) Added APOP support.
2) Added option to save/not-save the password.
3) If you try to start a second copy of PMLPP, it will now bring the
running copy to the foreground rather than beeping and bitching that
a copy is already running.
4) Some code cleanup out of personal pride... no bugs reported or
found, hence, nothing has been "fixed."

Changes in version 1.20: ( not released publically )
-----------------------

Upgrade as above - READ NOTE #1 below!
                   ^^^^^^^^^^^^^^^^^^

1) Changed polling interval granularity from minutes to SECONDS! I don't
   know why anyone would want this, but the change was asked for by
   enough people to warrant it. The valid range is now 1 .. 86400. If 
   you set it up to poll every 2 seconds and things "break," don't come
   crying to me :-)

2) Found and fixed some potential problems with the pop routines - I've
   reverted to the "KISS" principle and have cut out some fancy and 
   unneccesary baggage. 

3) Added a visual indication that a poll command is really being executed.
   If you manually force a refresh, the message count window will show
   "--" until the poll completes/returns.

4) There is some limited debugging/logging code in the pop.c module. 
   If you "#define DEBUG" in pop.c and recompile, the EXE will
   write specific socket errors and such to whatever you have defined
   as your syslog. Of course, you must have syslogd running :-)

Changes in version 1.21:
-----------------------

1) Double-clicking either mouse button over the main window will launch
   your mail reader.
   
Changes in version 1.22:
-----------------------   

1) Added code to IGNORE a DNS lookup failure on an IP address. In
   other words, if you use an IP address rather than a host/domain
   name for the pop server, make sure it's typed in correctly, as
   PMLPP will attempt to use it even if it can't confirm that said
   address exists by doing a DNS/HOSTS-file lookup. This allows you
   to "force" PMLPP to connect to a specific machine, bypassing
   DNS.
   
Changes in version 1.30:
-----------------------

1) Somewhere along the line I trashed the "Play Sound" and "Save
   Password" toggles - neither one was behaving as advertised.
   Fixed.
   
2) Added a small dialog that prompts for the password on startup
   if it wasn't saved in the config file - much friendlier than
   popping up the entire configuration dialog :-)
   
Changes in version 1.40:
-----------------------

1) Changed the "refresh" button to serve a dual purpose: If you've
   defined a mailreader, AND there is new mail available, clicking
   on the button will launch your mail reader; Otherwise, it
   reverts to the old behavior of polling the server. The "refresh"
   menu item still works as before and will only cause a refresh.
   
   Thanks to Olexander Lebid for the idea.

Changes in version 2.10:
-----------------------

1) Properties (configuration) is now done with a notebook control.

2) Made APOP a toggleable item in properties - some servers were
   choking on the first attempt at an APOP handshake, and
   wouldn't recover; i.e., the "normal" handshake attempt that
   would follow an APOP failure wouldn't work either. The
   default is OFF, so if you want APOP, be sure and enable it
   in the properties notebook.
   
3) Had some leftover debugging code that caused IBM's beta MPTS to
   choke - they've cured their breakage (thanks Bertram, Steve
   and especially Lori  - IBM has a lot of great people, they just
   hide them well :-) and I've wiped the superfluous code :-)

Changes in version 2.20:
-----------------------

1) Oops! I screwed up the window centering code in 2.10 - noticed
   about 10 minutes after I uploaded it to Hobbes :-( Fixed.

Enjoy,

Jim Louvau
jim@vwm.com
