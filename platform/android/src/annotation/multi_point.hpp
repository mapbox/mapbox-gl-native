#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <jni/jni.hpp>

#include "../geometry/lat_lng.hpp"
#include "../java/lang.hpp"
#include "../java/util.hpp"

namespace mbgl {
namespace android {

class MultiPoint : protected mbgl::util::noncopyable {

protected:

  template <class Geometry>
  static Geometry toGeometry(JNIEnv& env, java::util::List pointsList) {
      NullCheck(env, &pointsList);
      auto jarray =
          (jni::Array<jni::Object<LatLng>>)java::util::List_toArray::Call(env, pointsList);
      NullCheck(env, &jarray);

      std::size_t size = jarray.Length(env);

      Geometry geometry;
      geometry.reserve(size);

      for (std::size_t i = 0; i < size; i++) {
          auto latLng = jarray.Get(env, i);
          NullCheck(env, &latLng);

          geometry.push_back(LatLng::getGeometry(env, latLng));

          jni::DeleteLocalRef(env, latLng);
      }

      jni::DeleteLocalRef(env, jarray);
      return geometry;
  }
};

} // namespace android
} // namespace mbgl