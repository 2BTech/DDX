<!---
################################################################################
##                         DATA DISPLAY APPLICATION X                         ##
##                            2B TECHNOLOGIES, INC.                           ##
##                                                                            ##
## The DDX is free software: you can redistribute it and/or modify it under   ##
## the terms of the GNU General Public License as published by the Free       ##
## Software Foundation, either version 3 of the License, or (at your option)  ##
## any later version.  The DDX is distributed in the hope that it will be     ##
## useful, but WITHOUT ANY WARRANTY; without even the implied warranty of     ##
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General  ##
## Public License for more details.  You should have received a copy of the   ##
## GNU General Public License along with the DDX.  If not, see                ##
## <http://www.gnu.org/licenses/>.                                            ##
##                                                                            ##
##  For more information about the DDX, check out the 2B website or GitHub:   ##
##       <http://twobtech.com/DDX>       <https://github.com/2BTech/DDX>      ##
################################################################################
-->

# DDX Remote Management Protocol
The DDX uses [JSON-RPC 2.0](http://www.jsonrpc.org/specification).  At the low level, each DDX daemon opens a TCP server, by default on port 4388, to which GUIs, other DDX daemons, and other data sources or sinks can connect. Every RPC object transmitted must be separated by exactly one line feed (`\n`).  All RPC objects will be ignored until a `register` request is accepted by the daemon.

## Conventions

## Registration & Disconnection
Every connection must be registered before its requests will be honored.

### Request: `register`


### Notification: `disconnect`
