#pragma once

/**
 * @file memory.hpp
 * @brief Advanced memory management utilities and allocators
 * @author fresh-milkshake
 * @version 1.0.0
 * @date 2024
 * 
 * This module provides advanced memory management facilities including custom allocators,
 * object pools, smart pointers, and memory alignment utilities. Enable with
 * `BADCPPLIB_ENABLE_MEMORY`.
 */

#include "core.hpp"
#include <memory>
#include <vector>
#include <atomic>
#include <mutex>
#include <algorithm>

namespace badcpplib {

/**
 * @namespace badcpplib::memory
 * @brief Advanced memory management and allocation utilities
 * 
 * This namespace provides a comprehensive set of memory management tools:
 * 
 * **Allocators:**
 * - IAllocator: Base interface for custom allocators
 * - LinearAllocator: Stack-based allocator for temporary allocations
 * - AlignedAllocator: Allocator with custom alignment requirements
 * 
 * **Object Management:**
 * - ObjectPool: Efficient object pooling for frequent allocations
 * - PooledObject: RAII wrapper for pooled objects
 * - UniqueResource: Generic RAII wrapper for any resource
 * 
 * **Smart Pointers:**
 * - IntrusivePtr: Intrusive reference-counted smart pointer
 * - RefCounted: Base class for intrusive reference counting
 * - RefCounter: Thread-safe reference counter
 * 
 * **Memory Utilities:**
 * - AlignedMemory: Aligned memory allocation wrapper
 * - Memory utility functions for alignment and safety
 * 
 * @section performance Performance Considerations
 * 
 * All allocators and utilities are designed for high performance:
 * - LinearAllocator: O(1) allocation, no fragmentation
 * - ObjectPool: O(1) allocation/deallocation with threading support
 * - IntrusivePtr: Lower overhead than std::shared_ptr
 * - Aligned allocations: Optimized for SIMD and cache performance
 * 
 * @section thread_safety Thread Safety
 * 
 * - ObjectPool: Thread-safe with internal locking
 * - RefCounter: Thread-safe using atomic operations
 * - Other components: Not thread-safe, external synchronization required
 * 
 * @example{lineno}
 * ```cpp
 * #define BADCPPLIB_ENABLE_MEMORY
 * #include "badcpplib/badcpplib.hpp"
 * using namespace badcpplib;
 * 
 * // Linear allocator for temporary allocations
 * LinearAllocator allocator(1024);
 * void* ptr = allocator.allocate(128);
 * allocator.reset();  // Free all at once
 * 
 * // Object pool for frequent allocations
 * ObjectPool<MyClass> pool(32);
 * auto obj = pool.acquire(constructor_args);
 * pool.release(obj);
 * 
 * // Intrusive smart pointer
 * class MyClass : public RefCounted { ... };
 * auto ptr = make_intrusive<MyClass>();
 * ```
 */
namespace memory {

// Base allocator interface
class IAllocator {
public:
    virtual ~IAllocator() = default;
    virtual void* allocate(usize size, usize alignment = sizeof(void*)) = 0;
    virtual void deallocate(void* ptr) = 0;
    virtual usize get_allocated_bytes() const = 0;
    virtual void reset() = 0;
};

// Simple linear allocator (Stack Allocator)
class LinearAllocator : public IAllocator {
private:
    u8* memory_;
    usize size_;
    usize offset_;
    usize allocated_bytes_;
    bool owns_memory_;
    
public:
    explicit LinearAllocator(usize size) 
        : size_(size), offset_(0), allocated_bytes_(0), owns_memory_(true) {
        memory_ = new u8[size];
    }
    
    LinearAllocator(void* memory, usize size)
        : memory_(static_cast<u8*>(memory)), size_(size), 
          offset_(0), allocated_bytes_(0), owns_memory_(false) {}
    
    ~LinearAllocator() {
        if (owns_memory_) {
            delete[] memory_;
        }
    }
    
    // Cannot copy
    LinearAllocator(const LinearAllocator&) = delete;
    LinearAllocator& operator=(const LinearAllocator&) = delete;
    
    // Can move
    LinearAllocator(LinearAllocator&& other) noexcept
        : memory_(other.memory_), size_(other.size_), 
          offset_(other.offset_), allocated_bytes_(other.allocated_bytes_),
          owns_memory_(other.owns_memory_) {
        other.memory_ = nullptr;
        other.owns_memory_ = false;
    }
    
    void* allocate(usize size, usize alignment = sizeof(void*)) override {
        // Address alignment
        usize current_addr = reinterpret_cast<usize>(memory_ + offset_);
        usize aligned_addr = (current_addr + alignment - 1) & ~(alignment - 1);
        usize aligned_offset = aligned_addr - reinterpret_cast<usize>(memory_);
        
        if (aligned_offset + size > size_) {
            return nullptr; // Not enough memory
        }
        
        void* result = memory_ + aligned_offset;
        offset_ = aligned_offset + size;
        allocated_bytes_ += size;
        
        return result;
    }
    
    void deallocate(void* ptr) override {
        // Linear allocator doesn't support individual deallocation
        // Can add pointer validity check
        (void)ptr;
    }
    
    void reset() override {
        offset_ = 0;
        allocated_bytes_ = 0;
    }
    
    usize get_allocated_bytes() const override {
        return allocated_bytes_;
    }
    
    usize get_remaining_bytes() const {
        return size_ - offset_;
    }
    
    f64 get_utilization() const {
        return static_cast<f64>(offset_) / static_cast<f64>(size_);
    }
};

// Fixed-size object pool
template<typename T>
class ObjectPool {
private:
    struct Block {
        alignas(T) u8 data[sizeof(T)];
        Block* next;
    };
    
    std::vector<std::unique_ptr<u8[]>> chunks_;
    Block* free_list_;
    usize block_size_;
    usize blocks_per_chunk_;
    usize total_blocks_;
    usize used_blocks_;
    std::mutex pool_mutex_;
    
    void allocate_new_chunk() {
        usize chunk_size = blocks_per_chunk_ * sizeof(Block);
        auto chunk = std::make_unique<u8[]>(chunk_size);
        
        // Initialize blocks in new chunk
        Block* blocks = reinterpret_cast<Block*>(chunk.get());
        for (usize i = 0; i < blocks_per_chunk_ - 1; ++i) {
            blocks[i].next = &blocks[i + 1];
        }
        blocks[blocks_per_chunk_ - 1].next = free_list_;
        free_list_ = blocks;
        
        total_blocks_ += blocks_per_chunk_;
        chunks_.push_back(std::move(chunk));
    }
    
public:
    explicit ObjectPool(usize blocks_per_chunk = 64) 
        : free_list_(nullptr), block_size_(sizeof(T)), 
          blocks_per_chunk_(blocks_per_chunk), total_blocks_(0), used_blocks_(0) {
        allocate_new_chunk();
    }
    
    ~ObjectPool() {
        // Object destructors must be called by user
    }
    
    template<typename... Args>
    T* acquire(Args&&... args) {
        std::lock_guard<std::mutex> lock(pool_mutex_);
        
        if (!free_list_) {
            allocate_new_chunk();
        }
        
        Block* block = free_list_;
        free_list_ = free_list_->next;
        used_blocks_++;
        
        // Create object in-place
        return new(block) T(std::forward<Args>(args)...);
    }
    
    void release(T* obj) {
        if (!obj) return;
        
        std::lock_guard<std::mutex> lock(pool_mutex_);
        
        // Call destructor
        obj->~T();
        
        // Return block to free list
        Block* block = reinterpret_cast<Block*>(obj);
        block->next = free_list_;
        free_list_ = block;
        used_blocks_--;
    }
    
    usize get_total_blocks() const { return total_blocks_; }
    usize get_used_blocks() const { return used_blocks_; }
    usize get_free_blocks() const { return total_blocks_ - used_blocks_; }
    f64 get_utilization() const { 
        return static_cast<f64>(used_blocks_) / static_cast<f64>(total_blocks_); 
    }
};

// RAII wrapper for automatic object return to pool
template<typename T>
class PooledObject {
private:
    T* object_;
    ObjectPool<T>* pool_;
    
public:
    PooledObject(T* obj, ObjectPool<T>* pool) : object_(obj), pool_(pool) {}
    
    ~PooledObject() {
        if (object_ && pool_) {
            pool_->release(object_);
        }
    }
    
    // Cannot copy
    PooledObject(const PooledObject&) = delete;
    PooledObject& operator=(const PooledObject&) = delete;
    
    // Can move
    PooledObject(PooledObject&& other) noexcept 
        : object_(other.object_), pool_(other.pool_) {
        other.object_ = nullptr;
        other.pool_ = nullptr;
    }
    
    PooledObject& operator=(PooledObject&& other) noexcept {
        if (this != &other) {
            if (object_ && pool_) {
                pool_->release(object_);
            }
            object_ = other.object_;
            pool_ = other.pool_;
            other.object_ = nullptr;
            other.pool_ = nullptr;
        }
        return *this;
    }
    
    T* get() const { return object_; }
    T& operator*() const { return *object_; }
    T* operator->() const { return object_; }
    
    bool is_valid() const { return object_ != nullptr; }
    
    // Manual object release
    void release() {
        if (object_ && pool_) {
            pool_->release(object_);
            object_ = nullptr;
            pool_ = nullptr;
        }
    }
};

// Function to create RAII object from pool
template<typename T, typename... Args>
PooledObject<T> make_pooled(ObjectPool<T>& pool, Args&&... args) {
    T* obj = pool.acquire(std::forward<Args>(args)...);
    return PooledObject<T>(obj, &pool);
}

// Reference counter (Reference Counter)
class RefCounter {
private:
    std::atomic<usize> count_;
    
public:
    RefCounter() : count_(1) {}
    
    void add_ref() {
        count_.fetch_add(1, std::memory_order_relaxed);
    }
    
    bool release() {
        return count_.fetch_sub(1, std::memory_order_acq_rel) == 1;
    }
    
    usize get_count() const {
        return count_.load(std::memory_order_relaxed);
    }
};

// Smart pointer with reference counting (intrusive)
template<typename T>
class IntrusivePtr {
private:
    T* ptr_;
    
    void add_ref() {
        if (ptr_) {
            ptr_->add_ref();
        }
    }
    
    void release() {
        if (ptr_ && ptr_->release()) {
            delete ptr_;
        }
    }
    
public:
    IntrusivePtr() : ptr_(nullptr) {}
    
    explicit IntrusivePtr(T* ptr) : ptr_(ptr) {
        // Don't increment counter as object already has count = 1
    }
    
    IntrusivePtr(const IntrusivePtr& other) : ptr_(other.ptr_) {
        add_ref();
    }
    
    IntrusivePtr(IntrusivePtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }
    
    ~IntrusivePtr() {
        release();
    }
    
    IntrusivePtr& operator=(const IntrusivePtr& other) {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            add_ref();
        }
        return *this;
    }
    
    IntrusivePtr& operator=(IntrusivePtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }
    
    T* get() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    
    bool is_valid() const { return ptr_ != nullptr; }
    explicit operator bool() const { return is_valid(); }
    
    void reset(T* ptr = nullptr) {
        release();
        ptr_ = ptr;
        // Don't increment counter if ptr is not nullptr
    }
    
    T* detach() {
        T* result = ptr_;
        ptr_ = nullptr;
        return result;
    }
    
    usize use_count() const {
        return ptr_ ? ptr_->get_count() : 0;
    }
};

// Base class for objects with intrusive reference counting
class RefCounted {
private:
    RefCounter counter_;
    
public:
    void add_ref() {
        counter_.add_ref();
    }
    
    bool release() {
        return counter_.release();
    }
    
    usize get_count() const {
        return counter_.get_count();
    }
    
protected:
    virtual ~RefCounted() = default;
};

// Function to create intrusive_ptr
template<typename T, typename... Args>
IntrusivePtr<T> make_intrusive(Args&&... args) {
    return IntrusivePtr<T>(new T(std::forward<Args>(args)...));
}

// Simple aligned allocator
template<usize Alignment>
class AlignedAllocator {
public:
    static void* allocate(usize size) {
        void* ptr = nullptr;
        
#ifdef _WIN32
        ptr = _aligned_malloc(size, Alignment);
#else
        if (posix_memalign(&ptr, Alignment, size) != 0) {
            ptr = nullptr;
        }
#endif
        
        return ptr;
    }
    
    static void deallocate(void* ptr) {
        if (ptr) {
#ifdef _WIN32
            _aligned_free(ptr);
#else
            free(ptr);
#endif
        }
    }
};

// RAII wrapper for aligned memory
template<usize Alignment>
class AlignedMemory {
private:
    void* ptr_;
    usize size_;
    
public:
    explicit AlignedMemory(usize size) : size_(size) {
        ptr_ = AlignedAllocator<Alignment>::allocate(size);
        if (!ptr_) {
            throw std::bad_alloc();
        }
    }
    
    ~AlignedMemory() {
        AlignedAllocator<Alignment>::deallocate(ptr_);
    }
    
    // Cannot copy
    AlignedMemory(const AlignedMemory&) = delete;
    AlignedMemory& operator=(const AlignedMemory&) = delete;
    
    // Can move
    AlignedMemory(AlignedMemory&& other) noexcept 
        : ptr_(other.ptr_), size_(other.size_) {
        other.ptr_ = nullptr;
        other.size_ = 0;
    }
    
    AlignedMemory& operator=(AlignedMemory&& other) noexcept {
        if (this != &other) {
            AlignedAllocator<Alignment>::deallocate(ptr_);
            ptr_ = other.ptr_;
            size_ = other.size_;
            other.ptr_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    void* get() const { return ptr_; }
    usize size() const { return size_; }
    
    template<typename T>
    T* as() const { return static_cast<T*>(ptr_); }
    
    bool is_valid() const { return ptr_ != nullptr; }
};

// Functions to create aligned memory
template<usize Alignment>
AlignedMemory<Alignment> make_aligned_memory(usize size) {
    return AlignedMemory<Alignment>(size);
}

// Memory utilities
namespace utils {
    
    // Check pointer alignment
    bool is_aligned(const void* ptr, usize alignment) {
        return (reinterpret_cast<usize>(ptr) % alignment) == 0;
    }
    
    // Align size up
    usize align_up(usize size, usize alignment) {
        return (size + alignment - 1) & ~(alignment - 1);
    }
    
    // Align size down
    usize align_down(usize size, usize alignment) {
        return size & ~(alignment - 1);
    }
    
    // Safe memory zeroing
    void secure_zero(void* ptr, usize size) {
        volatile u8* vptr = static_cast<volatile u8*>(ptr);
        for (usize i = 0; i < size; ++i) {
            vptr[i] = 0;
        }
    }
    
    // Memory copying with overlap check
    void safe_memcpy(void* dest, const void* src, usize size) {
        if (dest == src || size == 0) return;
        
        const u8* src_bytes = static_cast<const u8*>(src);
        u8* dest_bytes = static_cast<u8*>(dest);
        
        // Check for overlap
        if (src_bytes < dest_bytes && src_bytes + size > dest_bytes) {
            // Copy backwards to avoid overlap
            for (usize i = size; i > 0; --i) {
                dest_bytes[i - 1] = src_bytes[i - 1];
            }
        } else {
            // Normal forward copying
            for (usize i = 0; i < size; ++i) {
                dest_bytes[i] = src_bytes[i];
            }
        }
    }
    
    // Memory block comparison
    bool memory_equal(const void* ptr1, const void* ptr2, usize size) {
        const u8* bytes1 = static_cast<const u8*>(ptr1);
        const u8* bytes2 = static_cast<const u8*>(ptr2);
        
        for (usize i = 0; i < size; ++i) {
            if (bytes1[i] != bytes2[i]) {
                return false;
            }
        }
        
        return true;
    }
    
} // namespace utils

// RAII wrapper for any resource
template<typename Resource, typename Deleter>
class UniqueResource {
private:
    Resource resource_;
    Deleter deleter_;
    bool owns_resource_;
    
public:
    UniqueResource(Resource resource, Deleter deleter)
        : resource_(std::move(resource)), deleter_(std::move(deleter)), owns_resource_(true) {}
    
    ~UniqueResource() {
        if (owns_resource_) {
            deleter_(resource_);
        }
    }
    
    // Cannot copy
    UniqueResource(const UniqueResource&) = delete;
    UniqueResource& operator=(const UniqueResource&) = delete;
    
    // Can move
    UniqueResource(UniqueResource&& other) noexcept
        : resource_(std::move(other.resource_)), deleter_(std::move(other.deleter_)),
          owns_resource_(other.owns_resource_) {
        other.owns_resource_ = false;
    }
    
    UniqueResource& operator=(UniqueResource&& other) noexcept {
        if (this != &other) {
            if (owns_resource_) {
                deleter_(resource_);
            }
            resource_ = std::move(other.resource_);
            deleter_ = std::move(other.deleter_);
            owns_resource_ = other.owns_resource_;
            other.owns_resource_ = false;
        }
        return *this;
    }
    
    const Resource& get() const { return resource_; }
    Resource& get() { return resource_; }
    
    void release() {
        owns_resource_ = false;
    }
    
    void reset(Resource new_resource) {
        if (owns_resource_) {
            deleter_(resource_);
        }
        resource_ = std::move(new_resource);
        owns_resource_ = true;
    }
};

// Factory to create unique_resource
template<typename Resource, typename Deleter>
auto make_unique_resource(Resource&& resource, Deleter&& deleter) {
    return UniqueResource<std::decay_t<Resource>, std::decay_t<Deleter>>(
        std::forward<Resource>(resource), std::forward<Deleter>(deleter));
}

} // namespace memory
} // namespace badcpplib 