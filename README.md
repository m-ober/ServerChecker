# ServerChecker

A simple, yet very useful tool to manage one or many gameservers in
one place. For supported servers/protocols, ServerChecker
directly communicates with the server and will restart it upon
a failure - even if the process is still running.

The original author of this software seems to have abandoned it,
as there was no update for quite some time and now even
the webpage is no longer reachable.

## Build

For Visual Studio 2013 (and above), the Multibyte MFC Library is required:

https://www.microsoft.com/download/details.aspx?id=40770

## Usage

This version should be running fine on Windows XP and above (used by myself on 
Windows Server 2008 R2).

For the most part, the software should be self-explanatory. If ServerChecker
keeps restarting a process over and over, while the server should be running fine,
it may be the case that the protocol is outdated and thus ServerChecker
cannot successfully communicate with the server, which will then trigger a restart.
If this happens, choose "Normal Program". This will only restart the server
if the process crashes/exits.

Note that Windows Error Reporting and everything else that may require user
interaction should be disabled, otherwise ServerChecker may not recognize that
a process crashed, because it still seems to be running (but actually Windows
wants you to choose whether you want to send an error report or not).

For more information on this topic, see:

https://technet.microsoft.com/en-us/library/cc754364.aspx

### Download

Pre-built binaries can be found here:

https://github.com/m-ober/ServerChecker/releases

### Screenshot

![ServerChecker](https://raw.githubusercontent.com/m-ober/ServerChecker/docs/serverchecker.png "ServerChecker")

## License

ServerChecker will keep supported servers up and running
Copyright (C) 2005  [DumB]H!GHGuY

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
