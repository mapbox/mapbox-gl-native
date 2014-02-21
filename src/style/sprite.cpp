#include <llmr/style/sprite.hpp>

#include <string>
#include <llmr/platform/platform.hpp>
#include <llmr/platform/gl.hpp>

#include <png.h>

#include <rapidjson/document.h>

using namespace llmr;

SpritePosition::SpritePosition(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t pixelRatio)
    : x(x),
      y(y),
      width(width),
      height(height),
      pixelRatio(pixelRatio) {}

ImagePosition::ImagePosition(const vec2<uint16_t>& size, vec2<float> tl, vec2<float> br)
    : size(size),
      tl(tl),
      br(br) {}


Sprite::~Sprite() {
    if (img) {
        free(img);
    }
}

Sprite::operator bool() const {
    std::lock_guard<std::mutex> lock(mtx);
    return loaded;
}

void Sprite::load(const std::string& base_url, float pixelRatio) {
    std::shared_ptr<Sprite> sprite = shared_from_this();

    auto complete = [sprite]() {
        std::lock_guard<std::mutex> lock(sprite->mtx);
        if (sprite->img && sprite->pos.size()) {
            sprite->loaded = true;
            platform::restart();
            fprintf(stderr, "sprite loaded\n");
        }
    };

    std::string suffix = (pixelRatio > 1 ? "@2x" : "");

    platform::request_http(base_url + suffix + ".json", [sprite](const platform::Response & res) {
        if (res.code == 200) {
            sprite->parseJSON(res.body);
        } else {
            fprintf(stderr, "failed to load sprite\n");
        }
    }, complete);

    platform::request_http(base_url + suffix + ".png", [sprite](const platform::Response & res) {
        if (res.code == 200) {
            sprite->loadImage(res.body);
        } else {
            fprintf(stderr, "failed to load sprite image\n");
        }
    }, complete);
}


void Sprite::parseJSON(const std::string& data) {
    std::lock_guard<std::mutex> lock(mtx);

    rapidjson::Document d;
    d.Parse<0>(data.c_str());

    if (d.IsObject()) {
        for (rapidjson::Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
            const std::string& name = itr->name.GetString();
            const rapidjson::Value& value = itr->value;

            if (value.IsObject()) {
                uint16_t x = 0;
                uint16_t y = 0;
                uint16_t width = 0;
                uint16_t height = 0;
                uint8_t pixelRatio = 1;

                if (value.HasMember("x")) x = value["x"].GetInt();
                if (value.HasMember("y")) y = value["y"].GetInt();
                if (value.HasMember("width")) width = value["width"].GetInt();
                if (value.HasMember("height")) height = value["height"].GetInt();
                if (value.HasMember("pixelRatio")) pixelRatio = value["height"].GetInt();

                fprintf(stderr, "");

                pos.insert({ name, { x, y, width, height, pixelRatio } });
            }
        }
    }
}

struct Buffer {
    Buffer(const std::string& data)
        : data(data.data()), length(data.size()) {}
    const char *const data = 0;
    const size_t length = 0;
    size_t pos = 0;
};


void readCallback(png_structp png, png_bytep data, png_size_t length) {
    Buffer *reader = static_cast<Buffer *>(png_get_io_ptr(png));

    // Read `length` bytes into `data`.
    if (reader->pos + length > reader->length) {
        png_error(png, "Read Error");
    } else {
        memcpy(data, reader->data + reader->pos, length);
        reader->pos += length;
    }
}

void errorHandler(png_structp png, png_const_charp error_msg) {
    throw std::runtime_error(error_msg);
}

void warningHandler(png_structp png, png_const_charp error_msg) {
    fprintf(stderr, "PNG: %s\n", error_msg);
}

void Sprite::loadImage(const std::string& data) {
    std::lock_guard<std::mutex> lock(mtx);

    Buffer buffer(data);

    if (buffer.length < 8 || !png_check_sig((const png_bytep)buffer.data, 8)) {
        fprintf(stderr, "image is not a valid PNG image\n");
        return;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, errorHandler, warningHandler);
    assert(png);

    png_infop info = png_create_info_struct(png);
    assert(info);

    int depth, color, interlace;

    try {
        png_set_read_fn(png, (png_voidp)&buffer, readCallback);
        png_read_info(png, info);
        png_get_IHDR(png, info, (png_uint_32*)&width, (png_uint_32*)&height, &depth, &color, &interlace, nullptr, nullptr);
        bool alpha = (color & PNG_COLOR_MASK_ALPHA) || png_get_valid(png, info, PNG_INFO_tRNS);

        // From http://trac.mapnik.org/browser/trunk/src/png_reader.cpp
        if (color == PNG_COLOR_TYPE_PALETTE)
            png_set_expand(png);
        if (color == PNG_COLOR_TYPE_GRAY)
            png_set_expand(png);
        if (png_get_valid(png, info, PNG_INFO_tRNS))
            png_set_expand(png);
        if (depth == 16)
            png_set_strip_16(png);
        if (depth < 8)
            png_set_packing(png);
        if (color == PNG_COLOR_TYPE_GRAY ||
                color == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png);

        if (interlace == PNG_INTERLACE_ADAM7)
            png_set_interlace_handling(png);

        // Always add an alpha channel.
        if (!alpha) {
            png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);
        }

        double gamma;
        if (png_get_gAMA(png, info, &gamma))
            png_set_gamma(png, 2.2, gamma);

        png_read_update_info(png, info);

        unsigned int rowbytes = png_get_rowbytes(png, info);
        assert(width * 4 == rowbytes);

        img = (char *)malloc(width * height * 4);
        char *surface = img;
        assert(surface);

        png_bytep row_pointers[height];
        for (unsigned i = 0; i < height; ++i) {
            row_pointers[i] = (png_bytep)(surface + (i * rowbytes));
        }

        // Read image data
        png_read_image(png, row_pointers);

        png_read_end(png, nullptr);
    } catch (std::exception& e) {
        fprintf(stderr, "loading PNG failed: %s\n", e.what());
        png_destroy_read_struct(&png, &info, nullptr);
        if (img) {
            free(img);
            img = nullptr;
        }
        width = 0;
        height = 0;
    }
}

ImagePosition Sprite::getPosition(const std::string& name, bool repeating) {
    if (!*this) return {};

    // `repeating` indicates that the image will be used in a repeating pattern
    // repeating pattern images are assumed to have a 1px padding that mirrors the opposite edge
    // positions for repeating images are adjusted to exclude the edge
    int8_t offset = repeating ? 1 : 0;

    auto it = pos.find(name);
    if (it == pos.end()) return {};

    SpritePosition& pos = it->second;
    return {
        {
            pos.width,
            pos.height
        },
        {
            (float)(pos.x + offset) / width,
            (float)(pos.y + offset) / height
        },
        {
            (float)(pos.x + pos.width - 2 * offset) / width,
            (float)(pos.y + pos.height - 2 * offset) / height
        }
    };
}

void Sprite::bind(bool linear) {
    if (!width || !height) {
        fprintf(stderr, "trying to bind texture without dimension\n");
        return;
    }

    if (!texture) {
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    GLuint filter = linear ? GL_LINEAR : GL_NEAREST;
    if (filter != this->filter) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        this->filter = filter;
    }
}
