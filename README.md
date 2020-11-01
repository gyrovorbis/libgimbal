
LibGimbal is a lightweight cross-platform framework used for creating low-overhead shared or dynamic libraries with an extendible C API.

LibGimbal was born of me taking a long-hard look at my work over the years, and finally conslidating all of the disjoint, duplicated, and nonstandardized core C code that tends to be reinvented for each C API that I have written. The name "libGimbal" is meant as a reference to "LibGyro"--as it is the basic foundational support library used by it and the other C APIs in the Elysian Shadows software ecosystem.

The main design goals are as such:
1) Deduplicate all of the boilerplate shit used by all standalone C libraries
2) Provide an elegant, consistent, robust API that is uniform for all client libraries
3) Lightweight, with no external dependencies
4) C99 to compile everywhere
5) API designed to facilitate interop with other languages
	a. Object-oriented style that maps directly to C++ classes
		i. C structs intended to be extended/encapsulated by C++ wrappers
		ii. Callback and event handler mechanisms mappable to virtual functions
	b. Vanilla C API allows for binding to languages with C ABI support
6) Abstracts away any required OS/hardware/platform-specific logic 
	a. compile-time overridable for static polymorphism
	b. run-time overridable for dynamic polymorphism
7) Runtime Extendible 
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

Features:
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