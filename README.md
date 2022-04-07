LibGimbal is a lightweight cross-platform framework used for creating low-overhead shared or dynamic libraries with an extensible C API.

BACKGROUND
==========
LibGimbal was born of me taking a long, hard look back at my work over the years, and finally identifying and conslidating all of the disjoint, duplicated, and nonstandardized core C code that tends to be reinvented for each C API that I have written. The name "libGimbal" is a play on the name "libGyro." Just as a gimbal can be used to orient and support a gyro it is the basic foundational support library used by libGyro and the other C APIs in the Elysian Shadows software ecosystem.

MAIN DESIGN GOALS AND PHILOSOPHIES
==================================
1) Deduplicate all of the boilerplate shit used by all my standalone C libraries
2) Provide an elegant, consistent, robust API that is uniform for all client libraries
3) Lightweight, with no external dependencies
4) C99-based codebase for universal support (except fucking Visual Studio)
5) Facilitate interop with other languages
	a. Object-oriented style that maps directly to C++ classes
		i. C structs intended to be extended/encapsulated by C++ wrappers
		ii. Callback and event handler mechanisms mappable to virtual functions
	b. Vanilla C API allows for binding to languages with C ABI support
6) Abstracts away any required OS/hardware/platform-specific logic 
	a. compile-time overridable for static polymorphism
	b. run-time overridable for dynamic polymorphism
7) Runtime Extensible Logic 
	a. overridable function pointers
	b. supports custom event types and handlers
	c. important structures may contain custom userdata pointers
8) Debugging/Validation Feature Flags
	a. powerful debugging features allowing 
		i. API validation/error handling
		ii. resource tracking
		iii. call stack capturing
	b. Individually configurable
		i. assertion behavior
		ii. log behavior
		iii. error/stack tracking
	c. Features compile away in release builds with no overhead
	d. Provides uniform API for client error checking and validation
		a. client automatically adheres to feature-flag configuration

FEATURE SET
===========
- Hardware/Device Abstraction Mechanism
	a. all common hardware/os/device-specific operations are customizable
	b. compile-time polymorphism and direct function calls for static libraries
	c. run-time polymorphism via custom function pointers for shared libraries
	d. run-time polymorphism via Event handler mechanism
- Advanced Logging Capabilities
	a. multi-level hierarchial log representation
	b. multiple filterable severity levels
	c. optional source-code context capturing 
- Custom allocators
	a. malloc, realloc, free are user-overridable
	b. allows for resource tracking/debugging and custom allocation schemes
	c. defaults to C standard library if custom allocators not specified
- Compile-time configurable/overridable built-in and user-types 
	a. standardized bool, int, size_t representations
	b. all internal void* userdata storage optionally typedefable to concrete type
- Robust Feature Flags 
	a. API validation checks
	b. assert vs logging on errors
	c. last error tracking
	d. source location tracking
- Common Library Utilities
	a. Macro utilities (stringification, concatenation, variadic overload helpers)
	b. Build Environment Definitions
		- supplied by CMake and/or external CI
		- compiler, OS, toolchain, Git branch, commit ID, build ID, etc
	c. API validation/utility macros 
		- error propagation/reporting, argument validation, asserts, control-flow, etc
	d. Command-line argument handlers (WIP)
	e. Compile-time/run-time version logging/checks
- Extendible Error Handling System
	a. provides extendible error code enum type
	b. provides extendible error string functionality
	c. feature-flags for enabling/disabling error checking
	d. feature-flags for error handling: assert, log, ignore, etc
- Centralized, nonstatic, extensible top-level API state struct "GimContext"
	a. allows for C++-style "inheritance" model to extend state data
	b. provides a generic event handler mechanism for extending functionality via runtime polymorphism
- Important structs maintain optional userdata pointers
	a. allows for adding client data to existing objects at runtime
	b. all userdata pointer types are optionally overridable at compile-time
- Client API Code Generation Utilities
	a. Skeleton code generated for common macros and overrides
	b. CMake project utilities
- Wrappers to other Languages
	a. Object-oriented C++ wrapper

WHAT'S THE POINT EXPLANATION
============================
1) exposing compiler/language specific functionality in a language-agnostic manner
2) macro meta programming utilities
3) API generation and validation utilities 
    * automatically generate macros for both C and C++ from a single meta macro definition
    * entire suite of unified API declarators and valadator macros for configurable customizable error handling
         a. handle errors as warnings, errors, log them, assert them, save them, return values, etc all configurable via Cmake
         b. enable/disable runtime API validations for debug builds
         c. unified stack tracing mechanism for API errors
         e. unified structured logging mechanism
         f. unified custom allocation mechanism
         g. unified variadic/generic function overloading mechanism (works in both C and C++)
4) Cross-language containers that work in both C and C++ simultaneously
    a. GblVector
    b. GblHashSet (and gimbal::HashMap in C++)
    c. GblTreeSet
    e. GblFlatSet (WIP)
    f. GblList singly and doubly linked lists
    * All are 100% STL compatible and complaint APIs with custom allocator support
5) Cross-language algorithms and utilities
   a. sorting algorithms (shell, quick, insertion, bubble, etc)
   c. hashing algorithms (Murmur, Sip, Jenkins, etc)
   e. Unified Rand() random number API with multiple seed support for custom generators and sharing seeds across runtime
   f. lots of quality of life common numeric algorithms like next power of 2, GCD, calculating minimal alignment/offset for allocations
   g. logging and capturing all build system and GIT commit shit for all projects in the build system
6) Custom Primitive Data Types
   a. GblString and the C++ gimbal::string equivalent which are std:;string/string_view compatible yet have stack allocation optimizations
   b. GblVariant as a type-erased generic value holder with C++-20-style std::variant API
   c. Custom error results: which have corresponding C and C++ types contain messages, error codes, and can be converted to and from C++ exceptions
   d. GblBuffer
7) Thread system with thread-local storage
8) Generic event-system
9) Runtime Extensible Object-Oriented Meta Type System
  a. Instances, Classes, Objects
  b. support for inheritance, virtual methods
  c. support for interfaces
  d. new user types can be registered to be compatible with the existing GblVariant type
  f. property system for Object types (still WIP)
10) Common Unit Test framework and Utilities
11) Generic Module system (WIP)

CREDITS
=======
Falco Girgis

USAGE
=====
TODO

API DOCUMENTATION
=================
TODO

LICENSE
=======
Do-whatever-the-fuck-you-want anti-communist freedom-loving 'Murican software license.
