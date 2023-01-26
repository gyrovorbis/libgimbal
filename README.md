# libGimbal #
LibGimbal is a cross-platform, general-purpose core applications framework written in C17 with C++20 bindings, inspired by GLib, QtCore, and the Objective-C Foundation framework. It originated as the core of the Elysian Shadows tech stack, and as such supports a myriad of embedded devices and video game consoles such as the Sega Dreamcast and Sony PSP. It places a strong emphasis on performance and language interoperability, with the core being written in C for easily binding to languages such as Rust, Swift, Go, Lua, and any other C-compatible language.

## Documentation ##
The official documentation for the project can be found at http://libgimbal.elysianshadows.com.

## Features ##
### Core Framework ###
- platform-specific macros and utilities
- error handling and propagation
- debug and verification utilities
- structured logging
- custom allocator support
- basic data types

### Preprocessor ###
- commonly-used utility macros
- function overloading support for C
- macro compositon, variadic argument handling
- cross-language C/C++ generic selection
- eval, tuples, maps, lists, meta macro programming constructs

### Containers ###
- abstract data types with both C and C++ support
- Type-compatible replacement STL containers for C++
- STL-like containers and APIs for C
- support for custom allocators
- dynamic arrays, lists, ring buffers
- sets, trees, hash tables, byte arrays

### Algorithms ###
- hashing algorithms (Murmur, Sip, Fnv1)
- sorting algorithms, searching algorithms
- random number generation API
- CRC, parity, LCM, GCD, prime numbers

### Allocators ###
- allocation trackers with leak detection
- paged arena allocators with state rewinding
- stack-based scope allocator with destructor chains
- efficient pool-based, arena-backed allocator

### Meta Type System ###
- generic, polymorphic variant types with type conversions
- cross-language, extensible object-oriented type system
- virtual functions, inheritance, private members for C
- mixins, interfaces, abstract classes, dynamic polymorphism
- generic FFI with cross-language closures and marshals
- generic, data-oriented property system
- Qt-style generic, event-driven signal system
- object hierarchy management, parenting, traversals
- event handling, filtering, propagation
- plugin and module system

### Utilities ###
- generic reference-counted containers
- command-line option and argument parsing
- version parsing, packing, checking
- UUIDv4 generation and manipulation
- date-time parsing, formatting, processing

### Unit Testing Framework ###
- cross-platform object-oriented C-based unit test framework
- test scenarios, suites, cases
- generic verification macros with source context capture
- automatic timing and performance measurements
- memory utilization and leak detection
- support for dynamic/run-time test plan construction
