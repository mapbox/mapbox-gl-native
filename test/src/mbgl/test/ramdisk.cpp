#include <mbgl/test/ramdisk.hpp>

#include <stdexcept>

#include <cstring>

namespace mbgl {
namespace test {

#if TEST_HAS_RAMDISK

RamDisk::RamDisk(const size_t size) {
    char executable[64];
    snprintf(executable, sizeof executable, "test/scripts/ramdisk_darwin.sh %lu", size / 512);
    if (!(child = popen(executable, "r+"))) {
        throw std::runtime_error("Could not start child process");
    }
    write("\n");
}

void RamDisk::write(const char* command) {
    const size_t length = strlen(command);
    if (fwrite(command, 1, length, child) != length) {
        throw std::runtime_error("Could not write to child process");
    }
    // Read lines until we get a line with a single NUL byte, which serves as command completion marker.
    char line[256];
    do {
        if (!fgets(line, sizeof line, child)) {
            throw std::runtime_error("Could not read from child process");
        }
    } while (strncmp(line, "\0\n", sizeof line) != 0);
}

void RamDisk::setReadOnly() {
    write("readonly\n");
}

void RamDisk::setReadWrite() {
    write("readwrite\n");
}

void RamDisk::fillDiskExceptFor(size_t remaining) {
    char command[32];
    snprintf(command, sizeof command, "filldisk %lu\n", remaining);
    write(command);
}

void RamDisk::emptyDisk() {
    write("emptydisk\n");
}

RamDisk::~RamDisk() {
    pclose(child);
}

#endif // TEST_HAS_RAMDISK

} // namespace test
} // namespace mbgl
