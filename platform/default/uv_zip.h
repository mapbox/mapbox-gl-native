#ifndef UV_ZIP
#define UV_ZIP

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <uv.h>
#include <zip.h>

typedef struct uv_zip_s uv_zip_t;

typedef void (*uv_zip_cb)(uv_zip_t* req);

struct uv_zip_s {
    uv_work_t work;
    ssize_t result;
    const char *message;
    struct zip *archive;
    struct zip_file *file;
    struct zip_stat *stat;
    void *data;
    zip_flags_t flags;
    const char *path;
    uv_zip_cb cb;
    uv_buf_t *buf;
};

void uv_zip_init(uv_zip_t *zip);
void uv_zip_cleanup(uv_zip_t *zip);

int uv_zip_open(uv_loop_t* loop, uv_zip_t *zip, const char *path, zip_flags_t flags, uv_zip_cb cb);
int uv_zip_fdopen(uv_loop_t* loop, uv_zip_t *zip, uv_file fd, int flags, uv_zip_cb cb);
int uv_zip_stat(uv_loop_t* loop, uv_zip_t *zip, const char *fname, zip_flags_t flags, uv_zip_cb cb);
int uv_zip_fopen(uv_loop_t* loop, uv_zip_t *zip, const char *fname, zip_flags_t flags, uv_zip_cb cb);
int uv_zip_fread(uv_loop_t* loop, uv_zip_t *zip, struct zip_file *file, uv_buf_t *buf, uv_zip_cb cb);
int uv_zip_fclose(uv_loop_t* loop, uv_zip_t *zip, struct zip_file *file, uv_zip_cb cb);
int uv_zip_discard(uv_loop_t* loop, uv_zip_t *zip, uv_zip_cb cb);

#ifdef __cplusplus
}
#endif

#endif // UV_ZIP
