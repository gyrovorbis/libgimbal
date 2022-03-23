//handles releasing previous shit
//handles releasing previous shit
//does nothing if already correct type#define GIMBAL_CONTAINER_HPP

#ifndef GIMBAL_CONTAINER_HPP
#define GIMBAL_CONTAINER_HPP

#include "../types/gimbal_typedefs.hpp"
#include "gimbal_container.h"
#include "gimbal_generics.hpp"
#include <limits>
#include <initializer_list>
#include <concepts>
#include <algorithm>
#include <compare>

namespace gimbal {

class Context;

template<typename It, typename T>
concept type_compatible_iterator_readable =
        std::input_iterator<It> &&
        requires(It it, T t) {
            { t = *it };
        };

template<typename It, typename T>
concept type_compatible_iterator_writable =
        std::output_iterator<It, T>;

template<typename It, typename T>
concept type_compatible_iterator =
    type_compatible_iterator_readable<It, T> &&
    type_compatible_iterator_writable<It, T>;

template<typename It, typename T>
concept type_compatible_iterator_read_only =
    type_compatible_iterator_readable<It, T> &&
    !type_compatible_iterator_writable<It, T>;

template<typename It>
concept contiguous_iterator =
    std::input_or_output_iterator<It> &&
    (std::is_base_of_v<std::contiguous_iterator_tag, It> ||
     std::is_base_of_v<gimbal::tags::RandomAccessIteratorBase, It>);

template<typename It>
concept forward_iterator =
    std::input_or_output_iterator<It> &&
    std::is_base_of_v<std::forward_iterator_tag, It>;

template<typename C, typename T>
concept type_compatible_container_readable =
    requires(C c) {
        { std::cbegin(c) } -> type_compatible_iterator_readable<T>;
        { std::cend(c) } -> type_compatible_iterator_readable<T>;
    };

template<typename C, typename T>
concept type_compatible_container_writable =
    requires(C c) {
        { std::begin(c) } -> type_compatible_iterator_writable<T>;
        { std::end(c) } -> type_compatible_iterator_writable<T>;
    };

template<typename C, typename T>
concept type_compatible_container =
    type_compatible_container_readable<C, T> &&
    type_compatible_container_writable<C, T>;

template<typename C, typename T>
concept type_compatible_container_read_only =
    type_compatible_container_readable<C, T> &&
    !type_compatible_container_writable<C, T>;

template<typename C, typename T>
concept type_compatible_contiguous_container_readable =
    type_compatible_container<C, T> &&
    requires(const C c) {
        { c.data() } -> std::same_as<const T*>;
    };

template<typename C, typename T>
concept type_compatible_contiguous_container_writable =
    type_compatible_container<C, T> &&
    requires(C c) {
        { c.data() } -> std::same_as<T*>;
    };

template<typename C, typename T>
concept type_compatible_contiguous_container =
    type_compatible_contiguous_container_readable<C, T> &&
    type_compatible_container_writable<C, T>;

template<typename C, typename T>
concept type_compatible_contiguous_container_read_only =
    type_compatible_contiguous_container_readable<C, T> &&
    !type_compatible_container_writable<C, T>;

template<typename C>
concept generic_container_readable =
    requires(C c) {
        { std::cbegin(c) } -> std::input_or_output_iterator;
        { std::cend(c) } -> std::input_or_output_iterator;
    };

template<typename C,
         typename T = std::decay_t<decltype(*std::begin(std::declval<C>()))>>
struct container_value {
    using type = T;
};

template<typename C>
using container_value_t = typename container_value<C>::type;

template<typename It,
         typename T = std::decay_t<decltype(*(std::declval<It>()))>>
struct iterator_value {
    using type = T;
};

template<typename C>
using iterator_value_t = typename iterator_value<C>::type;


}



#endif // GIMBAL_CONTAINER_HPP
