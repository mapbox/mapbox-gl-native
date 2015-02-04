#include "thread_object.hpp"
#include "node_file_source.hpp"


 #include <unistd.h>

////////////////////////////////////////////////////////////////////////////////////////////////
// Static Node Methods

v8::Persistent<v8::FunctionTemplate> ThreadObject::constructorTemplate;

void ThreadObject::Init(v8::Handle<v8::Object> target) {
    NanScope();

    v8::Local<v8::FunctionTemplate> t = NanNew<v8::FunctionTemplate>(New);

    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(NanNew("ThreadObject"));

    NanAssignPersistent(constructorTemplate, t);

    target->Set(NanNew("ThreadObject"), t->GetFunction());
}

NAN_METHOD(ThreadObject::New) {
    NanScope();

    if (!args.IsConstructCall()) {
        return NanThrowTypeError("Use the new operator to create new ThreadObject objects");
    }

    if (args.Length() < 1 || !NanHasInstance(NodeFileSource::constructorTemplate, args[0])) {
        return NanThrowTypeError("Requires a FileSource as first argument");
    }

    auto fs = ObjectWrap::Unwrap<NodeFileSource>(args[0]->ToObject());
    auto to = new ThreadObject(fs);
    to->Wrap(args.This());

    NanReturnValue(args.This());
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Instance

ThreadObject::ThreadObject(NodeFileSource *fs) {
    self = v8::Persistent<v8::Object>::New(handle_);

    auto async = new uv_async_t;
    async->data = this;
    uv_async_init(uv_default_loop(), async, [](uv_async_t *as, int) {
        NanScope();
        auto to = reinterpret_cast<ThreadObject *>(as->data);

        to->self.Dispose();

        uv_close((uv_handle_t *)as, [](uv_handle_t *handle) {
            delete reinterpret_cast<uv_async_t *>(handle);
        });
    });

    thread = std::thread([this, fs, async]() {
        auto loop = uv_loop_new();

        fs->request({ mbgl::Resource::Image, "http://example.com" }, loop, [&](const mbgl::Response &res) {
            fprintf(stderr, "response.status: %d\n", res.status);
            fprintf(stderr, "response.etag: %s\n", res.etag.c_str());
            fprintf(stderr, "response.modified: %lld\n", res.modified);
            fprintf(stderr, "response.expires: %lld\n", res.expires);
            fprintf(stderr, "response.data.size: %ld\n", res.data.size());
            fprintf(stderr, "response.message: %s\n", res.message.c_str());
        });

        uv_run(loop, UV_RUN_DEFAULT);

        uv_loop_delete(loop);

        // Notify that the loop has finished executing.
        uv_async_send(async);
    });
}

ThreadObject::~ThreadObject() {
    thread.join();
}
