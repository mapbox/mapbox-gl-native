#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <jni/jni.hpp>

#include "../geometry/lat_lng.hpp"
#include "../java/util.hpp"

namespace mbgl {
namespace android {

class MultiPoint : protected mbgl::util::noncopyable {

protected:

  template <class Geometry>
  static Geometry toGeometry(JNIEnv& env, const jni::Object<java::util::List>& pointsList) {
      auto jarray = java::util::List::toArray<LatLng>(env, pointsList);

      std::size_t size = jarray.Length(env);

      Geometry geometry;
      geometry.reserve(size);

      for (std::size_t i = 0; i < size; i++) {
          geometry.push_back(LatLng::getGeometry(env, jarray.Get(env, i)));
      }

      return geometry;
  }
};

} // namespace android
} // namespace mbgl