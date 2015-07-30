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

### Receiver Types
In addition to any member of the ClientType enumeration, the following receivers are
defined for use in this document:

Name|Info
---|---
Server|A device which received a `register` request (this should be the device which built a TCP server)
Client|A device which made a connection by sending a `register` request (this should be the device which connected to an existing TCP server)

## Defined Types

### `DataLine` Type
Represents a single data line.  It is an array of objects where each object
represents a single column.  Column ordering is maintained.  Each column object
contains the following members:

Name|Info|Type
---|---|---
`Column`|The name of the column|string
`Data`|The actual data payload|string

### `Settings` Type

### `Config` Type

### `ClientType` Enumeration
Name|Value|Description
---|---|---
`Daemon`|0|A DDX daemon capable of executing paths
`Manager`|1|Usually a GUI instance
`Vertex`|2|A data responder or producer which does not run paths
`Listener`|3|A destination for loglines and alerts

### `PathState`	Enumeration
Name|Value|Description
---|---|---
`Vacant`|0|The path does not exist or has not been loaded
`Initializing`|1|The path is in the process of being initialized
`Ready`|2|The path is ready to be started
`Running`|3|The path is actively running
`Finished`|4|The path has finished processing all data (finite inlets only)
`Terminated`|5|A fatal error occurred that prevented the path from starting

## Global Errors
All errors explicitly specified by the JSON-RPC 2.0 specification can be used by
the DDX.  Some of these errors, such as parse error and invalid params, may include
more information in the `data` field.  The following server errors are also defined:

Code|Message|Meaning|Macro
---|---|---|---
-32000|Access denied|The client's ClientType is not sufficient for the request|E_ACCESS_DENIED
-32001|Parameters not an object|The param element is not a JSON object|E_PARAMETER_OBJECT
-32002|Not supported|The requested functionality is not supported by the server (e.g., path management on a GUI)|E_NOT_SUPPORTED

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
`DaylightSavingsEnabled`|Whether the client's timezone enables DST. _Note_: devices should ignore DST by default|bool
`Locale`|The client's locale|string

Result:

Name|Info|Type
---|---|---
`DDX_version`|The server's DDX version in the format "n.n"|string
`DDX_author`|The server's DDX author|string
`CID`|The server-given, client-taken connection ID; see "Connection IDs"|string
`Name`|The server's (usually) self-designated name|string
`Timezone`|The server's timezone as TZdb string|string
`DaylightSavingsEnabled`|Whether the server's timezone enables DST. _Note_: devices should ignore DST by default|bool
`Locale`|The server's locale|string

Upon connection to a daemon, actions will occur based on the connected client type:

- `Daemon`:  All listening modules will be notified that a new daemon has connected
- `Manager`:  The client will automatically start watching all opened paths and will
start receiving alerts (but not regular log lines)
- `Vertex`:  All listening modules will be notified that a new vertex has connected
- `Listener`:  The client will be registered to receive alerts (but not regular log lines)

Errors:

Code|Message|Macro
---|---|---
500|Server does not implement network communication (for future use)|E_NETWORK_DISABLED
501|Server is not compatible with the client version|E_VERSION_FORBIDDEN
502|Server does not allow external management|E_NO_OUTSIDE_MANAGEMENT
503|Address forbidden|E_ADDRESS_FORBIDDEN
504|Specified client type is explicitly forbidden|E_CLIENT_TYPE_FORBIDDEN
505|Version unreadable|E_VERSION_UNREADABLE


### Global notification: `disconnect`
- not this

## Path Management

### Daemon request: `listPaths`
### Daemon request: `openPath`
Initialize a given path.  It will be automatically watched.  Params:

Name|Info|Type
---|---|---
`Path`|The name of the path to be opened|string
`AutoStart`|Whether to start the path once it is ready|bool

Result bool will be true on success.  _Note_: this will be sent before the path
is ready for execution.  Users should wait for a `pathStateChanged` notification
to know when the path is ready for execution.

Errors:

Code|Message|Macro
---|---|---
200|Path does not exist|E_PATH_NONEXISTENT

### Daemon request: `startPath`
Start data processing on a given path.  The path will be initialized and then started
if it is not already open.  It will be automatically watched.  Params:

Name|Info|Type
---|---|---
`Path`|The name of the path to be started|string

Result bool will be true on success.

Errors:

Code|Message|Macro
---|---|---
200|Path does not exist|E_PATH_NONEXISTENT

### Daemon request: `stopPath`
Stop data processing on a given path.  Params:

Name|Info|Type
---|---|---
`Path`|The name of the path to be stopped.|string

Result bool will be true on success.

Errors:

Code|Message|Macro
---|---|---
200|Path does not exist|E_PATH_NONEXISTENT

### Daemon request: `testPath`
### Daemon request: `addPath`
### Daemon request: `modifyPath`
### Daemon request: `watchPath`
Start watching a path.  Watching a path means a client will receive its
`pathStateChanged` notifications and echoed lines.  To ignore a path,
send this with "false" for both Listen parameters.

Name|Info|Type
---|---|---
`Path`|The name of the path to be watched|string
`AllPaths`|If true, ignore the name and apply this to all running paths; defaults to false if omitted|bool
`StateListen`|Whether to listen to state changes|bool
`EchoListen`|Whether to listen to echoed lines|bool

Result bool will be true on success.

Errors:

Code|Message|Macro
---|---|---
200|Path does not exist|E_PATH_NONEXISTENT

### Daemon notification: `pathEcho`
When a path contains the Echo module, every data line it receives will be echoed
out to any watchers of the containing path.  The params of this notification will
contain the following:

Name|Info|Type
---|---|---
`Path`|The name of the path this is coming from|string
`Data`|The echoed data line|`DataLine`

### Daemon notification: `pathStateChanged`
Sent whenever a watched path changes state.  Params:

Name|Info|Type
---|---|---
`Path`|The name of the path|string
`State`|The path's new state|`PathState`

## Administration

### Global notification: `log`
Params:

Name|Info|Type
---|---|---
`Message`|The message|string
`Time`|Full time in "yyyy/MM/dd HH:mm:ss.zzz" format (in server's timezone)|string
`IsAlert`|Whether this is destined for the user or for logging only|bool

### Global request: `setLogFilters`
Params:

Name|Info|Type
---|---|---
`SendAlerts`|Whether sender should receive alerts|bool
`SendLogs`|Whether sender should receive non-alert loglines|bool

Result bool will be true on success.

### Global request: `listSettings`
List all of the program's settings.  Takes no parameters.

Result is an object with every setting listed inside.  If the setting's type cannot
be serialized into a string despite the use of base64-encoding and JSON, `CannotConvert`
will be true and the `Value` and `Default` parameters are undefined.  If both `IsJson`
and `IsBase64` are true, values must be JSON-encoded prior to being base64-encoded.
Each entry is an object with this format:

Name|Info|Type
---|---|---
[key]|Either just the name or `[group]/[name]` if it is in a group|string
`Description`|A brief description of the setting's purpose|string
`Value`|The current value, encoded as a string|string
`Default`|The default value, encoded as a string|string
`Type`|The type name returned by [`QMetaType::typeName`](http://doc.qt.io/qt-5/qmetatype.html#Type-enum)|string
`CannotConvert`|True if the type could not be serialized in any way. `Value` and `Default` should be ignored if true.|bool
`IsBase64`|Whether `Value` and `Default` are base-64 encoded; defaults to "false" if omitted|bool
`IsJson`|Whether `Value` and `Default` are encoded in JSON; defaults to "false" if omitted|bool

### Global request: `editSetting`
Currently, only settings which can be converted to and from strings or serialized
into binary are supported.  If both `IsJson` and `IsBase64` are true, value
is base64-decoded prior to being JSON-decoded.

Params:

Name|Info|Type
---|---|---
`Name`|The setting's name (case-sensitive)|string
`Group`|The setting's group (case-sensitive, can be empty)|string
`Value`|The setting's new value (case-sensitive)|string
`ShouldSave`|Whether the setting should be permanently saved; defaults to "true" if omitted|bool
`Reset`|If true, ignore the given value and reset it to default; defaults to "false" if ommitted|bool
`IsBase64`|Whether the value is base-64 encoded; defaults to "false" if omitted|bool
`IsJson`|Whether the value is encoded in JSON; defaults to "false" if omitted|bool

Result bool will be true on success.

Errors:

Code|Message|Macro
---|---|---
120|Setting does not exist|E_SETTING_NONEXISTENT
121|Setting could not be converted to target type|E_SETTING_CONVERT
122|Setting could not be base-64 or JSON decoded|E_SETTING_DECODE
123|Setting outside of requesting module|E_SETTING_DENIED
124|Setting reset requests must be saved|E_SETTING_SAVEREQUIRED

## Modules
Modules instantiated in opened paths can send their own requests and publish handlers
for specific notifications and requests.  Each of these has a bit of overhead built
onto it to help direct incoming objects.

### DataBeacon Module
