#pragma once

#include <mbgl/util/optional.hpp>

#include <string>

namespace mbgl {
namespace style {
namespace conversion {

/*
   The `conversion` namespace defines conversions from a templated type `V` representing a JSON
   object conforming to the schema defined by the Mapbox Style Specification, to the various C++
   types that form the C++ model of that domain:

       * `std::unique_ptr<Source>`
       * `std::unique_ptr<Layer>`
       * `Filter`
       * `PropertyValue<T>`

   A single template function serves as the public interface:

       template <class T, class V>
       optional<T> convert(const V& value, Error& error);

   Where `T` is one of the above types. If the conversion fails, the result is empty, and the
   error parameter includes diagnostic text suitable for presentation to a library user. Otherwise,
   a filled optional is returned.

   The implementation of `convert` requires that the following are legal expressions for a value `v`
   of type `const V&`:

      * `isUndefined(v)` -- returns a boolean indication whether `v` is undefined or a JSON null

      * `isArray(v)` -- returns a boolean indicating whether `v` represents a JSON array
      * `arrayLength(v)` -- called only if `isArray(v)`; returns a size_t length
      * `arrayMember(v)` -- called only if `isArray(v)`; returns `V` or `V&`

      * `isObject(v)` -- returns a boolean indicating whether `v` represents a JSON object
      * `objectMember(v, name)` -- called only if `isObject(v)`; `name` is `const char *`; return value:
         * is true when evaluated in a boolean context iff the named member exists
         * is convertable to a `V` or `V&` when dereferenced
      * `eachMember(v, [] (const std::string&, const V&) -> optional<Error> {...})` -- called
         only if `isObject(v)`; calls the provided lambda once for each key and value of the object;
         short-circuits if any call returns an `Error`

      * `toBool(v)` -- returns `optional<bool>`, absence indicating `v` is not a JSON boolean
      * `toNumber(v)` -- returns `optional<float>`, absence indicating `v` is not a JSON number
      * `toString(v)` -- returns `optional<std::string>`, absence indicating `v` is not a JSON string
      * `toValue(v)` -- returns `optional<mbgl::Value>`, a variant type, for generic conversion,
        absence indicating `v` is not a boolean, number, or string. Numbers should be converted to
        unsigned integer, signed integer, or floating point, in descending preference.

   The mbgl core implements these requirements for RapidJSON types, and the node bindings implement
   them for v8 types.
*/

struct Error { std::string message; };

template <class T, class Enable = void>
struct Converter;

template <class T, class V, class...Args>
optional<T> convert(const V& value, Error& error, Args&&...args) {
    return Converter<T>()(value, error, std::forward<Args>(args)...);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
