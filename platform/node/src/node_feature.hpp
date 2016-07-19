#pragma once

#include <mbgl/util/feature.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

namespace node_mbgl {

v8::Local<v8::Value> toJS(const mbgl::Value&);
v8::Local<v8::Object> toJS(const mbgl::Feature&);
v8::Local<v8::Object> toJS(const mbgl::Feature::geometry_type&);
v8::Local<v8::Object> toJS(const mbgl::PropertyMap&);

}
