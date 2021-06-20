# LibGimbal
The Ultimate C17 and C++20 Cross-Language Runtime Framework

# TODO
## String
  - [ ] C
    - [ ] Leverage more GblVector API calls
  - [ ] C++
    - [ ] Ensure whole std::string() API is implemented
    - [ ] Generalized char* container/iterator compatibilty
    - [ ] gimbal::Vector<char> compatibility? 
  
## Containers 
### Vector 
  - [ ] C++
    - [ ] Add exceptions + error bounds checking
    - [ ] Clean up + move + leverage container/iterator-compatibility concepts
  - [ ] Testing
    - [ ] Test errors, out-of-bounds, bad insertions, exceptions, etc
    - [ ] Generalize + Leverage gimbal::test::String() 
      - Heap/Stack allocation checks, move semantics verification, allocator-support
    - [ ] Nontrivial C++ Object Type Testing
      - nontrivial destructors, move semantics, etc need to be validated 
    - FlexibleVector C/++ static stack storage validation (both sides)
### Map 
  - two-way tree-based associative container backed by a vector/array 
  - full C++ std::map type compatibilty 
  - Use C stdlib bsp qsort/find functions 
 ### Generalized Iterators 
  - Or is that better modeled by leveraging GblObject mechanism?

- [ ] Command Line Parser 
  - [ ] C++
    - Whole fucking API
  - [ ] Testing
    - All Unit Tests

- [ ] Variant
  - [ ] C 
    - [ ] Dynamic user MetaType support 
    - [ ] Mechanism for user-added API operator overloads
    - [ ] Variant v Variant string comparisons seem sketchy
  - [ ] C++
    - [ ] std::variant, std::any, std::optional<T> support (use nil)
  - [ ] Testing
    - Make sure heap string lifetime/allocation shit is bulletproof

- [ ] Object 
  - [ ] C 
    - [ ] Implement GblMetaObject back-end + API 
    - [ ] Public API 
      - MetaObject
      - Property Management
      - Virtual Methods
        - Derived objects with overridden vtable 
        - Virtual method invocation mechanism 
        - Call into base implementation from derived 
  - [ ] C++
    - Subclassing + virtual overloading 
    - Calling into overridden base class virtual function implementation 
  - [ ] Testing 
    - C/++ interop 
    - Variant compatibility 

- [ ] Handle 
  - Event handler override? 
  - API override? 
  - Subclass Object 

- [ ] Context 
  - [ ] API
    - Event routing/management 
    - Custom client GBL_RESULT registry
    - Custom client MetaType registry 
    - Option to enable/disable debug/config variables at runtime based on environment variables? (getenv())
    - C/++ view/object allocation/shit finalization
  - [ ] Testing 
    - Generalize gimbal::test::Context to use Handle types where applicable  
    - Generalize test Context API validation framework for client APIs to leverage 
      - Abstract generic stack/guard shit away into gimbal_monitor.hpp
    - ACTUALLY VERIFY SOURCE CODE CAPTURING 

- [ ] gimbal_macros_map.hpp
  - Clean up, organize shit by property/enum types and shit 
  - Make sure naming scheme is standardized 

- [ ] gimbal_test.h 
  - Generic C-style unit test API 

- [ ] gimbal_config.h.in 
  - typedef-ing common values for client APIs (GblSize -> EvmuSize, GblEnum -> EvmuEnum)
  - UserExt compile-time function names 

- [ ] Task title ~3d #type @name yyyy-mm-dd  
  - [ ] Sub-task or description  

### Completed Column ✓
- [x] Completed task title  
