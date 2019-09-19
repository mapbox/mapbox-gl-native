#include "node_feature.hpp"

namespace node_mbgl {

using namespace mapbox::geometry;

using Value = mbgl::Value;
using Feature = mbgl::Feature;
using FeatureIdentifier = mbgl::FeatureIdentifier;
using Geometry = mbgl::Feature::geometry_type;
using GeometryCollection = mapbox::geometry::geometry_collection<double>;
using Properties = mbgl::PropertyMap;

template <class T>
struct ToType {
public:
    v8::Local<v8::String> operator()(const empty&) {
        return type("Empty");
    }

    v8::Local<v8::String> operator()(const point<T>&) {
        return type("Point");
    }

    v8::Local<v8::String> operator()(const line_string<T>&) {
        return type("LineString");
    }

    v8::Local<v8::String> operator()(const polygon<T>&) {
        return type("Polygon");
    }

    v8::Local<v8::String> operator()(const multi_point<T>&) {
        return type("MultiPoint");
    }

    v8::Local<v8::String> operator()(const multi_line_string<T>&) {
        return type("MultiLineString");
    }

    v8::Local<v8::String> operator()(const multi_polygon<T>&) {
        return type("MultiPolygon");
    }

    v8::Local<v8::String> operator()(const geometry_collection<T>&) {
        return type("GeometryCollection");
    }

private:
    v8::Local<v8::String> type(const char* type_) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::New(type_).ToLocalChecked());
    }
};

template <class T>
struct ToCoordinatesOrGeometries {
public:
    // Handles line_string, polygon, multi_point, multi_line_string, multi_polygon, and geometry_collection.
    template <class E>
    v8::Local<v8::Object> operator()(const std::vector<E>& vector) {
        Nan::EscapableHandleScope scope;
        v8::Local<v8::Array> result = Nan::New<v8::Array>(vector.size());
        for (std::size_t i = 0; i < vector.size(); ++i) {
            Nan::Set(result, i, operator()(vector[i]));
        }
        return scope.Escape(result);
    }

    v8::Local<v8::Object> operator()(const point<T>& point) {
        Nan::EscapableHandleScope scope;
        v8::Local<v8::Array> result = Nan::New<v8::Array>(2);
        Nan::Set(result, 0, Nan::New(point.x));
        Nan::Set(result, 1, Nan::New(point.y));
        return scope.Escape(result);
    }

    v8::Local<v8::Object> operator()(const geometry<T>& geometry) {
        return toJS(geometry);
    }
};

struct ToValue {
    v8::Local<v8::Value> operator()(mbgl::NullValue) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::Null());
    }

    v8::Local<v8::Value> operator()(bool t) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::New(t));
    }

    v8::Local<v8::Value> operator()(int64_t t) {
        return operator()(double(t));
    }

    v8::Local<v8::Value> operator()(uint64_t t) {
        return operator()(double(t));
    }

    v8::Local<v8::Value> operator()(double t) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::New(t));
    }

    v8::Local<v8::Value> operator()(const std::string& t) {
        Nan::EscapableHandleScope scope;
        return scope.Escape(Nan::New(t).ToLocalChecked());
    }

    v8::Local<v8::Value> operator()(const std::vector<mbgl::Value>& array) {
        Nan::EscapableHandleScope scope;
        v8::Local<v8::Array> result = Nan::New<v8::Array>();
        for (unsigned int i = 0; i < array.size(); i++) {
            result->Set(i, toJS(array[i]));
        }
        return scope.Escape(result);
    }

    v8::Local<v8::Value> operator()(const std::unordered_map<std::string, mbgl::Value>& map) {
        return toJS(map);
    }
};

v8::Local<v8::Object> toJS(const Geometry& geometry) {
    Nan::EscapableHandleScope scope;

    v8::Local<v8::Object> result = Nan::New<v8::Object>();

    Nan::Set(result,
        Nan::New("type").ToLocalChecked(),
        Geometry::visit(geometry, ToType<double>()));

    Nan::Set(result,
        Nan::New(geometry.is<GeometryCollection>() ? "geometries" : "coordinates").ToLocalChecked(),
        Geometry::visit(geometry, ToCoordinatesOrGeometries<double>()));

    return scope.Escape(result);
}

v8::Local<v8::Value> toJS(const Value& value) {
    return Value::visit(value, ToValue());
}

v8::Local<v8::Object> toJS(const Properties& properties) {
    Nan::EscapableHandleScope scope;

    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    for (const auto& property : properties) {
        Nan::Set(result, Nan::New(property.first).ToLocalChecked(), toJS(property.second));
    }

    return scope.Escape(result);
}

v8::Local<v8::Object> toJS(const Feature& feature) {
    Nan::EscapableHandleScope scope;

    v8::Local<v8::Object> result = Nan::New<v8::Object>();

    Nan::Set(result, Nan::New("type").ToLocalChecked(), Nan::New("Feature").ToLocalChecked());
    Nan::Set(result, Nan::New("geometry").ToLocalChecked(), toJS(feature.geometry));
    Nan::Set(result, Nan::New("properties").ToLocalChecked(), toJS(feature.properties));

    if (!feature.id.is<mbgl::NullValue>()) {
        Nan::Set(result, Nan::New("id").ToLocalChecked(), FeatureIdentifier::visit(feature.id, ToValue()));
    }

    Nan::Set(result, Nan::New("source").ToLocalChecked(), toJS(feature.source));
    if (!feature.sourceLayer.empty()) {
        Nan::Set(result, Nan::New("sourceLayer").ToLocalChecked(), toJS(feature.sourceLayer));
    }
    Nan::Set(result, Nan::New("state").ToLocalChecked(), toJS(feature.state));

    return scope.Escape(result);
}

} // namespace node_mbgl
