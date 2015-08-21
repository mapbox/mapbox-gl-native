#include "uv_zip.h"

#include <assert.h>
#include <errno.h>
#include <string.h>


void uv__zip_open_error(uv_zip_t *zip, int error) {
    assert(zip);
    zip->result = -error;
    if (zip->message) {
        free((char *)zip->message);
        zip->message = NULL;
    }
    const zip_uint64_t size = zip_error_to_str(NULL, 0, error, errno) + 1;
    zip->message = malloc(size);
    zip_error_to_str((char *)zip->message, size, error, errno);
}

void uv__zip_store_error(uv_zip_t *zip, const char *message) {
    assert(zip);
    if (zip->message) {
        free((char *)zip->message);
        zip->message = NULL;
    }
    const unsigned long length = strlen(message) + 1;
    zip->message = malloc(length);
    strncpy((char *)zip->message, message, length);
}

void uv__zip_error(uv_zip_t *zip) {
    assert(zip);
    int error;
    zip_error_get(zip->archive, &error, NULL);
    zip->result = -error;
    uv__zip_store_error(zip, zip_strerror(zip->archive));
}

void uv__zip_file_error(uv_zip_t *zip) {
    assert(zip);
    int error;
    zip_file_error_get(zip->file, &error, NULL);
    zip->result = -error;
    uv__zip_store_error(zip, zip_file_strerror(zip->file));
}

void uv__zip_work_fdopen(uv_work_t *req) {
    uv_zip_t *zip = (uv_zip_t *)req->data;
    assert(zip);
    assert(!zip->archive);

    // extract the fd
    uv_file fd = *(uv_file *)zip->path;
    free((uv_file *)zip->path);
    zip->path = NULL;

    int error;
    zip->archive = zip_fdopen(fd, zip->flags, &error);
    if (!zip->archive) {
        uv__zip_open_error(zip, error);
    } else {
        zip->result = 0;
    }
}

void uv__zip_work_stat(uv_work_t *req) {
    uv_zip_t *zip = (uv_zip_t *)req->data;
    assert(zip);
    assert(zip->archive);
    if (!zip->stat) {
        zip->stat = malloc(sizeof(struct zip_stat));
        zip_stat_init(zip->stat);
    }
    if (0 != zip_stat(zip->archive, zip->path, zip->flags, zip->stat)) {
        uv__zip_error(zip);
    }
}

void uv__zip_work_fopen(uv_work_t *req) {
    uv_zip_t *zip = (uv_zip_t *)req->data;
    assert(zip);
    assert(zip->archive);
    zip->file = zip_fopen(zip->archive, zip->path, zip->flags);
    if (!zip->file) {
        uv__zip_error(zip);
    }
}

void uv__zip_work_fread(uv_work_t *req) {
    uv_zip_t *zip = (uv_zip_t *)req->data;
    assert(zip);
    assert(zip->file);
    assert(zip->buf);
    zip->result = zip_fread(zip->file, zip->buf->base, zip->buf->len);
    if (zip->result < 0) {
        uv__zip_file_error(zip);
    }
}

void uv__zip_work_fclose(uv_work_t *req) {
    uv_zip_t *zip = (uv_zip_t *)req->data;
    assert(zip);
    assert(zip->file);
    zip->result = zip_fclose(zip->file);
    if (zip->result != 0) {
        uv__zip_file_error(zip);
    } else {
        zip->file = NULL;
    }
}

void uv__zip_work_discard(uv_work_t *req) {
    uv_zip_t *zip = (uv_zip_t *)req->data;
    assert(zip->archive);
    zip_discard(zip->archive);
    zip->archive = NULL;
    zip->result = 0;
}

void uv__zip_after_work(uv_work_t *req, int status) {
    uv_zip_t *zip = (uv_zip_t *)req->data;
    assert(zip);
    if (zip->cb) {
        zip->cb(zip);
    }
}

void uv_zip_init(uv_zip_t *zip) {
    assert(zip);
    zip->work.data = zip;
    zip->message = NULL;
    zip->stat = NULL;
    uv_zip_cleanup(zip);
}

void uv_zip_cleanup(uv_zip_t *zip) {
    assert(zip);
    zip->data = NULL;
    zip->flags = 0;
    zip->result = 0;
    zip->path = NULL;
    zip->cb = NULL;
    zip->archive = NULL;
    zip->file = NULL;
    zip->buf = NULL;

    if (zip->message) {
        free((char *)zip->message);
        zip->message = NULL;
    }

    if (zip->stat) {
        free(zip->stat);
        zip->stat = NULL;
    }
}

int uv_zip_fdopen(uv_loop_t* loop, uv_zip_t *zip, uv_file fd, int flags, uv_zip_cb cb) {
    assert(loop);
    assert(zip);
    assert(fd);
    zip->result = 0;
    zip->path = malloc(sizeof(uv_file));
    *(uv_file *)zip->path = fd;
    zip->flags = flags;
    zip->cb = cb;
    return uv_queue_work(loop, &zip->work, uv__zip_work_fdopen, uv__zip_after_work);
}

int uv_zip_stat(uv_loop_t* loop, uv_zip_t *zip, const char *fname, zip_flags_t flags, uv_zip_cb cb) {
    assert(loop);
    assert(zip);
    assert(fname);
    assert(strlen(fname));
    assert(zip->archive);
    zip->result = 0;
    zip->path = fname;
    zip->flags = flags;
    zip->cb = cb;
    return uv_queue_work(loop, &zip->work, uv__zip_work_stat, uv__zip_after_work);
}

int uv_zip_fopen(uv_loop_t* loop, uv_zip_t *zip, const char *fname, zip_flags_t flags, uv_zip_cb cb) {
    assert(loop);
    assert(zip);
    assert(fname);
    assert(strlen(fname));
    assert(zip->archive);
    zip->result = 0;
    zip->path = fname;
    zip->flags = flags;
    zip->file = NULL;
    zip->cb = cb;
    return uv_queue_work(loop, &zip->work, uv__zip_work_fopen, uv__zip_after_work);
}

int uv_zip_fclose(uv_loop_t* loop, uv_zip_t *zip, struct zip_file *file, uv_zip_cb cb) {
    assert(loop);
    assert(zip);
    assert(file);
    assert(zip->archive);
    zip->result = 0;
    zip->file = file;
    zip->cb = cb;
    return uv_queue_work(loop, &zip->work, uv__zip_work_fclose, uv__zip_after_work);
}

int uv_zip_fread(uv_loop_t* loop, uv_zip_t *zip, struct zip_file *file, uv_buf_t *buf, uv_zip_cb cb) {
    assert(loop);
    assert(zip);
    assert(file);
    assert(buf);
    assert(zip->archive);
    zip->result = 0;
    zip->file = file;
    zip->buf = buf;
    zip->cb = cb;
    return uv_queue_work(loop, &zip->work, uv__zip_work_fread, uv__zip_after_work);
}

int uv_zip_discard(uv_loop_t* loop, uv_zip_t *zip, uv_zip_cb cb) {
    assert(loop);
    assert(zip);
    assert(!zip->file);
    zip->result = 0;
    zip->cb = cb;
    return uv_queue_work(loop, &zip->work, uv__zip_work_discard, uv__zip_after_work);
}
