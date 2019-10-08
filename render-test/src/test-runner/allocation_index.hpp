#pragma once

#include <cstddef>

class AllocationIndex {
public:
    AllocationIndex() = delete;
    /**
     * @brief Starts/stops allocated memory indexing.
     */
    static void setActive(bool);
    /**
     * @brief Returns memory indexing activity status.
     */
    static bool isActive();
    /**
     * @brief Resets the allocated memory index and all statistics data.
     */
    static void reset();
    /**
     * @brief Same as `malloc()` + indexes the allocated data,
     * if indexing is active.
     * 
     * @return void* 
     */
    static void* allocate(size_t);
    /**
     * @brief Same as `free()` + removes the corresponding data from index,
     * if these data are present in the index and indexing is active.
     * 
     * @return void* 
     */
    static void deallocate(void*) noexcept;
    /**
     * @brief Returns the size (in bytes) of allocated data, currently present in the index.
     * 
     * @return size_t
     */
    static size_t getAllocatedSize();
    /**
     * @brief Returns the total amount of allocations since indexing start.
     * 
     * @return size_t 
     */
    static size_t getAllocationsCount();

    /**
     * @brief Returns the maximum size (in bytes) of the allocated data in the index, since last `reset()` call.
     * 
     * @return size_t 
     */
    static size_t getAllocatedSizePeak();
};
