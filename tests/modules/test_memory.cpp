#define BADCPPLIB_ENABLE_CORE
#define BADCPPLIB_ENABLE_MEMORY
#include "badcpplib/badcpplib.hpp"
#include "badcpplib/test_framework.hpp"
#include <thread>
#include <atomic>

using namespace badcpplib;
using namespace badcpplib::testing;

// Test class for ObjectPool
class TestObject {
public:
    i32 value;
    string name;
    
    TestObject() : value(0), name("default") {}
    TestObject(i32 v, const string& n) : value(v), name(n) {}
    
    bool operator==(const TestObject& other) const {
        return value == other.value && name == other.name;
    }
};

// Test class for IntrusivePtr
class TestRefCounted : public memory::RefCounted {
public:
    i32 data;
    mutable bool destructor_called = false;
    
    TestRefCounted(i32 d = 0) : data(d) {}
    
    ~TestRefCounted() {
        destructor_called = true;
    }
};

int main() {
    TEST_SUITE("Memory Module Tests");
    
    TEST_CASE("LinearAllocator basic functionality")
        const usize allocator_size = 1024;
        memory::LinearAllocator allocator(allocator_size);
        
        // Check initial state
        ASSERT_EQ(allocator.get_allocated_bytes(), 0u);
        ASSERT_EQ(allocator.get_remaining_bytes(), allocator_size);
        ASSERT_EQ(allocator.get_utilization(), 0.0);
        
        // Memory allocation
        void* ptr1 = allocator.allocate(100);
        ASSERT_TRUE(ptr1 != nullptr);
        ASSERT_EQ(allocator.get_allocated_bytes(), 100u);
        ASSERT_TRUE(allocator.get_remaining_bytes() < allocator_size);
        
        void* ptr2 = allocator.allocate(200, 16); // With 16-byte alignment
        ASSERT_TRUE(ptr2 != nullptr);
        ASSERT_TRUE(allocator.get_allocated_bytes() >= 200u);
        
        // Check alignment
        usize addr = reinterpret_cast<usize>(ptr2);
        ASSERT_EQ(addr % 16, 0u);
        
        // Try to allocate more than available
        void* ptr_fail = allocator.allocate(allocator_size);
        ASSERT_TRUE(ptr_fail == nullptr);
        
        // Reset allocator
        allocator.reset();
        ASSERT_EQ(allocator.get_allocated_bytes(), 0u);
        ASSERT_EQ(allocator.get_remaining_bytes(), allocator_size);
    TEST_END
    
    TEST_CASE("LinearAllocator external memory")
        const usize buffer_size = 512;
        std::unique_ptr<u8[]> buffer = std::make_unique<u8[]>(buffer_size);
        
        memory::LinearAllocator allocator(buffer.get(), buffer_size);
        
        void* ptr = allocator.allocate(100);
        ASSERT_TRUE(ptr != nullptr);
        ASSERT_TRUE(ptr >= buffer.get());
        ASSERT_TRUE(ptr < buffer.get() + buffer_size);
        
        ASSERT_EQ(allocator.get_allocated_bytes(), 100u);
    TEST_END
    
    TEST_CASE("ObjectPool basic functionality")
        memory::ObjectPool<TestObject> pool(4); // 4 blocks per chunk
        
        // Check initial state
        ASSERT_EQ(pool.get_used_blocks(), 0u);
        ASSERT_EQ(pool.get_total_blocks(), 4u);
        ASSERT_EQ(pool.get_free_blocks(), 4u);
        ASSERT_EQ(pool.get_utilization(), 0.0);
        
        // Get objects from pool
        TestObject* obj1 = pool.acquire(42, "first");
        ASSERT_TRUE(obj1 != nullptr);
        ASSERT_EQ(obj1->value, 42);
        ASSERT_EQ(obj1->name, "first");
        ASSERT_EQ(pool.get_used_blocks(), 1u);
        
        TestObject* obj2 = pool.acquire(100, "second");
        ASSERT_TRUE(obj2 != nullptr);
        ASSERT_EQ(pool.get_used_blocks(), 2u);
        
        // Return objects to pool
        pool.release(obj1);
        ASSERT_EQ(pool.get_used_blocks(), 1u);
        ASSERT_EQ(pool.get_free_blocks(), 3u);
        
        pool.release(obj2);
        ASSERT_EQ(pool.get_used_blocks(), 0u);
        ASSERT_EQ(pool.get_free_blocks(), 4u);
        
        // Get more objects than in one chunk
        std::vector<TestObject*> objects;
        for (i32 i = 0; i < 6; ++i) {
            objects.push_back(pool.acquire(i, "test" + std::to_string(i)));
        }
        
        ASSERT_EQ(pool.get_used_blocks(), 6u);
        ASSERT_TRUE(pool.get_total_blocks() >= 6u); // Should have created new chunk
        
        // Cleanup
        for (auto* obj : objects) {
            pool.release(obj);
        }
    TEST_END
    
    TEST_CASE("PooledObject RAII wrapper")
        memory::ObjectPool<TestObject> pool(4);
        
        {
            auto pooled_obj = memory::make_pooled(pool, 123, "pooled");
            ASSERT_TRUE(pooled_obj.is_valid());
            ASSERT_EQ(pooled_obj->value, 123);
            ASSERT_EQ(pooled_obj->name, "pooled");
            ASSERT_EQ(pool.get_used_blocks(), 1u);
        } // Object automatically returned to pool
        
        ASSERT_EQ(pool.get_used_blocks(), 0u);
        
        // Test move semantics
        auto pooled1 = memory::make_pooled(pool, 456, "move_test");
        ASSERT_EQ(pool.get_used_blocks(), 1u);
        
        auto pooled2 = std::move(pooled1);
        ASSERT_FALSE(pooled1.is_valid());
        ASSERT_TRUE(pooled2.is_valid());
        ASSERT_EQ(pool.get_used_blocks(), 1u);
        
        pooled2.release(); // Manual release
        ASSERT_FALSE(pooled2.is_valid());
        ASSERT_EQ(pool.get_used_blocks(), 0u);
    TEST_END
    
    TEST_CASE("RefCounter basic functionality")
        memory::RefCounter counter;
        
        // Initial counter value
        ASSERT_EQ(counter.get_count(), 1u);
        
        // Increment counter
        counter.add_ref();
        ASSERT_EQ(counter.get_count(), 2u);
        
        counter.add_ref();
        ASSERT_EQ(counter.get_count(), 3u);
        
        // Decrement counter
        bool should_delete = counter.release();
        ASSERT_FALSE(should_delete);
        ASSERT_EQ(counter.get_count(), 2u);
        
        should_delete = counter.release();
        ASSERT_FALSE(should_delete);
        ASSERT_EQ(counter.get_count(), 1u);
        
        should_delete = counter.release();
        ASSERT_TRUE(should_delete);
        ASSERT_EQ(counter.get_count(), 0u);
    TEST_END
    
    TEST_CASE("IntrusivePtr basic functionality")
        // Create IntrusivePtr
        auto ptr1 = memory::make_intrusive<TestRefCounted>(42);
        ASSERT_TRUE(ptr1.is_valid());
        ASSERT_EQ(ptr1->data, 42);
        ASSERT_EQ(ptr1.use_count(), 1u);
        
        // Copy
        auto ptr2 = ptr1;
        ASSERT_EQ(ptr1.use_count(), 2u);
        ASSERT_EQ(ptr2.use_count(), 2u);
        ASSERT_EQ(ptr1.get(), ptr2.get());
        
        // Assignment
        memory::IntrusivePtr<TestRefCounted> ptr3;
        ASSERT_FALSE(ptr3.is_valid());
        
        ptr3 = ptr1;
        ASSERT_EQ(ptr1.use_count(), 3u);
        ASSERT_TRUE(ptr3.is_valid());
        
        // Reset
        ptr2.reset();
        ASSERT_FALSE(ptr2.is_valid());
        ASSERT_EQ(ptr1.use_count(), 2u);
        
        // Create a flag to track destruction
        std::shared_ptr<bool> destructor_flag = std::make_shared<bool>(false);
        
        // Create a new RefCounted object that sets the flag
        class TestRefCountedWithFlag : public memory::RefCounted {
        public:
            std::shared_ptr<bool> flag;
            i32 data;
            
            TestRefCountedWithFlag(std::shared_ptr<bool> f, i32 d) : flag(f), data(d) {}
            ~TestRefCountedWithFlag() { *flag = true; }
        };
        
        auto ptr_test = memory::make_intrusive<TestRefCountedWithFlag>(destructor_flag, 99);
        ptr_test.reset();
        
        // Object should be deleted
        ASSERT_TRUE(*destructor_flag);
    TEST_END
    
    TEST_CASE("IntrusivePtr move semantics")
        auto ptr1 = memory::make_intrusive<TestRefCounted>(100);
        TestRefCounted* raw_ptr = ptr1.get();
        
        ASSERT_EQ(ptr1.use_count(), 1u);
        
        // Move
        auto ptr2 = std::move(ptr1);
        ASSERT_FALSE(ptr1.is_valid());
        ASSERT_TRUE(ptr2.is_valid());
        ASSERT_EQ(ptr2.get(), raw_ptr);
        ASSERT_EQ(ptr2.use_count(), 1u);
        
        // Move assignment
        memory::IntrusivePtr<TestRefCounted> ptr3;
        ptr3 = std::move(ptr2);
        ASSERT_FALSE(ptr2.is_valid());
        ASSERT_TRUE(ptr3.is_valid());
        ASSERT_EQ(ptr3.use_count(), 1u);
    TEST_END
    
    TEST_CASE("AlignedAllocator functionality")
        // Test 16-byte alignment
        void* ptr16 = memory::AlignedAllocator<16>::allocate(100);
        ASSERT_TRUE(ptr16 != nullptr);
        
        usize addr16 = reinterpret_cast<usize>(ptr16);
        ASSERT_EQ(addr16 % 16, 0u);
        
        memory::AlignedAllocator<16>::deallocate(ptr16);
        
        // Test 64-byte alignment
        void* ptr64 = memory::AlignedAllocator<64>::allocate(200);
        ASSERT_TRUE(ptr64 != nullptr);
        
        usize addr64 = reinterpret_cast<usize>(ptr64);
        ASSERT_EQ(addr64 % 64, 0u);
        
        memory::AlignedAllocator<64>::deallocate(ptr64);
    TEST_END
    
    TEST_CASE("AlignedMemory RAII wrapper")
        {
            auto aligned_mem = memory::make_aligned_memory<32>(512);
            
            ASSERT_TRUE(aligned_mem.is_valid());
            ASSERT_EQ(aligned_mem.size(), 512u);
            
            // Check alignment
            usize addr = reinterpret_cast<usize>(aligned_mem.get());
            ASSERT_EQ(addr % 32, 0u);
            
            // Test read/write
            u8* data = static_cast<u8*>(aligned_mem.get());
            data[0] = 0xAA;
            data[511] = 0xBB;
            
            ASSERT_EQ(data[0], 0xAAu);
            ASSERT_EQ(data[511], 0xBBu);
        } // Memory automatically freed
        
        // Test move semantics
        auto mem1 = memory::make_aligned_memory<16>(256);
        void* ptr = mem1.get();
        
        auto mem2 = std::move(mem1);
        ASSERT_FALSE(mem1.is_valid());
        ASSERT_TRUE(mem2.is_valid());
        ASSERT_EQ(mem2.get(), ptr);
    TEST_END
    
    TEST_CASE("Memory utility functions")
        // is_aligned
        usize aligned_addr = 64;
        usize unaligned_addr = 65;
        
        ASSERT_TRUE(memory::utils::is_aligned(reinterpret_cast<void*>(aligned_addr), 64));
        ASSERT_FALSE(memory::utils::is_aligned(reinterpret_cast<void*>(unaligned_addr), 64));
        
        // align_up
        ASSERT_EQ(memory::utils::align_up(65, 16), 80u);
        ASSERT_EQ(memory::utils::align_up(64, 16), 64u);
        ASSERT_EQ(memory::utils::align_up(1, 8), 8u);
        
        // align_down
        ASSERT_EQ(memory::utils::align_down(65, 16), 64u);
        ASSERT_EQ(memory::utils::align_down(80, 16), 80u);
        ASSERT_EQ(memory::utils::align_down(15, 8), 8u);
        
        // secure_zero
        u8 buffer[16];
        std::fill(buffer, buffer + 16, 0xFF);
        memory::utils::secure_zero(buffer, 16);
        
        for (usize i = 0; i < 16; ++i) {
            ASSERT_EQ(buffer[i], 0u);
        }
        
        // memory_equal
        u8 buffer1[] = {1, 2, 3, 4, 5};
        u8 buffer2[] = {1, 2, 3, 4, 5};
        u8 buffer3[] = {1, 2, 3, 4, 6};
        
        ASSERT_TRUE(memory::utils::memory_equal(buffer1, buffer2, 5));
        ASSERT_FALSE(memory::utils::memory_equal(buffer1, buffer3, 5));
        ASSERT_TRUE(memory::utils::memory_equal(buffer1, buffer3, 4)); // First 4 bytes equal
    TEST_END
    
    TEST_CASE("UniqueResource RAII wrapper")
        // Test with simple resource
        bool resource_cleaned = false;
        
        {
            auto deleter = [&resource_cleaned](i32&) { resource_cleaned = true; };
            auto resource = memory::make_unique_resource(42, deleter);
            
            ASSERT_EQ(resource.get(), 42);
            ASSERT_FALSE(resource_cleaned);
        } // Destructor called here
        
        ASSERT_TRUE(resource_cleaned);
        
        // Test with file descriptor (simulation)
        i32 fake_fd = 123;
        bool fd_closed = false;
        
        auto closer = [&fd_closed](i32 fd) {
            if (fd > 0) fd_closed = true;
        };
        
        {
            auto file_resource = memory::make_unique_resource(fake_fd, closer);
            ASSERT_EQ(file_resource.get(), 123);
        } // Destructor should call closer
        
        ASSERT_TRUE(fd_closed);
    TEST_END
    
    TEST_CASE("Memory stress test")
        // Test performance and correctness under heavy load
        const usize iterations = 1000;
        
        // LinearAllocator stress test
        memory::LinearAllocator allocator(64 * 1024); // 64KB
        
        std::vector<void*> allocations;
        for (usize i = 0; i < iterations && allocator.get_remaining_bytes() > 32; ++i) {
            void* ptr = allocator.allocate(32, 8);
            if (ptr) {
                allocations.push_back(ptr);
                // Write test data
                *static_cast<u32*>(ptr) = static_cast<u32>(i);
            }
        }
        
        // Verify data not corrupted
        for (usize i = 0; i < allocations.size(); ++i) {
            u32 value = *static_cast<u32*>(allocations[i]);
            ASSERT_EQ(value, static_cast<u32>(i));
        }
        
        // ObjectPool stress test
        memory::ObjectPool<TestObject> pool(16);
        
        std::vector<TestObject*> objects;
        for (usize i = 0; i < iterations; ++i) {
            objects.push_back(pool.acquire(static_cast<i32>(i), "test"));
        }
        
        // Verify data
        for (usize i = 0; i < objects.size(); ++i) {
            ASSERT_EQ(objects[i]->value, static_cast<i32>(i));
        }
        
        // Release in random order
        for (usize i = 0; i < objects.size(); i += 2) {
            pool.release(objects[i]);
        }
        for (usize i = 1; i < objects.size(); i += 2) {
            pool.release(objects[i]);
        }
        
        ASSERT_EQ(pool.get_used_blocks(), 0u);
    TEST_END
    
    TEST_CASE("Thread safety basic test")
        memory::ObjectPool<TestObject> pool(32);
        const usize num_threads = 4;
        const usize operations_per_thread = 100;
        
        std::vector<std::thread> threads;
        std::atomic<usize> total_acquisitions{0};
        
        for (usize i = 0; i < num_threads; ++i) {
            threads.emplace_back([&pool, &total_acquisitions, operations_per_thread, i]() {
                std::vector<TestObject*> local_objects;
                
                // Acquire objects
                for (usize j = 0; j < operations_per_thread; ++j) {
                    TestObject* obj = pool.acquire(static_cast<i32>(i * 1000 + j), "thread_test");
                    if (obj) {
                        local_objects.push_back(obj);
                        total_acquisitions.fetch_add(1);
                    }
                }
                
                // Small delay
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                
                // Return objects
                for (auto* obj : local_objects) {
                    pool.release(obj);
                }
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Check results
        ASSERT_EQ(pool.get_used_blocks(), 0u);
        ASSERT_EQ(total_acquisitions.load(), num_threads * operations_per_thread);
    TEST_END
    
    RUN_ALL_TESTS();
} 