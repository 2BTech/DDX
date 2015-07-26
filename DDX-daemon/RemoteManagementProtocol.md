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
The DDX uses [JSON-RPC 2.0](http://www.jsonrpc.org/specification).  At the low level, each DDX daemon opens a TCP server, by default on port 4388, to which GUIs, other DDX daemons, and other data sources or sinks can connect.  Every RPC object transmitted must be separated by exactly one line feed (`\n`).  All text is encoded in UTF-8.  All RPC objects will be ignored until a `register` request is accepted by the daemon.

<!--[TOC]-->

## Conventions

## Registration & Disconnection
Every connection must be registered before its requests will be honored.

### Server request: `register`
Params:

Name|Info|Type
---|---|---
`DDX_version` | The client's DDX version in the format "n.n" | string
`DDX_author` | The client's DDX author | string
`ClientType` | The client's type; see ClientTypes | string
`Name` | The client's (usually) self-designated name | string
`Timezone` | The client's timezone as TZdb string | string

Response params:

Name | Info | Type
--- | --- | ---
`DDX_version`|The server's DDX version in the format "n.n"|string
`DDX_author`|The server's DDX author|string
`Name`|The server's (usually) self-designated name|string
`Timezone`|The server's timezone as TZdb string|string


### Server notification: `disconnect`
