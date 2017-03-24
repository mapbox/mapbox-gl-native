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
  static Geometry toGeometry(JNIEnv& env, jni::Object<java::util::List> pointsList, jni::Object<java::util::List> holePointsList) {
      NullCheck(env, &pointsList);
      auto jarray = java::util::List::toArray<LatLng>(env, pointsList);
      NullCheck(env, &jarray);

      std::size_t jsize = 0;
      std::size_t ksize = 0;
      auto karray = java::util::List::toArray<LatLng>(env, holePointsList);
      if (holePointsList) {
          ksize = karray.Length(env);
      } else {
          jsize = jarray.Length(env);
      }

      Geometry geometry;
      geometry.reserve(jsize + ksize);

      for (std::size_t i = 0; i < jsize; i++) {
          auto latLng = jarray.Get(env, i);
          NullCheck(env, &latLng);

          geometry.push_back(LatLng::getGeometry(env, latLng));

          jni::DeleteLocalRef(env, latLng);
      }

      jni::DeleteLocalRef(env, jarray);

      for (std::size_t i = 0; i < ksize; i++) {
          auto latLng = karray.Get(env, i);
          NullCheck(env, &latLng);

          geometry.push_back(LatLng::getGeometry(env, latLng));

          jni::DeleteLocalRef(env, latLng);
      }

      jni::DeleteLocalRef(env, karray);
      return geometry;
  }
};

} // namespace android
} // namespace mbgl