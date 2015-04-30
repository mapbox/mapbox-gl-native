#include <mbgl/util/uv.hpp>
#include <mbgl/util/uv_detail.hpp>
#include <mbgl/util/string.hpp>

#include <uv.h>

// Check libuv library version.
const static bool uvVersionCheck = []() {
    const unsigned int version = uv_version();
    const unsigned int major = (version >> 16) & 0xFF;
    const unsigned int minor = (version >> 8) & 0xFF;
    const unsigned int patch = version & 0xFF;

#ifndef UV_VERSION_PATCH
    // 0.10 doesn't have UV_VERSION_PATCH defined, so we "fake" it by using the library patch level.
    const unsigned int UV_VERSION_PATCH = version & 0xFF;
#endif

    if (major != UV_VERSION_MAJOR || minor != UV_VERSION_MINOR || patch != UV_VERSION_PATCH) {
        throw std::runtime_error(mbgl::util::sprintf<96>(
            "libuv version mismatch: headers report %d.%d.%d, but library reports %d.%d.%d", UV_VERSION_MAJOR,
            UV_VERSION_MINOR, UV_VERSION_PATCH, major, minor, patch));
    }
    return true;
}();

#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10

int uv_key_create(uv_key_t* key) {
  return -pthread_key_create(key, NULL);
}

void uv_key_delete(uv_key_t* key) {
  if (pthread_key_delete(*key))
    abort();
}

void* uv_key_get(uv_key_t* key) {
  return pthread_getspecific(*key);
}

void uv_key_set(uv_key_t* key, void* value) {
  if (pthread_setspecific(*key, value))
    abort();
}

#endif

namespace uv {

std::string cwd() {
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
    char dir[512];
    uv_cwd(dir, 512);
    return dir;
#else
    size_t max = 0;
    std::string dir;
    do {
        max += 256;
        dir.resize(max);
        uv_cwd(const_cast<char *>(dir.data()), &max);
    } while (max == dir.size());
    dir.resize(max - 1);
    return dir;
#endif
}


lock::lock(mutex &mtx_) : mtx(&mtx_) {
    if (mtx) { mtx->lock(); }
}
lock::lock(const std::unique_ptr<mutex> &mtx_) : mtx(mtx_.get()) {
    if (mtx) { mtx->lock(); }
}
lock::~lock() {
    if (mtx) { mtx->unlock(); }
}
lock::lock(lock &&other) {
    std::swap(mtx, other.mtx);
}
lock &lock::operator=(lock &&other) {
    std::swap(mtx, other.mtx);
    return *this;
}


readlock::readlock(rwlock &mtx_) : mtx(&mtx_) {
    if (mtx) { mtx->rdlock(); }
}
readlock::readlock(const std::unique_ptr<rwlock> &mtx_) : mtx(mtx_.get()) {
    if (mtx) { mtx->rdlock(); }
}
readlock::~readlock() {
    if (mtx) { mtx->rdunlock(); }
}
readlock::readlock(readlock &&lock) {
    std::swap(mtx, lock.mtx);
}
readlock &readlock::operator=(readlock &&lock) {
    std::swap(mtx, lock.mtx);
    return *this;
}


writelock::writelock(rwlock &mtx_) : mtx(&mtx_) {
    if (mtx) { mtx->wrlock(); }
}
writelock::writelock(const std::unique_ptr<rwlock> &mtx_) : mtx(mtx_.get()) {
    if (mtx) { mtx->wrlock(); }
}
writelock::~writelock() {
    if (mtx) { mtx->wrunlock(); }
}
writelock::writelock(writelock &&lock) {
    std::swap(mtx, lock.mtx);
}
writelock &writelock::operator=(writelock &&lock) {
    std::swap(mtx, lock.mtx);
    return *this;
}

const char *getFileRequestError(uv_fs_t *req) {
#if UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR <= 10
    return uv_strerror(uv_last_error(req->loop));
#else
    return uv_strerror(int(req->result));
#endif
}

}
