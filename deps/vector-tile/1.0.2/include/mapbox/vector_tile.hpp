#pragma once

#include "vector_tile/vector_tile_config.hpp"
#include <mapbox/geometry.hpp>
#include <protozero/pbf_reader.hpp>

#include <cmath>
#include <cstdint>
#include <map>
#include <functional> // reference_wrapper
#include <string>
#include <stdexcept>

#include <experimental/optional>

template <typename T>
using optional = std::experimental::optional<T>;

namespace mapbox { namespace vector_tile {

using point_type = mapbox::geometry::point<std::int16_t>;

class points_array_type : public std::vector<point_type> {
public:
    using coordinate_type = point_type::coordinate_type;
    template <class... Args>
    points_array_type(Args&&... args) : std::vector<point_type>(std::forward<Args>(args)...) {}
};

class points_arrays_type : public std::vector<points_array_type> {
public:
    using coordinate_type = points_array_type::coordinate_type;
    template <class... Args>
    points_arrays_type(Args&&... args) : std::vector<points_array_type>(std::forward<Args>(args)...) {}
};

class layer;

class feature {
public:
    using properties_type = mapbox::geometry::property_map;
    using packed_iterator_type = protozero::iterator_range<protozero::pbf_reader::const_uint32_iterator>;

    feature(protozero::data_view const&, layer const&);

    GeomType getType() const { return type; }
    optional<mapbox::geometry::value> getValue(std::string const&) const;
    properties_type getProperties() const;
    optional<mapbox::geometry::identifier> const& getID() const;
    std::uint32_t getExtent() const;
    std::uint32_t getVersion() const;
    template <typename GeometryCollectionType>
    GeometryCollectionType getGeometries(float scale) const;

private:
    const layer& layer_;
    optional<mapbox::geometry::identifier> id;
    GeomType type = GeomType::UNKNOWN;
    packed_iterator_type tags_iter;
    packed_iterator_type geometry_iter;
};

class layer {
public:
    layer(protozero::data_view const& layer_view);

    std::size_t featureCount() const { return features.size(); }
    protozero::data_view const& getFeature(std::size_t) const;
    std::string const& getName() const;
    std::uint32_t getExtent() const { return extent; }
    std::uint32_t getVersion() const { return version; }

private:
    friend class feature;

    std::string name;
    std::uint32_t version;
    std::uint32_t extent;
    std::map<std::string, std::uint32_t> keysMap;
    std::vector<std::reference_wrapper<const std::string>> keys;
    std::vector<protozero::data_view> values;
    std::vector<protozero::data_view> features;
};

class buffer {
public:
    buffer(std::string const& data);
    std::vector<std::string> layerNames() const;
    std::map<std::string, const protozero::data_view> getLayers() const { return layers; };
    layer getLayer(const std::string&) const;

private:
    std::map<std::string, const protozero::data_view> layers;
};

static mapbox::geometry::value parseValue(protozero::data_view const& value_view) {
    mapbox::geometry::value value;
    protozero::pbf_reader value_reader(value_view);
    while (value_reader.next())
    {
        switch (value_reader.tag()) {
        case ValueType::STRING:
            value = value_reader.get_string();
            break;
        case ValueType::FLOAT:
            value = static_cast<double>(value_reader.get_float());
            break;
        case ValueType::DOUBLE:
            value = value_reader.get_double();
            break;
        case ValueType::INT:
            value = value_reader.get_int64();
            break;
        case ValueType::UINT:
            value = value_reader.get_uint64();
            break;
        case ValueType::SINT:
            value = value_reader.get_sint64();
            break;
        case ValueType::BOOL:
            value = value_reader.get_bool();
            break;
        default:
            value_reader.skip();
            break;
        }
    }
    return value;
}

inline feature::feature(protozero::data_view const& feature_view, layer const& l)
    : layer_(l),
      id(),
      type(GeomType::UNKNOWN),
      tags_iter(),
      geometry_iter()
    {
    protozero::pbf_reader feature_pbf(feature_view);
    while (feature_pbf.next()) {
        switch (feature_pbf.tag()) {
        case FeatureType::ID:
            id = optional<mapbox::geometry::identifier>{ feature_pbf.get_uint64() };
            break;
        case FeatureType::TAGS:
            tags_iter = feature_pbf.get_packed_uint32();
            break;
        case FeatureType::TYPE:
            type = static_cast<GeomType>(feature_pbf.get_enum());
            break;
        case FeatureType::GEOMETRY:
            geometry_iter = feature_pbf.get_packed_uint32();
            break;
        default:
            feature_pbf.skip();
            break;
        }
    }
}

inline optional<mapbox::geometry::value> feature::getValue(const std::string& key) const {
    auto keyIter = layer_.keysMap.find(key);
    if (keyIter == layer_.keysMap.end()) {
        return optional<mapbox::geometry::value>();
    }

    const auto values_count = layer_.values.size();
    const auto keymap_count = layer_.keysMap.size();
    auto start_itr = tags_iter.begin();
    const auto end_itr = tags_iter.end();
    while (start_itr != end_itr) {
        std::uint32_t tag_key = static_cast<std::uint32_t>(*start_itr++);

        if (keymap_count <= tag_key) {
            throw std::runtime_error("feature referenced out of range key");
        }

        if (start_itr == end_itr) {
            throw std::runtime_error("uneven number of feature tag ids");
        }

        std::uint32_t tag_val = static_cast<std::uint32_t>(*start_itr++);;
        if (values_count <= tag_val) {
            throw std::runtime_error("feature referenced out of range value");
        }

        if (tag_key == keyIter->second) {
            return parseValue(layer_.values[tag_val]);
        }
    }

    return optional<mapbox::geometry::value>();
}

inline feature::properties_type feature::getProperties() const {
    auto start_itr = tags_iter.begin();
    const auto end_itr = tags_iter.end();
    properties_type properties;
    auto iter_len = std::distance(start_itr,end_itr);
    if (iter_len > 0) {
        properties.reserve(static_cast<std::size_t>(iter_len/2));
        while (start_itr != end_itr) {
            std::uint32_t tag_key = static_cast<std::uint32_t>(*start_itr++);
            if (start_itr == end_itr) {
                throw std::runtime_error("uneven number of feature tag ids");
            }
            std::uint32_t tag_val = static_cast<std::uint32_t>(*start_itr++);
            properties.emplace(layer_.keys.at(tag_key),parseValue(layer_.values.at(tag_val)));
        }
    }
    return properties;
}

inline optional<mapbox::geometry::identifier> const& feature::getID() const {
    return id;
}

inline std::uint32_t feature::getExtent() const {
    return layer_.getExtent();
}

inline std::uint32_t feature::getVersion() const {
    return layer_.getVersion();
}

template <typename GeometryCollectionType>
GeometryCollectionType feature::getGeometries(float scale) const {
    std::uint8_t cmd = 1;
    std::uint32_t length = 0;
    std::int64_t x = 0;
    std::int64_t y = 0;

    GeometryCollectionType paths;

    paths.emplace_back();

    auto start_itr = geometry_iter.begin();
    const auto end_itr = geometry_iter.end();
    bool first = true;
    std::uint32_t len_reserve = 0;
    std::size_t extra_coords = 0;
    if (type == GeomType::LINESTRING) {
        extra_coords = 1;
    } else if (type == GeomType::POLYGON) {
        extra_coords = 2;
    }
    bool is_point = type == GeomType::POINT;

    while (start_itr != end_itr) {
        if (length == 0) {
            std::uint32_t cmd_length = static_cast<std::uint32_t>(*start_itr++);
            cmd = cmd_length & 0x7;
            length = len_reserve = cmd_length >> 3;
            // Prevents the creation of vector tiles that would cause
            // a denial of service from massive over allocation. Protection
            // limit is based on the assumption of an int64_t point which is
            // 16 bytes in size and wanting to have a maximum of 1 MB of memory
            // used.
            constexpr std::uint32_t MAX_LENGTH = (1024 * 1024) / 16;
            if (len_reserve > MAX_LENGTH) {
                len_reserve = MAX_LENGTH;
            }
        }

        --length;

        if (cmd == CommandType::MOVE_TO || cmd == CommandType::LINE_TO) {

            if (is_point) {
                if (first && cmd == CommandType::MOVE_TO) {
                    // note: this invalidates pointers. So we always
                    // dynamically get the path with paths.back()
                    paths.reserve(len_reserve);
                    first = false;
                }
            } else {
                if (first && cmd == CommandType::LINE_TO) {
                    paths.back().reserve(len_reserve + extra_coords);
                    first = false;
                }
            }

            if (cmd == CommandType::MOVE_TO && !paths.back().empty()) {
                if (paths.back().size() < paths.back().capacity()) {
                    // Assuming we had an invalid length before
                    // lets shrink to fit, just to make sure
                    // we don't have a large capacity vector
                    // just wasting memory
                    paths.back().shrink_to_fit();
                }
                paths.emplace_back();
                if (!is_point) {
                    first = true;
                }
            }

            x += protozero::decode_zigzag32(static_cast<std::uint32_t>(*start_itr++));
            y += protozero::decode_zigzag32(static_cast<std::uint32_t>(*start_itr++));
            float px = ::roundf(static_cast<float>(x) * scale);
            float py = ::roundf(static_cast<float>(y) * scale);
            static const float max_coord = static_cast<float>(std::numeric_limits<typename GeometryCollectionType::coordinate_type>::max());
            static const float min_coord = static_cast<float>(std::numeric_limits<typename GeometryCollectionType::coordinate_type>::min());

            if (px > max_coord ||
                px < min_coord ||
                py > max_coord ||
                py < min_coord
                ) {
                std::runtime_error("paths outside valid range of coordinate_type");
            } else {
                paths.back().emplace_back(
                    static_cast<typename GeometryCollectionType::coordinate_type>(px),
                    static_cast<typename GeometryCollectionType::coordinate_type>(py));
            }
        } else if (cmd == CommandType::CLOSE) {
            if (!paths.back().empty()) {
                paths.back().push_back(paths.back()[0]);
            }
            length = 0;
        } else {
            throw std::runtime_error("unknown command");
        }
    }
    if (paths.size() < paths.capacity()) {
        // Assuming we had an invalid length before
        // lets shrink to fit, just to make sure
        // we don't have a large capacity vector
        // just wasting memory
        paths.shrink_to_fit();
    }
#if defined(DEBUG)
    for (auto const& p : paths) {
        assert(p.size() == p.capacity());
    }
#endif
    return paths;
}

inline buffer::buffer(std::string const& data)
    : layers() {
        protozero::pbf_reader data_reader(data);
        while (data_reader.next(TileType::LAYERS)) {
            const protozero::data_view layer_view = data_reader.get_view();
            protozero::pbf_reader layer_reader(layer_view);
            std::string name;
            bool has_name = false;
            while (layer_reader.next(LayerType::NAME)) {
                name = layer_reader.get_string();
                has_name = true;
            }
            if (!has_name) {
                throw std::runtime_error("Layer missing name");
            }
            layers.emplace(name, layer_view);
        }
}

inline std::vector<std::string> buffer::layerNames() const {
    std::vector<std::string> names;
    names.reserve(layers.size());
    for (auto const& layer : layers) {
        names.emplace_back(layer.first);
    }
    return names;
}

inline layer buffer::getLayer(const std::string& name) const {
    auto layer_it = layers.find(name);
    if (layer_it == layers.end()) {
        throw std::runtime_error(std::string("no layer by the name of '")+name+"'");
    }
    return layer(layer_it->second);
}

inline layer::layer(protozero::data_view const& layer_view) :
    name(),
    version(1),
    extent(4096),
    keysMap(),
    keys(),
    values(),
    features()
{
    bool has_name = false;
    bool has_extent = false;
    bool has_version = false;
    protozero::pbf_reader layer_pbf(layer_view);
    while (layer_pbf.next()) {
        switch (layer_pbf.tag()) {
        case LayerType::NAME:
            {
                name = layer_pbf.get_string();
                has_name = true;
            }
            break;
        case LayerType::FEATURES:
            {
                features.push_back(layer_pbf.get_view());
            }
            break;
        case LayerType::KEYS:
            {
                // We want to keep the keys in the order of the vector tile
                // https://github.com/mapbox/mapbox-gl-native/pull/5183
                auto iter = keysMap.emplace(layer_pbf.get_string(), keysMap.size());
                keys.emplace_back(std::reference_wrapper<const std::string>(iter.first->first));
            }
            break;
        case LayerType::VALUES:
            {
                values.emplace_back(layer_pbf.get_view());
            }
            break;
        case LayerType::EXTENT:
            {
                extent = layer_pbf.get_uint32();
                has_extent = true;
            }
            break;
        case LayerType::VERSION:
            {
                version = layer_pbf.get_uint32();
                has_version = true;
            }
            break;
        default:
            {
                layer_pbf.skip();
            }
            break;
        }
    }
    if (!has_version || !has_name || !has_extent) {
        std::string msg("missing required field:");
        if (!has_version) {
            msg += " version ";
        }
        if (!has_extent) {
            msg += " extent ";
        }
        if (!has_name) {
            msg += " name";
        }
        throw std::runtime_error(msg.c_str());
    }
}

inline protozero::data_view const& layer::getFeature(std::size_t i) const {
    return features.at(i);
}

inline std::string const& layer::getName() const {
    return name;
}

}} // namespace mapbox/vector_tile
