#ifndef GIMBAL_INSTANCE_H
#define GIMBAL_INSTANCE_H

#include "gimbal_type.h"

#ifdef __cplusplus
extern "C" {
#endif


// requirements
// 1. extendible state data
// 2. parent class/object/hierarchy
// 3. virtual dispatcher
// 4. parent instance? (implement inheritance + callback chaining + COM interface forwarding

#if 0
Given a GblHandle, which "C Inherits" from GblObject:
typedef struct GblHandle {
    GblContextObject        base;
    void*                   pUserdata;
    GblHandleAllocatorFn    pFnAllocator;
    GblContext              hContext;
} GblHandle;

Given a gimbal::Handle which is the C++ extension/wrapper which inherits and adds to GblHandle:
class gimbal::Handle: public GblHandle {
    // add some shit
    int cppValue;
};
Here is the resulting memory layout:


                                                               +---> GblHandleMetaObject
           +--                     +-----------------------+   |
           |          GblObject    |pMetaObject            |---+ points to static type info
Static Type|                       |pMetaInstance          |---+ points to local instance info
      Data |                       +-----------------------+   |
           |          GblHandle    |pUserData              |   |
           |                       |pFnAllocator           |   |
           +--                     |hContext               |   |
Extra User |                       +-----------------------+   |
      Data |    gimbal::Handle     |pThisPtr               |   |
           |                       |cppValue               |   |
           + --                    +-----------------------+   |
           |    GblMetaInstance    |sizeOf                 | <-+
           |      (local copy)     |alignOf                |
 Dynamic   |                       |. . . . . . . . . . . .|
    Type   |    GblDispatcher      |vTableSize             |
  Metadata |                       |pVTable                |---+ points to local
           |                       +-----------------------+   | vtable copy
           |    VTable             |vtable[0]              |<--+
           |      (local copy)     |vtable[1]              |
           |                       |vtable[2]              |
           + --                    +-----------------------+

Total size = static data size + extra user data size + dynamic type metadata size


#endif

    typedef struct GblMetaAllocation {
          GblSize             sizeOf;
          GblSize             alignOf;
    } GblMetaAllocation;


  typedef struct GblMetaInstance {
        GblRefCount             refCount;
        GblDispatcher*          pDispatcher;
        GblSize                 parentCount;
        GblMetaInstance**       ppParents;
        struct {
            GblMetaAllocation   publicAlloc;
            GblMetaAllocation   privateAlloc;
        }                       stateData;
    } GblMetaInstance;

    typedef struct GblMetaObject {

    };

  typedef struct GblMetaType {
        const char*                 pTypeName;
        GblType                     typeId;
        GBL_TYPE_FLAGS              flags;
      const GblMetaObject*        pMetaObject;
      GblMetaInstance             metaInstance;
  } GblMetaType;


typedef struct GblMetaInstance {


};


typedef struct GblInstance {
    GblMetaInstance* pMetaInstance;

};




#ifdef __cplusplus
}
#endif

#endif // GIMBAL_INSTANCE_H
