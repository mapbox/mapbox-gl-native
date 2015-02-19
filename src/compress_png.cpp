#include "compress_png.hpp"
#include <mbgl/util/image.hpp>

namespace node_mbgl {

class CompressPNGWorker : public NanAsyncWorker {
public:
    CompressPNGWorker(NanCallback *callback_, v8::Local<v8::Object> buffer_, uint32_t width_,
                      uint32_t height_)
        : NanAsyncWorker(callback_),
          data(node::Buffer::Data(buffer_)),
          width(width_),
          height(height_) {
        NanAssignPersistent(buffer, buffer_);
        assert(width * height * 4 == node::Buffer::Length(buffer_));
    }

    ~CompressPNGWorker() {
        NanDisposePersistent(buffer);
    }

    void Execute() {
        result = mbgl::util::compress_png(width, height, data);
    }

    void HandleOKCallback() {
        NanScope();

        auto img = new std::string(std::move(result));

        v8::Local<v8::Value> argv[] = {
            NanNull(),
            NanNewBufferHandle(
                const_cast<char *>(img->data()),
                img->size(),
                // Retain the std::string until the buffer is deleted.
                [](char *, void *hint) {
                    delete reinterpret_cast<std::string *>(hint);
                },
                img
            )
        };

        callback->Call(2, argv);
    };

private:
    // Retains the buffer while this worker is processing. The user may not modify the buffer.
    v8::Persistent<v8::Object> buffer;
    void *data;
    const uint32_t width;
    const uint32_t height;

    // Stores the compressed PNG.
    std::string result;
};

NAN_METHOD(CompressPNG) {
    NanScope();

    if (args.Length() <= 0 || !args[0]->IsObject()) {
        return NanThrowTypeError("First argument must be the data object");
    }

    uint32_t width = 0;
    uint32_t height = 0;
    v8::Local<v8::Object> buffer;

    auto options = args[0]->ToObject();
    if (options->Has(NanNew("width"))) {
        width = options->Get(NanNew("width"))->Uint32Value();
    }
    if (!width) {
        NanThrowRangeError("Image width must be greater than 0");
    }
    if (options->Has(NanNew("height"))) {
        height = options->Get(NanNew("height"))->Uint32Value();
    }
    if (!height) {
        NanThrowRangeError("Image height must be greater than 0");
    }
    if (options->Has(NanNew("pixels"))) {
        buffer = options->Get(NanNew("pixels")).As<v8::Object>();
    }
    if (!node::Buffer::HasInstance(buffer)) {
        NanThrowTypeError("Pixels must be a Buffer object");
    }
    if (width * height * 4 != node::Buffer::Length(buffer)) {
        NanThrowError("Pixel buffer doesn't match image dimensions");
    }

    if (args.Length() < 2) {
        NanThrowTypeError("Second argument must be a callback function");
    }
    NanCallback *callback = new NanCallback(args[1].As<v8::Function>());

    NanAsyncQueueWorker(new CompressPNGWorker(callback, buffer, width, height));
    NanReturnUndefined();
}

void InitCompressPNG(v8::Handle<v8::Object> target) {
    target->Set(NanNew<v8::String>("compressPNG"),
                NanNew<v8::FunctionTemplate>(CompressPNG)->GetFunction());
}

}
