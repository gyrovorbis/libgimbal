<p align="left"><img src="http://libgimbal.elysianshadows.com/libgimbal_icon.png" alt="libGimbal">

# libGimbal #
LibGimbal is a cross-platform, general-purpose core applications framework written in C17 with C++20 bindings, inspired by GLib, QtCore, and the Objective-C Foundation framework. It is composed of a series of modules providing a core application layer, specialized string types, generic STL-style containers, sorting and searching algorithms, custom allocators, preprocessor metaprogramming constructs, miscellaneous utility structures, and a full-fledged dynamic type system bringing object-oriented constructs such as classes, signals, and properties to C and bindable languages. Additionally, it includes a C-based OO unit-testing framework built upon its type system and object model.

LibGimbal originated as the core of the Elysian Shadows tech stack, and as such supports a myriad of embedded devices and video game consoles such as the Sega Dreamcast and Sony PSP. It places a strong emphasis on performance and language interoperability, with the core being written in standard C17 for easily binding to languages such as Rust, Swift, Go, Lua, and any other C-compatible language. The library is being currently being tested and validated via the included suite of unit tests for Windows, MacOS, Linux, Android, iOS, Sega Dreamcast, Sony PSVita, and WebAssembly targets. 

## Documentation ##
The official documentation for the project can be found at http://libgimbal.elysianshadows.com.

## Features ##
### Core Framework ###
- platform-specific macros and utilities
- error handling and propagation
- debug and verification utilities
- structured logging

### Strings ###
- specialized, optimized custom string types
- interned strings
- reference counted strings
- string builders
- implicitly shared string lists
- pattern matching

### Containers ###
- STL-style abstract data types with both C and C++ APIs
- dynamic arrays, lists, ring buffers
- sets, trees, hash tables, byte arrays
- support for custom allocators

### Algorithms ###
- hashing algorithms (Murmur, Sip, Fnv1, SuperFast)
- sorting algorithms, searching algorithms
- random data generators and utilities
- CRC, parity, LCM, GCD, prime numbers

### Allocators ###
- allocation trackers with leak detection
- paged arena allocators with state rewinding
- stack-based scope allocator with destructor chains
- efficient pool-based, arena-backed allocator

### Preprocessor ###
- function overloading support for C
- macro compositon, variadic argument handling
- cross-language C/C++ generic selection
- meta macro programming constructs

### Meta Type System ###
- cross-language, extensible object-oriented type system
- generic, polymorphic variant types with type conversions
- virtual functions, inheritance, private members for C
- mixins, interfaces, abstract classes
- generic FFI with cross-language closures and marshals
- property system
- Qt/GTk style signal system
- object hierarchy management, parenting, traversals
- event handling, filtering, propagation
- plugin and module system

### Utilities ###
- generic reference-counted types
- command-line option and argument parsing
- version, UUID, date-time utilities

### Unit Testing Framework ###
- cross-platform object-oriented C-based unit test framework
- test scenarios, suites, cases
- generic verification macros with source context capture
- automatic timing and performance measurements
- memory utilization and leak detection
- dynamic/run-time test-plan construction and management
