#ifndef __QT__ // Qt doesn't expose SQLite VFS

#include <mbgl/test/sqlite3_test_fs.hpp>

#include <sqlite3.h>

#include <stdexcept>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>

static bool sqlite3_test_fs_debug = false;
static bool sqlite3_test_fs_io = true;
static bool sqlite3_test_fs_file_open = true;
static bool sqlite3_test_fs_file_create = true;
static int64_t sqlite3_test_fs_read_limit = -1;
static int64_t sqlite3_test_fs_write_limit = -1;

struct File {
    sqlite3_file base;
    sqlite3_file* real;
};

static int sqlite3_test_fs_close(sqlite3_file* pFile) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: close(%p)\n", pFile);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    auto* file = (File*)pFile;
    const int rc = file->real->pMethods->xClose(file->real);
    if (rc == SQLITE_OK) {
        sqlite3_free((void*)file->base.pMethods);
        file->base.pMethods = nullptr;
    }
    return rc;
}

static int sqlite3_test_fs_read(sqlite3_file* pFile, void* zBuf, int iAmt, sqlite3_int64 iOfst) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: read(%p, amount=%d, offset=%lld)\n", pFile, iAmt, iOfst);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    if (sqlite3_test_fs_read_limit >= 0) {
        if (iAmt > sqlite3_test_fs_read_limit) {
            iAmt = 0;
            return SQLITE_IOERR;
        }
        sqlite3_test_fs_read_limit -= iAmt;
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xRead(file->real, zBuf, iAmt, iOfst);
}

static int sqlite3_test_fs_write(sqlite3_file* pFile, const void* zBuf, int iAmt, sqlite3_int64 iOfst) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: write(%p, amount=%d, offset=%lld)\n", pFile, iAmt, iOfst);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    if (sqlite3_test_fs_write_limit >= 0) {
        if (iAmt > sqlite3_test_fs_write_limit) {
            iAmt = 0;
            return SQLITE_FULL;
        }
        sqlite3_test_fs_write_limit -= iAmt;
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xWrite(file->real, zBuf, iAmt, iOfst);
}

static int sqlite3_test_fs_truncate(sqlite3_file* pFile, sqlite3_int64 size) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: truncate(%p, size=%lld)\n", pFile, size);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xTruncate(file->real, size);
}

static int sqlite3_test_fs_sync(sqlite3_file* pFile, int flags) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: sync(%p, flags=%d)\n", pFile, flags);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xSync(file->real, flags);
}

static int sqlite3_test_fs_file_size(sqlite3_file* pFile, sqlite3_int64* pSize) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: file_size(%p)\n", pFile);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xFileSize(file->real, pSize);
}

static int sqlite3_test_fs_lock(sqlite3_file* pFile, int eLock) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: lock(%p, %d)\n", pFile, eLock);
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xLock(file->real, eLock);
}

static int sqlite3_test_fs_unlock(sqlite3_file* pFile, int eLock) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: unlock(%p, %d)\n", pFile, eLock);
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xUnlock(file->real, eLock);
}

static int sqlite3_test_fs_check_reserved_lock(sqlite3_file* pFile, int* pResOut) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: check_reserved_lock(%p)\n", pFile);
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xCheckReservedLock(file->real, pResOut);
}

static int sqlite3_test_fs_file_control(sqlite3_file* pFile, int op, void* pArg) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: file_control(%p, op=%d)\n", pFile, op);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xFileControl(file->real, op, pArg);
}

static int sqlite3_test_fs_sector_size(sqlite3_file* pFile) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: sector_size(%p)\n", pFile);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xSectorSize(file->real);
}

static int sqlite3_test_fs_device_characteristics(sqlite3_file* pFile) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: device_characteristics(%p)\n", pFile);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    auto* file = (File*)pFile;
    return file->real->pMethods->xDeviceCharacteristics(file->real);
}

static int sqlite3_test_fs_open(sqlite3_vfs* vfs, const char* zName, sqlite3_file* pFile, int flags, int* pOutFlags) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: open(name=%s, flags=%d) -> %p\n", zName, flags, pFile);
    }
    if (!sqlite3_test_fs_io) {
        pFile->pMethods = nullptr;
        return SQLITE_AUTH;
    }
    if (!sqlite3_test_fs_file_open) {
        pFile->pMethods = nullptr;
        return SQLITE_CANTOPEN;
    }

    auto* file = (File*)pFile;
    auto* unix_fs = (sqlite3_vfs*)vfs->pAppData;
    file->real = (sqlite3_file*)&file[1];

    if (!sqlite3_test_fs_file_create) {
        int res;
        const int result = unix_fs->xAccess(vfs, zName, SQLITE_ACCESS_EXISTS, &res);
        if (result != SQLITE_OK) {
            pFile->pMethods = nullptr;
            return result;
        }
        if (res != 1) {
            pFile->pMethods = nullptr;
            return SQLITE_CANTOPEN;
        }
    }

    const int status = unix_fs->xOpen(unix_fs, zName, file->real, flags, pOutFlags);
    if (file->real->pMethods) {
        auto* methods = (sqlite3_io_methods*)sqlite3_malloc(sizeof(sqlite3_io_methods));
        memset(methods, 0, sizeof(sqlite3_io_methods));
        methods->iVersion = 1;
        methods->xClose = sqlite3_test_fs_close;
        methods->xRead = sqlite3_test_fs_read;
        methods->xWrite = sqlite3_test_fs_write;
        methods->xTruncate = sqlite3_test_fs_truncate;
        methods->xSync = sqlite3_test_fs_sync;
        methods->xFileSize = sqlite3_test_fs_file_size;
        methods->xLock = sqlite3_test_fs_lock;
        methods->xUnlock = sqlite3_test_fs_unlock;
        methods->xCheckReservedLock = sqlite3_test_fs_check_reserved_lock;
        methods->xFileControl = sqlite3_test_fs_file_control;
        methods->xSectorSize = sqlite3_test_fs_sector_size;
        methods->xDeviceCharacteristics = sqlite3_test_fs_device_characteristics;
        pFile->pMethods = methods;
    }
    return status;
}

static int sqlite3_test_fs_delete(sqlite3_vfs* vfs, const char *zPath, int dirSync) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: delete(name=%s, sync=%d)\n", zPath, dirSync);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    auto* unix_fs = (sqlite3_vfs*)vfs->pAppData;
    return unix_fs->xDelete(unix_fs, zPath, dirSync);
}

static int sqlite3_test_fs_access(sqlite3_vfs* vfs, const char *zPath, int flags, int *pResOut) {
    if (sqlite3_test_fs_debug) {
        fprintf(stderr, "SQLite3: access(name=%s, flags=%d)\n", zPath, flags);
    }
    if (!sqlite3_test_fs_io) {
        return SQLITE_AUTH;
    }
    auto* unix_fs = (sqlite3_vfs*)vfs->pAppData;
    return unix_fs->xAccess(unix_fs, zPath, flags, pResOut);
}

namespace mbgl {
namespace test {

SQLite3TestFS::SQLite3TestFS() {
    sqlite3_vfs* unix_fs = sqlite3_vfs_find("unix");
    if (unix_fs == nullptr) {
        abort();
    }

    auto* test_fs = (sqlite3_vfs*)sqlite3_malloc(sizeof(sqlite3_vfs));
    if (test_fs == nullptr) {
        abort();
    }
    memset(test_fs, 0, sizeof(sqlite3_vfs));
    test_fs->iVersion = 1;
    test_fs->szOsFile = unix_fs->szOsFile + sizeof(File);
    test_fs->mxPathname = unix_fs->mxPathname;
    test_fs->zName = "test_fs";
    test_fs->pAppData = unix_fs;
    test_fs->xOpen = sqlite3_test_fs_open;
    test_fs->xDelete = sqlite3_test_fs_delete;
    test_fs->xAccess = sqlite3_test_fs_access;
    test_fs->xFullPathname = unix_fs->xFullPathname;
    test_fs->xDlOpen = unix_fs->xDlOpen;
    test_fs->xDlError = unix_fs->xDlError;
    test_fs->xDlSym = unix_fs->xDlSym;
    test_fs->xDlClose = unix_fs->xDlClose;
    test_fs->xRandomness = unix_fs->xRandomness;
    test_fs->xSleep = unix_fs->xSleep;
    test_fs->xCurrentTime = unix_fs->xCurrentTime;
    test_fs->xGetLastError = unix_fs->xGetLastError;

    sqlite3_vfs_register(test_fs, 0);
}

SQLite3TestFS::~SQLite3TestFS() {
    reset();
    sqlite3_vfs* test_fs = sqlite3_vfs_find("test_fs");
    if (test_fs) {
        sqlite3_vfs_unregister(test_fs);
        sqlite3_free((void*)test_fs);
    }
}

void SQLite3TestFS::setDebug(bool value) {
    sqlite3_test_fs_debug = value;
}

void SQLite3TestFS::allowIO(bool value) {
    sqlite3_test_fs_io = value;
}

void SQLite3TestFS::allowFileOpen(bool value) {
    sqlite3_test_fs_file_open = value;
}

void SQLite3TestFS::allowFileCreate(bool value) {
    sqlite3_test_fs_file_create = value;
}

void SQLite3TestFS::setReadLimit(int64_t value) {
    sqlite3_test_fs_read_limit = value;
}

void SQLite3TestFS::setWriteLimit(int64_t value) {
    sqlite3_test_fs_write_limit = value;
}

void SQLite3TestFS::reset() {
    setDebug(false);
    allowIO(true);
    allowFileOpen(true);
    allowFileCreate(true);
    setReadLimit(-1);
    setWriteLimit(-1);
}

} // namespace test
} // namespace mbgl

#endif // __QT__
