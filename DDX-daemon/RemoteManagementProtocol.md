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
request object and the "result" element of that request's response object.  All
parameters and results are required unless otherwise stated.  Requests 
also have an errors table, which enumerates all the errors which may be returned. 
See the "Global Errors" section for infomation on global errors and the error 
handling of notifications.  The `jsonrpc` and `id` members of objects are implied.
Messages are written in English in this document, but may be translated.  See the
"Internationalization" section.

## Defined Types

### The Settings Type

### The Config Type

### Enumerations

#### `ClientType`
Name|Value|Description
---|---|---
`Manager`|1|Usually a GUI instance
`DataVertex`|2|A data responder or producer
`Listener`|3|A destination for loglines and alerts


## Global Errors
All errors explicitly specified by the JSON-RPC 2.0 specification can be used by
the DDX.  Some of these errors, such as parse error and invalid params, may include
more information in the `data` field.  The following server errors are also defined:

Code|Message|Macro
---|---|---
-32000|Access denied|E_ACCESS_DENIED
-32001|Parameters not stored in an object|E_PARAMETER_ARRAY

## Registration & Disconnection

### Server request: `register`
Every connection must be registered before its requests will be honored.

Params:

Name|Info|Type
---|---|---
`DDX_version`|The client's DDX version in the format "n.n"|string
`DDX_author`|The client's DDX author|string
`CID`|The client-given, server-taken connection ID; see "Connection IDs"|string
`ClientType`|The client's type|`ClientType`
`Name`|The client's (usually) self-designated name|string
`Timezone`|The client's timezone as TZdb string|string
`Locale`|The client's locale|string
`

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
500|Server does not implement network communication (for future use)|E_NETWORK_DISABLED
501|Server is not compatible with the client version|E_VERSION_FORBIDDEN
502|Server does not allow external management (overrides E_ADDRESS_FORBIDDEN)|E_NO_OUTSIDE_MANAGEMENT
503|Address forbidden|E_ADDRESS_FORBIDDEN
504|Specified client type is explicitly forbidden|E_CLIENT_TYPE_FORBIDDEN


### Server notification: `disconnect`


## Path Management

## Administration

### Global notification: `log`
Params:

Name|Info|Type
---|---|---
`Message`|The message|string
`Time`|Full time in "yyyy/MM/dd HH:mm:ss.zzz" format|string
`IsAlert`|Whether this is destined for the user or for logging only|bool

### Global request: `setLogFilters`
Params:

Name|Info|Type
---|---|---
`SendAlerts`|Whether sender should receive alerts|bool
`SendLogs`|Whether sender should receive non-alert loglines|bool

Result bool will be true on success.

### Global request: `editSetting`
Currently, only settings which can be converted to and from strings or serialized
into binary are supported.  If both `IsJson` and `IsBase64` are true, value
is base64-decoded prior to being JSON-decoded.

Params:

Name|Info|Type
---|---|---
`Name`|The setting's name (case-sensitive)|string
`Group`|The setting's group (case-sensitive)|string
`Value`|The setting's new value (case-sensitive)|string
`ShouldSave`|Whether the setting should be permanently saved; defaults to "true" if omitted|bool
`Default`|If true, ignore the given value and reset it to default; defaults to "false" if ommitted|bool
`IsBase64`|Whether the value is base-64 encoded; defaults to "false" if omitted|bool
`IsJson`|Whether the value is encoded in JSON; defaults to "false" if omitted|bool

Result bool will be true on success.

Errors:

Code|Message|Macro
---|---|---
120|Setting does not exist|E_SETTING_NONEXISTENT
121|Setting could not be converted to target type|E_SETTING_NOCONVERT
122|Setting could not be base-64 or JSON decoded|E_SETTING_NODECODE
123|Setting outside of requesting module|E_SETTING_DENIED
124|Setting reset requests must be saved|E_SETTING_SAVEREQUIRED
