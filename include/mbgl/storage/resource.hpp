#pragma once

#include <mbgl/storage/response.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/util.hpp>
#include <mbgl/util/traits.hpp>

#include <string>

namespace mbgl {

class Resource {
public:
    enum Kind : uint8_t {
        Unknown = 0,
        Style,
        Source,
        Tile,
        Glyphs,
        SpriteImage,
        SpriteJSON,
        Image
    };

    struct TileData {
        std::string urlTemplate;
        uint8_t pixelRatio;
        int32_t x;
        int32_t y;
        int8_t z;
    };

    enum class LoadingMethod : uint8_t {
        None        = 0b00,
        Cache       = 0b01,
        Network     = 0b10,

        CacheOnly   = Cache,
        NetworkOnly = Network,
        All         = Cache | Network,
    };

    enum class Compression : bool {
        // The data will be an uncompressed blob, even if it was obtained in compressed form.
        Uncompressed = false,

        // The data will be returned compressed if it was already obtained in compressed form,
        // and uncompressed otherwise.
        PreferCompressed = true,
    };

    Resource(Kind kind_,
             std::string url_,
             optional<TileData> tileData_ = {},
             LoadingMethod loadingMethod_ = LoadingMethod::All,
             Compression compression_ = Compression::PreferCompressed)
        : kind(kind_),
          loadingMethod(loadingMethod_),
          compression(compression_),
          url(std::move(url_)),
          tileData(std::move(tileData_)) {
    }

    bool hasLoadingMethod(LoadingMethod method);

    static Resource style(const std::string& url);
    static Resource source(const std::string& url);
    static Resource tile(const std::string& urlTemplate,
                         float pixelRatio,
                         int32_t x,
                         int32_t y,
                         int8_t z,
                         Tileset::Scheme scheme,
                         LoadingMethod = LoadingMethod::All);
    static Resource glyphs(const std::string& urlTemplate,
                           const FontStack& fontStack,
                           const std::pair<uint16_t, uint16_t>& glyphRange);
    static Resource spriteImage(const std::string& base, float pixelRatio);
    static Resource spriteJSON(const std::string& base, float pixelRatio);
    static Resource image(const std::string& url);
    
    Kind kind;
    LoadingMethod loadingMethod;
    Compression compression;
    std::string url;

    // Includes auxiliary data if this is a tile request.
    optional<TileData> tileData;

    optional<Timestamp> priorModified = {};
    optional<Timestamp> priorExpires = {};
    optional<std::string> priorEtag = {};
    Blob priorData;
};


MBGL_CONSTEXPR Resource::LoadingMethod operator|(Resource::LoadingMethod a, Resource::LoadingMethod b) {
    return Resource::LoadingMethod(mbgl::underlying_type(a) | mbgl::underlying_type(b));
}

MBGL_CONSTEXPR Resource::LoadingMethod& operator|=(Resource::LoadingMethod& a, Resource::LoadingMethod b) {
    return (a = a | b);
}

MBGL_CONSTEXPR Resource::LoadingMethod operator&(Resource::LoadingMethod a, Resource::LoadingMethod b) {
    return Resource::LoadingMethod(mbgl::underlying_type(a) & mbgl::underlying_type(b));
}

inline bool Resource::hasLoadingMethod(Resource::LoadingMethod method) {
    return (loadingMethod & method) != Resource::LoadingMethod::None;
}

} // namespace mbgl
