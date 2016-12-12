
# Upgrading

This file contains instructions for users of Protozero who are upgrading from
one version to another.

You do not need to change anything if only the minor version changes, but it
is better to keep up with changes if you can. The switch to the next major
version will be easier then. And you might get some more convenient usage.

To help you with upgrading to new versions, you can define the C++ preprocessor
macro `PROTOZERO_STRICT_API` in which case Protozero will compile without the
code used for backwards compatibilty. You will then get compile errors for
older API usages.

## Upgrading from *v1.4.0* to *v1.4.1*

* You can now do `require('protozero')` in nodejs to print the path
  to the include paths for the protozero headers.

## Upgrading from *v1.3.0* to *v1.4.0*

* Functions in `pbf_reader` (and the derived `pbf_message`) called
  `get_packed_*()` now return an `iterator_range` instead of a `std::pair`.
  The new class is derived from `std::pair`, so changes are usually not
  strictly necessary. For future compatibility, you should change all
  attribute accesses on the returned objects from `first` and `second` to
  `begin()` and `end()`, respectively. So change something like this:

      auto x = message.get_packed_int32();
      for (auto it = x.first; it != x.second; ++it) {
          ....
      }

  to:

      auto x = message.get_packed_int32();
      for (auto it = x.begin(); it != x.end(); ++it) {
          ....
      }

  or even better use the range-based for loop:

      auto x = message.get_packed_int32();
      for (auto val : x) {
          ....
      }

  Ranges can also be used in this way. This will change the range in-place:

      auto range = message.get_packed_int32();
      while (!range.empty()) {
          auto value = range.front();
          range.drop_front();
          ....
      }

* The class `pbf_reader` has a new method `get_view()` returning an object
  of the new `protozero::data_view` class. The `data_view` only has minimal
  functionality, but what it has is compatible to the `std::string_view` class
  which will be coming in C++17. The view autoconverts to a `std::string` if
  needed. Use `get_view()` instead of `get_data()` giving you a more intuitive
  interface (call `data()` and `size()` on the view instead of using `first`
  and `second` on the `std::pair` returned by `get_data()`).

  You can set the macro `PROTOZERO_USE_VIEW` (before including `types.hpp`) to
  the name of any class that behaves like `protozero::data_view` and
  `data_view` will be an alias to that class instead of the implementation
  from protozero. This way you can use the C++17 `string_view` or a similar
  class if it is already available on your system.

