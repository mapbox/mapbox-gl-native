#pragma once

#include <cstdint>

namespace mbgl {
namespace test {

class SQLite3TestFS {
public:
    SQLite3TestFS();
    ~SQLite3TestFS();

    // When enabled, the VFS will log all I/O operations to stdout.
    void setDebug(bool);

    // Allow any type of I/O. Will fail with SQLITE_AUTH if set to false. This is useful to simulate
    // scenarios where the OS blocks an entire app's I/O, e.g. when it's in the background.
    void allowIO(bool);
    
    // Allow files to be opened. Will fail with SQLITE_CANTOPEN if set to false.
    void allowFileOpen(bool);
    
    // Allow files to be created. Will fail with SQLITE_CANTOPEN if set to false.
    void allowFileCreate(bool);
    
    // Allow N bytes to be read, then fail reads with SQLITE_IOERR. -1 == unlimited
    // This limit is global, not per file.
    void setReadLimit(int64_t);
    
    // Allow N bytes to be written, then fail writes with SQLITE_FULL. -1 == unlimited
    // This limit is global, not per file.
    void setWriteLimit(int64_t);
    
    // Reset all restrictions.
    void reset();
};

} // namespace test
} // namespace mbgl
