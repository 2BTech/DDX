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
The DDX uses strictly-compliant [JSON-RPC 2.0](http://www.jsonrpc.org/specification). 
At the low level, each DDX daemon opens a TCP server, by default on port 4388, to 
which GUIs, other DDX daemons, and other data sources or sinks can connect.  Every 
RPC object transmitted must be separated by exactly one line feed (ASCII 10).  All 
text must be encoded in UTF-8.  SSL support is planned but not currently in development. 
All RPC objects from a particular client will be ignored until a corresponding 
`register` request is accepted by the daemon.

<!--[TOC]-->

## Conventions
This document displays the contents of JSON-RPC objects in a simplified way. 
Every transmittable object begins with a header which identifies which device can
receive it, whether it is a request or notification, and its RPC `method` name. 
For example, the header "Server request: `register`" indicates that the object 
definition which follows represents to an RPC request object which can only be 
sent to a server and will have the method name `register`.  A request or notification 
that is marked "global" can be received by any device.  Object descriptions usually 
include parameter tables which list the information included in the object as a param 
or result.  Parameter tables must be represented as JSON objects.  Some objects will 
document a different type of param/result, although developers of additional DDX 
RPC commands should use this only where they are sure future versions are unlikely 
to take additional parameters.  Requests have two parameter sets, one titled 
"Params" and one titled "Result".  These correspond to the "params" element of a 
request object and the "result" element of that request's response object.  Requests 
also have an errors table, which enumerates all the errors which may be returned. 
See the "Global Errors" section for infomation on global errors and the error 
handling of notifications.

The `jsonrpc` and `id` members of objects are implied.

## Defined Types
- Enums - stored as string, list them here
- "settings" is a type
- "config" is a type (opposite of settings)

## Global Errors
TBD

## Registration & Disconnection

### Server request: `register`
Every connection must be registered before its requests will be honored.

Params:

Name|Info|Type
---|---|---
`DDX_version`|The client's DDX version in the format "n.n"|string
`DDX_author`|The client's DDX author|string
`CID`|The client-given, server-taken connection ID; see "Connection IDs"|string
`ClientType`|The client's type|ClientType
`Name`|The client's (usually) self-designated name|string
`Timezone`|The client's timezone as TZdb string|string
`Locale`|The client's locale|string

Result:

Name|Info|Type
---|---|---
`DDX_version`|The server's DDX version in the format "n.n"|string
`DDX_author`|The server's DDX author|string
`CID`|The server-given, client-taken connection ID; see "Connection IDs"|string
`Name`|The server's (usually) self-designated name|string
`Timezone`|The server's timezone as TZdb string|string
`Locale`|The server's locale|string

Errors:

Code|Message|Macro
---|---|---
500|Server does not implent network communication (for future use)|E_NETWORK_DISABLED
501|Server is not compatible with the client version|E_VERSION_FORBIDDEN
502|Server does not allow external management (overrides ADDRESS_FORBIDDEN)|E_NO_OUTSIDE_MANAGEMENT
503|Address forbidden|E_ADDRESS_FORBIDDEN
504|Specified client type is explicitly forbidden|E_CLIENT_TYPE_FORBIDDEN



### Server notification: `disconnect`


## Path Management

## Administration

### Global request: `editSetting`

Errors:

Code|Message|Macro
---|---|---
100|Access denied|E_ACCESS_DENIED
