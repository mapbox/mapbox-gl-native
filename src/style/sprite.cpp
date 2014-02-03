#include <llmr/style/sprite.hpp>

#include <string>

#include <rapidjson/document.h>
// #include <rapidjson/rapidjson.h>

using namespace llmr;

Sprite::Sprite(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t pixelRatio)
    : x(x),
      y(y),
      width(width),
      height(height),
      pixelRatio(pixelRatio) {}


Texture llmr::createTexture(const std::string& data) {
    Texture texture;

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

                texture.insert({ name, { x, y, width, height, pixelRatio } });
            }
        }
    }

    return texture;
}

// Texture llmr::createTexture(const char *data, size_t length) {
//     Texture texture;

//     json_t *root;
//     json_error_t error;
//     root = json_loadb(data, length, 0, &error);
//     // TODO: handle error

//     if (json_is_object(root)) {
//         const char *key;
//         json_t *value;

//         json_object_foreach(root, key, value) {
//             if (json_is_object(value)) {
//                 uint16_t x = 0;
//                 uint16_t y = 0;
//                 uint16_t width = 0;
//                 uint16_t height = 0;
//                 uint8_t pixelRatio = 1;

//                 json_t *property;
//                 if ((property = json_object_get(value, "x"))) x = json_integer_value(property);
//                 if ((property = json_object_get(value, "y"))) y = json_integer_value(property);
//                 if ((property = json_object_get(value, "width"))) width = json_integer_value(property);
//                 if ((property = json_object_get(value, "height"))) height = json_integer_value(property);
//                 if ((property = json_object_get(value, "pixelRatio"))) pixelRatio = json_integer_value(property);

//                 fprintf(stderr, "%s: %d %d | %d %d | %d\n", key, x, y, width, height, pixelRatio);

//                 texture.insert({ key, { x, y, width, height, pixelRatio } });
//             } else {
//                 fprintf(stderr, "unknown value for key %s\n", key);
//             }
//         }
//     } else {
//         fprintf(stderr, "root element should be an object\n");
//     }

//     json_decref(root);


//     return texture;
// }

