#ifndef GIMBAL_BUFFER_HPP
#define GIMBAL_BUFFER_HPP

#include "gimbal_buffer.h"
#include "../types/gimbal_typedefs.hpp"
#include "../objects/gimbal_context.hpp"

namespace gimbal {

class Buffer: public GblBuffer {
public:
                Buffer(Size bytes=0, const void* pData=nullptr, Context* pCtx=nullptr);
                Buffer(Size bytes, Context* pCtx);
                Buffer(Context* pCtx);

                Buffer(const Buffer& other);
                Buffer(const Buffer& other, Context* pCtx);
                Buffer(Buffer&& other);
                Buffer(Buffer&& other, Context* pCtx);

                ~Buffer(void);

    Buffer&     operator=(const Buffer& other);
    Buffer&     operator=(Buffer&& other);

    bool        operator==(const Buffer& rhs) const;
    bool        operator!=(const Buffer& rhs) const;

    Size        getSize(void) const;
    void*       getData(void) const;
    bool        isEmpty(void) const;
    Context*    getContext(void) const;

    void        clear(void);
    void        acquire(Size bytes, void* pData);
    auto        release(void) -> std::pair<Size, void*>;

    template<typename T>
    void        readValue(T* pOutData, Size offset=0, Size bytes=sizeof(T)) const;
    template<typename T>
    void        writeValue(const T& pInData, Size offset=0, Size bytes=sizeof(T));

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
inline Buffer::Buffer(Size bytes, const void* pData, Context* pCtx) {
    Exception::checkThrow(GblBuffer_construct(this, bytes, pData, pCtx? static_cast<GblContext>(*pCtx) : nullptr));
}
inline Buffer::Buffer(Size bytes, Context* pCtx):
    Buffer(bytes, nullptr, pCtx) {}

inline Buffer::Buffer(Context* pCtx):
    Buffer(0, nullptr, pCtx) {}

inline Buffer::~Buffer(void) {
    Exception::checkThrow(GblBuffer_destruct(this));
}

inline Buffer::Buffer(const Buffer& other, Context* pCtx):
    Buffer(pCtx)
{
    Exception::checkThrow(GblBuffer_copy(this, &other));
}

inline Buffer::Buffer(const Buffer& other):
    Buffer(other, other.getContext()) {}

inline Buffer::Buffer(Buffer&& other, Context* pCtx):
    Buffer(pCtx)
{
    Exception::checkThrow(GblBuffer_move(this, &other));
}

inline Buffer::Buffer(Buffer&& other):
    Buffer(std::move(other), other.getContext()) {}

inline bool Buffer::operator==(const Buffer& rhs) const {
    GblCmpResult result = 0;
    Exception::checkThrow(GblBuffer_compare(this, &rhs, &result));
    return !result;
}

inline bool Buffer::operator!=(const Buffer& rhs) const {
    return !(*this == rhs);
}


inline Buffer& Buffer::operator=(const Buffer& other) {
    Exception::checkThrow(GblBuffer_copy(this, &other));
    return *this;
}

inline Buffer& Buffer::operator=(Buffer&& other) {
    Exception::checkThrow(GblBuffer_move(this, &other));
    return *this;
}

inline Size Buffer::getSize(void) const {
    return GblBuffer_size(this);
}

inline void* Buffer::getData(void) const {
    return GblBuffer_data(this);
}

inline bool Buffer::isEmpty(void) const {
    return GblBuffer_empty(this);
}

inline Context* Buffer::getContext(void) const {
    return Context::fromHandle(GblBuffer_context(this));
}

inline void Buffer::clear(void) {
    Exception::checkThrow(GblBuffer_clear(this));
}

inline void Buffer::acquire(Size bytes, void* pData) {
    Exception::checkThrow(GblBuffer_acquire(this, bytes, pData));
}

inline auto Buffer::release(void) -> std::pair<Size, void*> {
    std::pair<Size, void*> retVal;
    Exception::checkThrow(GblBuffer_release(this, &retVal.first, &retVal.second));
    return retVal;
}

inline void Buffer::resize(Size bytes) {
    Exception::checkThrow(GblBuffer_resize(this, bytes));
}

inline void Buffer::grow(Size bytes) {
    Exception::checkThrow(GblBuffer_grow(this, bytes));
}

inline void Buffer::shrink(Size bytes) {
    Exception::checkThrow(GblBuffer_shrink(this, bytes));
}

inline void Buffer::erase(Size offset, Size bytes) {
    Exception::checkThrow(GblBuffer_erase(this, offset, bytes));
}

template<typename T>
inline void Buffer::insert(const T& dataIn, Size offset, Size bytes) {
    Exception::checkThrow(GblBuffer_insert(this, offset, bytes, &dataIn));
}

template<typename T>
inline void Buffer::append(const T& dataIn, Size bytes) {
    Exception::checkThrow(GblBuffer_append(this, bytes, &dataIn));
}

template<typename T>
inline void Buffer::prepend(const T& dataIn, Size bytes) {
    Exception::checkThrow(GblBuffer_prepend(this, bytes, &dataIn));
}
template<typename T>
inline void Buffer::readValue(T* pOutData, Size offset, Size bytes) const {
    Exception::checkThrow(GblBuffer_valueRead(this, offset, bytes, pOutData));
}

template<typename T>
inline void Buffer::writeValue(const T& pInData, Size offset, Size bytes) {
    Exception::checkThrow(GblBuffer_valueWrite(this, offset, bytes, &pInData));
}



}

#endif // GIMBAL_BUFFER_HPP
