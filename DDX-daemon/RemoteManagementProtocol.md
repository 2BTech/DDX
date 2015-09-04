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

# DDX Remote Management Protocol (DDX-RPC)
DDX-RPC uses strictly-compliant [JSON-RPC 2.0](http://www.jsonrpc.org/specification).
The only exceptions to the JSON-RPC protocol are as follows:

- Batch requests are not currently supported
- The `params` element in requests must be an object
- JSON _must_ be sent in compacted form because line feed characters (ASCII 10) are
prohibited within RPC items

All text must be encoded in UTF-8.  All DDX-RPC objects from a particular client will
be ignored until a corresponding `register` request is accepted by the server.

## Link Agnosticism
For the most part, DDX-RPC can be implemented on any reliable and persistent bidirectional
data transmission system.  So far, the DDX daemon only implements a TCP/TLS link.  An
HTTP/HTTPS link may eventually replace it to help with firewall issues.  Furthermore,
SCTP may provide a more reliable (although less available) solution than TCP.

### TCP/TLS Link
At the low level, each DDX daemon opens a TCP server, by default on port 4388, to
which GUIs, other DDX daemons, and other data sources or sinks can connect.  Every
RPC object transmitted must be separated by exactly one line feed (ASCII 10).  TLS
is applied to connections according to certain rules.


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
document a different type of param/result, although developers of additional DDX-RPC
commands should use this only where they are sure future versions are unlikely 
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

### `DataLine` Type
Represents a single data line.  It is an array of objects where each object
represents a single column.  Column ordering is maintained.  Each column object
contains the following members:

Name|Info|Type
---|---|---
`Column`|The name of the column|string
`Data`|The actual data payload|string
`IsBase64`|Whether the `Data` member is base-64 encoded; defaults to false if omitted|bool

### `Settings` Type

### `Config` Type

### Temporal Types
Type|Description|Format
---|---|---
`UtcTime`|The [ISO 8601](http://www.iso.org/iso/catalogue_detail?csnumber=40874) format for UTC|`YYYY-MM-DDTHH:mm:ssZ`
`Timezone`|Timezone as an IANA TZdb string or UTC offset|"America/Denver", "UTC-07:00"

### `DeviceRole` Flags
Name|Value|Description
---|---|---
`Daemon`|0x1|Can execute paths
`Manager`|0x2|An interface for a device which executes paths
`Vertex`|0x4|A data responder or producer which does not execute paths
`Listener`|0x8|A destination for loglines and alerts
`Global`|0x80|A pseudo-role which indicates role-less information

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

Code|Message|Macro|Data
---|---|---|---
-32000|Access denied|E_ACCESS_DENIED|
-32001|Invalid response|E_INVALID_RESPONSE|The id of the invalid response
-32002|Not supported|E_NOT_SUPPORTED|
-32003|Batch not supported|E_NO_BATCH|
-32004|Request timed out|E_REQUEST_TIMEOUT|
-32005|Encryption required|E_ENCRYPTION_REQUIRED|
-32006|Device disconnected|E_DEVICE_DISCONNECTED|`DisconnectReason`

Note that the "Request timed out" error is reserved for sending by the DDX-RPC
implementation on the device from which the request was sent.  When a request is
sent, the DDX-RPC implementation marks down the time.  If no corresponding response is
received within a given time, the DDX-RPC implementation should send a simulated
response error with the code -32005.

The "Device disconnected" error will also be sent by the DDX-RPC implementation.

If the requested/notified method is unavailable, the "Method not found" error will be sent
with the requested method name in the `data` element.  Requests will use the original ID
and notifications will use null.

## Registration & Disconnection
Registration is a required handshake that allows connecting devices to understand
each other's environment, capabilities, identity, and reason for connecting.  Both devices
must send a `register` request to the other, both of which need to complete successfully
before messages will be passed.

#### Requester vs. Target, Inbound vs. Outbound
Within the context of registration, a "requester" is the device that builds a connection to
a "target".  Connections are known to their target as "inbound" and to their requester as
"outbound".

### TCP Devices & TLS
It is highly recommended that TCP implementations of the DDX-RPC encrypt their connection.  Encryption
is accomplished with TLS v1.2.  Note that TLS is used only to encrypt the connection; identity verification
through TLS is currently not supported.  Encryption status must be determined before any incoming
data will be delivered to the JSON parser.  Each device can determine encryption status per connection
at the time the connection is made.  Each TCP device must then establish encryption status
by sending the phrase `encryption:[status]` followed by a line break character (ASCII 10).
`status` can be one of the following:

Status|Meaning
---|---
`disabled`|The device does not support encryption
`enabled`|The device allows encryption but is not requesting it
`requested`|The device allows encryption and is requesting it, but does not require it
`required`|The device allows and requires encryption

Whether encryption is to be used on the connection is then chosen based on the strictest possible
level of the two devices.  If one device requires encryption but the other does not support it,
the device which requires it will disconnect with `EncryptionRequired`.  If encryption is not used,
both devices will then immediately start accepting JSON and registration can begin.  If encryption
is used, the requester will then begin sending TLS handshakes until successful and the target will
wait until a handshake succeeds.  Both devices will start accepting JSON immediately after a
handshake succeeds.  N

By default, encryption is `required` on external connections but `enabled` on connections through localhost.

### Passwords
DDX-RPC implementations can require a distinct password for each individual role in addition
to a global password.  Every required password must be listed in the `register` request
for it to succeed.

### Server request: `register`
Every connection must be registered before its requests will be honored.  Any requests or
notifications sent prior to successful registration will be ignored without error.  _Note:_
`register` requests can **NOT** be a part of a JSON-RPC batch request.

Params:

Name|Info|Type
---|---|---
`DDX_version`|The client's DDX version in the format "n.n"|string
`DDX_author`|The client's DDX author|string
`CID`|The client-given, server-taken connection ID; see "Connection IDs"|string
`Roles`|The roles which this client fills|`DeviceRole`
`Requester`|The requester ID|string
`Target`|The target ID|string
`RequesterInfo`|Information about the requester which the target can use to determine whether to accept the connection or not (null if coming from the client)|any
`Passwords`|An array of strings containing any passwords that are required|array\<string>
`Name`|The client's (usually) self-designated name|string
`Timezone`|The client's timezone|`Timezone`
`Locale`|The client's locale|string

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
502|Server does not allow external connections|E_NO_EXTERNAL_CONNECTIONS
503|Address forbidden|E_ADDRESS_FORBIDDEN
504|A specified client role is explicitly forbidden|E_CLIENT_TYPE_FORBIDDEN
505|Version unreadable|E_VERSION_UNREADABLE
506|Password invalid|E_PASSWORD_INVALID
507|Target invalid|E_TARGET_INVALID

Error E_PASSWORD_INVALID should contain a `DeviceRole` flag integer which specifies any
roles which were denied because of invalid passwords.

### Global notification: `disconnect`
This notification is usually sent immediately before a device terminates a connection.
However, many cases for disconnection do not allow time for this notification to be
sent, in which case the socket will disconnect without sending anything.

Params:

Name|Info|Type
---|---|---
`Reason`|The reason for disconnection|`DisconnectReason`

`DisconnectReason` Enumeration:

Name|Value|Description
---|---|---
`UnknownReason`|0|Unknown disconnection
`ShuttingDown`|1|The disconnecting member is shutting down by request
`Restarting`|2|The disconnecting member is restarting and will be back shortly
`FatalError`|3|The disconnecting member experienced a fatal error and is shutting down
`ConnectionTerminated`|4|The connection was explicitly terminated
`RegistrationTimeout`|5|The connection was alive too long without registering
`BufferOverflow`|6|The connection sent an object too long to be handled
`StreamClosed`|7|The stream was closed by its low-level handler
`EncryptionRequired`|8|Encryption is required on this connection

## Path Management

### Daemon request: `path.list`
### Daemon request: `path.open`
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

### Daemon request: `path.start`
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

### Daemon request: `path.stop`
Stop data processing on a given path.  Params:

Name|Info|Type
---|---|---
`Path`|The name of the path to be stopped.|string

Result bool will be true on success.

Errors:

Code|Message|Macro
---|---|---
200|Path does not exist|E_PATH_NONEXISTENT

### Daemon request: `path.test`
### Daemon request: `path.add`
### Daemon request: `path.modify`
### Daemon request: `path.watch`
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

### Daemon notification: `path.echo`
When a path contains the Echo module, every data line it receives will be echoed
out to any watchers of the containing path.  The params of this notification will
contain the following:

Name|Info|Type
---|---|---
`Path`|The name of the path this is coming from|string
`Data`|The echoed data line|`DataLine`

### Daemon notification: `path.stateChanged`
Sent whenever a watched path changes state.  Params:

Name|Info|Type
---|---|---
`Path`|The name of the path|string
`State`|The path's new state|`PathState`

## Administration

### Listener notification: `log`
Params:

Name|Info|Type
---|---|---
`Message`|The message|string
`Time`|The time the message was sent|`UtcTime`
`IsAlert`|Whether this is destined for the user or for logging only|bool

### Listener request: `log.setFilters`
Params:

Name|Info|Type
---|---|---
`SendAlerts`|Whether sender should receive alerts|bool
`SendLogs`|Whether sender should receive non-alert loglines|bool

Result bool will be true on success.

### Global request: `settings.list`
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

### Global request: `settings.edit`
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

## Appendix

### Non-RPC Errors
These are errors which are not part of the DDX-RPC protocol but are included here
for auto-generation convenience and because they may at some point be a part of the RPC.

Code|Message|Macro
---|---|---
10|DDX settings are for higher version or corrupted|E_SETTINGS_VERSION
20|A TCP server could not be established|E_TCP_SERVER_FAILED
