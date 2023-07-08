/*! \file
 *  \brief Containers module-wide include
 *  \ingroup containers
 *
 *  \author Falco Girgis
 */


#ifndef GIMBAL_CONTAINERS_H
#define GIMBAL_CONTAINERS_H

#include "containers/gimbal_array_deque.h"
#include "containers/gimbal_array_heap.h"
#include "containers/gimbal_array_list.h"
#include "containers/gimbal_array_map.h"
#include "containers/gimbal_doubly_linked_list.h"
#include "containers/gimbal_hash_set.h"
#include "containers/gimbal_linked_list.h"
#include "containers/gimbal_nary_tree.h"
#include "containers/gimbal_ring_buffer.h"
#include "containers/gimbal_ring_list.h"
#include "containers/gimbal_tree_set.h"

/*! \defgroup containers Containers
 * 	\brief Collection of abstract container types providing C++ STL-style APIs

    LibGimbal provides a rich set of abstract containers for various use-cases. Their naming schemes are such that the underlying data
    structure is the prefix an and the API type is the suffix. For example, a GblTreeSet is a Tree-based data structure implementing a
    set API.

    LibGimbal containers have been created with the goal of interoping directly with their C++ wrapper analogs, and as such can be used
    from either the C or C++ API. To facilitate this interop, and because we like the overall API design, the C APIs tend to mirror C++'s
    STL APIs fairly closely and should be fairly intuitive to work with for any C++ developer.


    ## Choosing the Right Container
        The following table can be referenced when deciding which container type to use:

        Type  			       | Description            						                                  | Header
        -----------------------|----------------------------------------------------------------------------------|-------
        GblLinkedListNode      |Intrusive, circularly singly linked list with C++ std::vector-style API           | gimbal_linked_list.h
        GblDoublyLinkedListNode|Intrusive, circularly doubly linked list with C++ std::vector-style API           | gimbal_doubly_linked_list.h
        GblNaryTreeNode		   |Instrusive generic tree structure with parent + linked list of children           | gimbal_nary_tree.h
        GblArrayList    	   |Dynamically resizable array, O(1) lookup, equivalent to C++ std::vector	          | gimbal_array_list.h
        GblRingList            |Non-intrusive circular doubly-linked list with high-level, convenience API        | gimbal_ring_list.h
        GblArrayDeque		   |Dynamically resizable array with good performance when resizing from either end   | gimbal_array_deque.h
        GblTreeSet             |Binary tree-based associative set structure, analogous to std::set		          | gimbal_tree_set.h
        GblHashSet	           |Hash table-based associative set structure, analogous to std::unordered_set       | gimbal_hash_set.h
        GblArrayMap		       |Resizable array-based, [K,V] container with optional binary searchability         | gimbal_array_map.h
        GblArrayHeap           |Dynamic array-based, binary heap structure providing a priority queue API         | gimbal_array_heap.h
        GblRingBuffer          |Fixed-capacity circular buffer backed by a contiguous array with queue semantics  | gimbal_ring_buffer.h
*/

#endif // GIMBAL_CONTAINERS_H
