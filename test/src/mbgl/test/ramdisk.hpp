#pragma once

#include <cstdio>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#if TARGET_OS_OSX
#define TEST_HAS_RAMDISK 1
#else
#define TEST_HAS_RAMDISK 0
#endif

namespace mbgl {
namespace test {

#if TEST_HAS_RAMDISK

class RamDisk {
public:
    RamDisk(size_t size = 1024 * 1024);
    ~RamDisk();
    void setReadOnly();
    void setReadWrite();
    void fillDiskExceptFor(size_t remaining = 0);
    void emptyDisk();

private:
    void write(const char* command);

private:
    FILE* child;
};

#endif

} // namespace test
} // namespace mbgl
