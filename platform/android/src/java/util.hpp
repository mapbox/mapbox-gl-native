#pragma once

#include "lang.hpp"

#include <mbgl/util/noncopyable.hpp>

namespace mbgl {
namespace android {
namespace java {
namespace util {

struct ListTag { static constexpr auto Name() { return "java/util/List"; } };
using List = jni::Object<ListTag>;
struct List_toArrayTag { static constexpr auto Name() { return "toArray"; } };
using List_toArray = binding::Method<ListTag, List_toArrayTag, jni::Array<jni::Object<>>(void)>;

struct MapTag { static constexpr auto Name() { return "java/util/Map"; } };
using Map = jni::Object<MapTag>;
struct Map_getTag { static constexpr auto Name() { return "get"; } };
using Map_get = binding::Method<MapTag, Map_getTag, jni::Object<>(jni::Object<>)>;

struct MapEntryTag { static constexpr auto Name() { return "java/util/Map$Entry"; }; };
using MapEntry = jni::Object<MapEntryTag>;
struct MapEntry_getKeyTag { static constexpr auto Name() { return "getKey"; } };
using MapEntry_getKey = binding::Method<MapEntryTag, MapEntry_getKeyTag, jni::Object<>(void)>;
struct MapEntry_getValueTag { static constexpr auto Name() { return "getValue"; } };
using MapEntry_getValue = binding::Method<MapEntryTag, MapEntry_getValueTag, jni::Object<>(void)>;

struct SetTag { static constexpr auto Name() { return "java/util/Set"; } };
using Set = jni::Object<SetTag>;
struct Set_toArrayTag { static constexpr auto Name() { return "toArray"; } };
using Set_toArray = binding::Method<SetTag, Set_toArrayTag, jni::Array<jni::Object<>>(void)>;

} // namespace util
} // namespace java
} // namespace android
} // namespace mbgl