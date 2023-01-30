#ifndef GIMBAL_BYTE_ARRAY_HPP
#define GIMBAL_BYTE_ARRAY_HPP

#include <utility>
#include "gimbal_byte_array.h"
#include "../types/gimbal_typedefs.hpp"
#include "../objects/gimbal_context.hpp"

namespace gimbal {

class ByteArray: public GblByteArray {
public:
                ByteArray(Size bytes=0, const void* pData=nullptr);

                ByteArray(const ByteArray& other);
                ByteArray(ByteArray&& other);

                ~ByteArray(void);

    ByteArray&  operator=(const ByteArray& other);
    ByteArray&  operator=(ByteArray&& other);

    bool        operator==(const ByteArray& rhs) const;
    bool        operator!=(const ByteArray& rhs) const;

    Size        getSize(void) const;
    template<typename T=void*>
    T           getData(void) const;
    bool        isEmpty(void) const;
    Context*    getContext(void) const;

    void        clear(void);
    void        acquire(std::pair<Size, void*> data);
    auto        release(void) -> std::pair<Size, void*>;

    template<typename T>
    void        readData(T* pOutData, Size offset=0, Size bytes=sizeof(T)) const;
    template<typename T>
    void        writeData(const T& pInData, Size offset=0, Size bytes=sizeof(T));

    void        resize(Size bytes);
    void        grow(Size bytes);
    void        shrink(Size bytes);

    void        erase(Size offset, Size bytes);

    template<typename T>
    void        insert(const T& dataIn, Size offset=0, Size bytes=sizeof(T));
    template<typename T>
    void        append(const T& dataIn, Size bytes=sizeof(T));
    template<typename T>
    void        prepend(const T& dataIn, Size bytes=sizeof(T));
};



// ======= IMPLEMENTATION =======
inline ByteArray::ByteArray(Size bytes, const void* pData) {
    Exception::checkThrow(GblByteArray_construct(this, bytes, pData));
}


inline ByteArray::ByteArray(const ByteArray& other) {
    Exception::checkThrow(GblByteArray_copy(this, &other));
}

inline ByteArray::ByteArray(ByteArray&& other) {
    Exception::checkThrow(GblByteArray_move(this, &other));
}

inline ByteArray::~ByteArray(void) {
    Exception::checkThrow(GblByteArray_destruct(this));
}

inline bool ByteArray::operator==(const ByteArray& rhs) const {
    GblCmpResult result = GblByteArray_compare(this, &rhs);
    return !result;
}

inline bool ByteArray::operator!=(const ByteArray& rhs) const {
    return !(*this == rhs);
}


inline ByteArray& ByteArray::operator=(const ByteArray& other) {
    Exception::checkThrow(GblByteArray_copy(this, &other));
    return *this;
}

inline ByteArray& ByteArray::operator=(ByteArray&& other) {
    Exception::checkThrow(GblByteArray_move(this, &other));
    return *this;
}

inline Size ByteArray::getSize(void) const {
    return GblByteArray_size(this);
}

template<typename T>
inline T ByteArray::getData(void) const {
    return reinterpret_cast<T>(GblByteArray_data(this));
}

inline bool ByteArray::isEmpty(void) const {
    return GblByteArray_isEmpty(this);
}

inline void ByteArray::clear(void) {
    Exception::checkThrow(GblByteArray_clear(this));
}

inline void ByteArray::acquire(std::pair<Size, void*> data) {
    Exception::checkThrow(GblByteArray_acquire(this, data.first, data.second));
}

inline auto ByteArray::release(void) -> std::pair<Size, void*> {
    std::pair<Size, void*> retVal;
    Exception::checkThrow(GblByteArray_release(this, &retVal.first, &retVal.second));
    return retVal;
}

inline void ByteArray::resize(Size bytes) {
    Exception::checkThrow(GblByteArray_resize(this, bytes));
}

inline void ByteArray::grow(Size bytes) {
    Exception::checkThrow(GblByteArray_grow(this, bytes));
}

inline void ByteArray::shrink(Size bytes) {
    Exception::checkThrow(GblByteArray_shrink(this, bytes));
}

inline void ByteArray::erase(Size offset, Size bytes) {
    Exception::checkThrow(GblByteArray_erase(this, offset, bytes));
}

template<typename T>
inline void ByteArray::insert(const T& dataIn, Size offset, Size bytes) {
    Exception::checkThrow(GblByteArray_insert(this, offset, bytes, &dataIn));
}

template<typename T>
inline void ByteArray::append(const T& dataIn, Size bytes) {
    Exception::checkThrow(GblByteArray_append(this, bytes, &dataIn));
}

template<typename T>
inline void ByteArray::prepend(const T& dataIn, Size bytes) {
    Exception::checkThrow(GblByteArray_prepend(this, bytes, &dataIn));
}
template<typename T>
inline void ByteArray::readData(T* pOutData, Size offset, Size bytes) const {
    Exception::checkThrow(GblByteArray_dataRead(this, offset, bytes, pOutData));
}

template<typename T>
inline void ByteArray::writeData(const T& pInData, Size offset, Size bytes) {
    Exception::checkThrow(GblByteArray_dataWrite(this, offset, bytes, &pInData));
}



}

#endif // GIMBAL_ByteArray_HPP
