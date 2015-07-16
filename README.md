# 2B Technologies DDX

The DDX is a general purpose data stream management framework developed by 2B Technologies.  It operates on streams producing "data lines" with live-updating formats.  It is designed to allow a wide variety of data operations (processing, filtering, relaying, logging, etc.) which can be dynamically configured and applied to a data stream.  It is robust enough for the same codebase to be utilized across an entire data ecosystem but flexible enough to run on an embedded Linux RTOS.  Its backbone is a management daemon, but a default GUI is also in the works.

## Module Design
An individual data processing stream is called a "path."  Paths are linear strings of "modules," which can be written to do a wide range of tasks.  Modules are written in C++ and directly built into the DDX, allowing them to be highly optimized.  The first module in every path is a particular kind of module called an "inlet."  Inlets are responsible for loosely describing the data and producing it.  Paths and their constituent modules are dynamically instantiated via JSON configuration documents, called "schemes."  Each module can publish pre-initialization settings as well as live actions, which can be updated as data changes.  Live actions can be triggered by arbitrary network devices, an attached DDX GUI, or other modules and paths in the same DDX daemon instance.  Modules can safely call each other's methods in most cases, allowing communication about data across any point in the unidirectional data streams.

## GPL Licensing
Because the bulk of the DDX's power lies in its modules, open-sourcing it was a natural decision.  Anyone can write modules which suit their needs or integrate other modules into their own DDX builds with ease.  

## Connectivity
The DDX is based on the Qt framework, which contains an array of connectivity solutions.  Inlets can be written to receive data from serial ports and emulators, the Internet, databases, other DDX daemon instances, and much more.  To simplify data management, inlets are read-only with respect to other modules, but modules in a path can write back to the originating devices by calling functions published by inlets.

## Path Durability
Paths are meant to operate their primary functions whenever possible, even if a particular module loses functionality.  As such, module design guidelines and restrictions encourage developers to make modules which report fatal errors and potentially trigger additional data saving systems but do not halt data flow.  This keeps data loss due to non-essential module failure to a minimum.

## Official Modules
2B Tech is working on a number of official modules for general purpose data management.  These will be available soon.