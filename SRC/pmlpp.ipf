:userdoc.
:title.Help for PMLPP
:docprof  toc=123456 
ctrlarea=coverpage.
:h1 id=ID@1.Legal Notice
:p.Copyright (C) 1997 James R. Louvau 
:p.This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version. This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details. You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
:p.You may contact the author of PMLPP at&colon. E-mail &colon. jim@vwm.com
Snail-mail &colon. Jim Louvau 3437 335th St. West Des Moines, IA 50266 
:p.
:p.
:p.
:p.
:p.
:h1 id=ID@2.Overview
:i1.Overview
PMLPP's main window consists of three main elements&colon. 
:ol.
:li.
:link reftype=hd refid=ID@3.A pushbutton/icon.
:elink. 
:li.
:link reftype=hd refid=ID@4.A message-count display window.
:elink. 
:li.
:link reftype=hd refid=ID@5.A popup/context menu.
:elink. 
:eol.
:note.You can drag the main window by clicking and holding either mouse button. 
:h2 id=ID@3.Refresh Button
:i1.Refresh Button
Clicking on the refresh button results in one of two actions: If you have
defined a mailreader, and there is mail pending, PMLPP will attempt to launch
your reader. Failing that, PMLPP will poll the server immediately if possible. 
The icon background of the button will also reflect PMLPP's current status. 
If the background is :color fc='PaleGray' bc='default'.GRAY
:color fc='Default' bc='Default'., there is no mail on the server. 
If :color fc='Green' bc='default'.GREEN
:color fc='Default' bc='Default'., one or more messages are available. 
If :color fc='Red' bc='default'.RED
:color fc='Default' bc='Default'., something has malfunctioned (see 
:link reftype=hd refid=ID@19.Error Codes
:elink.). 
:h2 id=ID@4.Message Count Window
:i1.Message Count Window
This is the small window below the refresh/icon button. One of two things will
appear here; a message count as of the last poll, or an error code. 
:h2 id=ID@5.Context Menu
:i1.Context Menu
PMLPP uses a combination system and context/popup menu. Clicking the
context-menu mouse button (usually button 2 or the right one) anywhere over the
main window will bring up the menu. 
Use the "
:link reftype=hd res=263.Properties
:elink." option to open a configuration dialog. 
:h1 id=ID@6 res=263.Configuration
:i1.Configuration
The following options are available for configuring PMLPP&colon. 
:sl.
:li.
:link reftype=hd res=265.Play Sound.
:elink. 
:li.
:link reftype=hd res=267.Minutes.
:elink. 
:li.
:link reftype=hd res=271.Server.
:elink. 
:li.
:link reftype=hd res=273.Port.
:elink. 
:li.
:link reftype=hd res=275.User.
:elink. 
:li.
:link reftype=hd res=277.Password.
:elink. 
:li.
:link reftype=hd res=292.Save Password.
:elink. 
:li.
:link reftype=hd res=293.Force IP.
:elink. 
:li.
:link reftype=hd res=294.APOP.
:elink. 
:li.
:link reftype=hd res=280.Program.
:elink. 
:li.
:link reftype=hd res=282.Options.
:elink. 
:li.
:link reftype=hd res=284.Directory.
:elink. 
:esl.
:h2 id='265' res=265.Play Sound
:i1.Play Sound
If this box is checked, PMLPP will beep :color fc='Red' bc='default'.ONCE 
:color fc='Default' bc='Default'.when it detects the :color
fc='Red' bc='default'.first 
:color fc='Default' bc='Default'.message; i.e., when the message count changes
from 0 (zero) to 1 (one). 
:h2 id='267' res=267.Seconds
:i1.Seconds
"Click" on the arrow-buttons or type in the number of :color
fc='Red' bc='default'.SECONDS 
:color fc='Default' bc='Default'.PMLPP should pause between polls to the pop
server. 
:note.The valid range is :color fc='Red' bc='default'.1 
:color fc='Default' bc='Default'.to :color fc='Red' bc='default'.86400 
:color fc='Default' bc='Default'.(24 hours). 
:h2 id='271' res=271.Server
:i1.Server
Your pop server's address as a domain name like&colon. 
:xmp.
mail.mydomain.com
:exmp.
or as an I.P. address like&colon. 
:xmp.
167.142.225.5
:exmp.
:h2 id='273' res=273.Port
:i1.Port
The POP3 "service" port (usually 110). 
:note.If set to :color fc='Red' bc='default'.0 (zero)
:color fc='Default' bc='Default'., PMLPP will look up the appropriate value in
your host's "services" file. This is your best bet for staying out of trouble. 
:h2 id='275' res=275.User
:i1.User
You, silly! The name you use to logon to the pop server. 
:caution.
Your user name is almost always :color fc='Red' bc='default'.CASE SENSITIVE! 
:color fc='Default' bc='Default'.
:ecaution.
:h2 id='277' res=277.Password
:i1.Password
The password you use to logon to the pop server. 
:caution.
As with your user name, passwords are almost always :color fc='Red'
bc='default'.CASE SENSITIVE! 
:color fc='Default' bc='Default'. 
:ecaution.
:h2 id='292' res=292.Save Password
:i1.Save Password
If this box is checked, PMLPP will save your password in its INI file, where
somebody could conceivably see it (INI files are binary, but text in strings is
visible). 
If left unchecked, you will be prompted for your password when PMLPP is
launched. 
:h2 id='293' res=293.Force IP
:i1.Force IP
If this box is checked, and you have entered a (numerically) valid I.P. address
in the server field, PMLPP will ignore any errors returned from DNS lookup, and
attempt to use this I.P. address. In other words, 
:hp2.YOU :ehp2.
are assuming responsibility for the correctness of the address. 
:h2 id='294' res=294.APOP
:i1.APOP
Checking this item causes PMLPP to attempt an APOP login. Should APOP fail,
PMLPP will revert to a normal login. 
:h2 id='280' res=280.Program
:i1.Program
The pathname of your mail-reader program. You only need to set this if you would
like to be able to launch your mail-reader from PMLPP's menu. 
:h2 id='282' res=282.Options
:i1.Options
Command-line option to pass to the reader, if any. 
:h2 id='284' res=284.Directory
:i1.Directory
What directory to change to before running the reader. 
:h1 id=ID@19.Error Codes
:i1.Error Codes
The following codes may appear in the "message count" window&colon. 
:dl.
:dt.ADDR 
:dd.
:dt.
:dd.PMLPP was unable to resolve the server name/address (gethostbyname() and/or
gethostbyaddr() failed). 
:dt.SOCK 
:dd.
:dt.
:dd.PMLPP was unable to obtain a new socket from the system (socket() failed). 
:dt.CONN 
:dd.
:dt.
:dd.PMLPP was unable to connect to the server (connect() failed). 
:dt.SRVR 
:dd.
:dt.
:dd.The server won't talk to PMLPP. It isn't running, or for some reason (it's
too busy, etc.) refuses to accept commands (we timed out waiting for it to
"talk" to us, or the server didn't respond with "+OK"). 
:dt.USER 
:dd.
:dt.
:dd.The server has rejected your user name; i.e., it doesn't like you or doesn't
know who you are. Make sure you've configured your name correctly. 
:dt.PASS 
:dd.
:dt.
:dd.The server has rejected your password. Make sure you've entered it
correctly. 
:nt.
Depending on the server, you may see this even when it's the USER name field
that's incorrect.
:ent.
:dt.STAT 
:dd.
:dt.
:dd.The server doesn't want to give us statistics. You should NEVER see this,
and if you do, it probably means something on the server end is broken. 
:dt.VALU 
:dd.
:dt.
:dd.PMLPP couldn't parse the statistics string returned by the server. You
should never see this either. 
:dt.
:dd.
:edl.
:note.Some of the above error codes may appear erroneously if something happens
to the connection in mid "conversation." In other words, you might see a "PASS"
error if PMLPP was waiting for the server to acknowledge your password, and the
connection was suddenly broken. 
:euserdoc.