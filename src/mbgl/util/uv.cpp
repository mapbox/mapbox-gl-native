#include <mbgl/util/uv.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <uv.h>

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
lock::lock(lock &&lock) {
    std::swap(mtx, lock.mtx);
}
lock &lock::operator=(lock &&lock) {
    std::swap(mtx, lock.mtx);
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
