
<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/gyrovorbis/libgimbal">
    <img src="http://libgimbal.elysianshadows.com/libgimbal_icon.png" alt="libGimbal">
  </a>

  <h3 align="center">libGimbal</h3>

  <p align="center">
    C17-Based Extended Standard Library and Cross-Language Runtime Framework
    <br />
    <a href="http://libgimbal.elysianshadows.com"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    Cross-Platform
    ·
    Cross-Language
    ·
    High Performance
  </p>
</div>

## Features ##
- Generic Containers 
- Generic Algorithms 
- Custom Allocators
- Specialized Strings 
- Standard Library Utilities
- Preprocessor Programming
- Dynamic Type System 
- Unit Test Framework

## Platforms ##
- Windows 
- MacOS 
- Linux 
- Sega Dreamcast
- Sony PSVita
- WebAssembly
- iOS
- Android

## Compilers ##
- Microsoft Visual Studio
- GCC
- Clang
- MinGW-w64
- Emscripten

## Modules ##
### Core Framework ###
- platform-specific macros and utilities
- error and exception handling
- debug and verification utilities
- structured logging with filtering
- object-oriented threading and concurrency model

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

## Building ##
Building is done with standard CMake, and all generators and mainstream compilers should be supported. You should be able to open CMakeLists.txt directly as a project file in most IDEs such as MSVC, XCode, Qt Creator, CLion, etc if you wish to build from a UI.

To build the project and its unit tests from the command-line, you can do the following:
```
mkdir build
cd build
cmake -DGBL_ENABLE_TESTS=ON ..
cmake --build . 
```

## Dependencies ##
  - Original HashSet and BTree back-ends: Josh Baker (tidwall) 
    - https://github.com/tidwall/hashmap.c
    - https://github.com/tidwall/btree.c
  - Original TinyCThreads: Marcus Geelnard, Evan Nemerson
    - https://tinycthread.github.io/
  - SipHash: Jean-Philippe Amasson, Daniel J. Bernstein 
    - http://creativecommons.org/publicdomain/zero/1.0/
  - MurmurHash3: Austin Appleby
  - Original strptime: Based on musl C Standard Library Implementation
    - https://git.musl-libc.org/cgit/musl/tree/src/time/strptime.c
  - GblPattern engine back-end: tiny-regex-c 
    - https://github.com/kokke/tiny-regex-c

## Bindings ##
LibGimbal was specifically written with language interoperability in mind. As such, we will be happy to support and feature bindings to your favorite language. The following is a list of libraries which offer bindings between libGimbal and other languages:

|Library                                                                         | Language |
|--------------------------------------------------------------------------------|----------|
|<a href="https://github.com/AaronGlazer/jimbal"><strong>Jimbal</strong></a>     |Jai       |
