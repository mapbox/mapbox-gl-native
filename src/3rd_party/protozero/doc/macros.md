
# Macros

## Version number

If `protozero/version.hpp` is included, the following macros are set:

| Macro                      | Example | Description                                    |
| -------------------------- | ------- | ---------------------------------------------- |
| `PROTOZERO_VERSION_MAJOR`  | 1       | Major version number                           |
| `PROTOZERO_VERSION_MINOR`  | 3       | Minor version number                           |
| `PROTOZERO_VERSION_PATCH`  | 2       | Patch number                                   |
| `PROTOZERO_VERSION_CODE`   | 10302   | Version (major * 10,000 + minor * 100 + patch) |
| `PROTOZERO_VERSION_STRING` | "1.3.2" | Version string                                 |

## Changing Protozero behaviour

The behaviour of Protozero can be changed by defining the following macros.
They have to be set before including any of the Protozero headers.

## `PROTOZERO_STRICT_API`

If this is set, you will get some extra warnings or errors during compilation
if you are using an old (deprecated) interface to Protozero. Enable this if
you want to make sure your code will work with future versions of Protozero.

## `PROTOZERO_USE_VIEW`

If this is unset `protozero::data_view` is Protozero's own implementation of
a *string view* class.

Set this if you want to use a different implementation such as the C++17
`std::string_view` class. In this case `protozero::data_view` will simply be
an alias to the class you specify.

    #define PROTOZERO_USE_VIEW std::string_view

This affects the result type of the `pbf_reader::get_view()` method and a few
others taking a `protozero::data_view` as parameters.

## `PROTOZERO_DO_NOT_USE_BARE_POINTER`

Can be set to force Protozero to not use bare pointers for some iterators
returned from `get_packed_*` calls. It is usually not necessary to use this
and might affect performance if you do. If you are getting errors about
unaligned memory access or a SIGBUS, you can try to set this. (Please also
report on error if this is the case.)

