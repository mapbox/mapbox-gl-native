
# Change Log

All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [unreleased] -

### Added

### Changed

### Fixed


## [1.4.2] - 2016-08-27

### Fixed

- Compile fix: Variable shadowing.


## [1.4.1] - 2016-08-21

### Fixed

- GCC 4.8 compile fixed

### Added

- New ability to dynamically require the module as a node module to ease
  building against from other node C++ modules.

## [1.4.0] - 2016-07-22

### Changed

- Use more efficient new `skip_varint()` function when iterating over
  packed varints.
- Split `decode_varint()` function into two functions speeding up the
  common case where a varint is only one byte long.
- Introduce new class `iterator_range` used instead of `std::pair` of
  iterators. This way the objects can be used in range-based for loops.
  Read UPGRADING.md for details.
- Introduce new class `data_view` and functions using and returning it.
  Read UPGRADING.md for details.


## [1.3.0] - 2016-02-18

### Added

- Added `config.hpp` header which now includes all the macro magic to
  configure the library for different architectures etc.
- New way to create repeated packed fields without using an iterator.
- Add `rollback()` function to `pbf_writer` for "manual" rollback.

### Changed

- Various test and documentation cleanups.
- Rename `pbf_types.hpp` to `types.hpp`.


## [1.2.3] - 2015-11-30

### Added

- Added `config.hpp` header which now includes all the macro magic to
  configure the library for different architectures etc.

### Fixed

- Unaligned access to floats/doubles on some ARM architectures.


## [1.2.2] - 2015-10-13

### Fixed

- Fix the recently broken writing of bools on big-endian architectures.


## [1.2.1] - 2015-10-12

### Fixed

- Removed unneeded code (1-byte "swap") which lead to test failures.


## [1.2.0] - 2015-10-08

### Added

- `pbf_message` and `pbf_builder` template classes wrapping `pbf_reader`
  and `pbf_writer`, respectively. The new classes are the preferred
  interface now.

### Changed

- Improved byte swapping operation.
- Detect some types of data corruption earlier and throw.


## [1.1.0] - 2015-08-22

### Changed

- Make pbf reader and writer code endianess-aware.


[unreleased]: https://github.com/osmcode/libosmium/compare/v1.4.2...HEAD
[1.4.2]: https://github.com/osmcode/libosmium/compare/v1.4.1...v1.4.2
[1.4.1]: https://github.com/osmcode/libosmium/compare/v1.4.0...v1.4.1
[1.4.0]: https://github.com/osmcode/libosmium/compare/v1.3.0...v1.4.0
[1.3.0]: https://github.com/osmcode/libosmium/compare/v1.2.3...v1.3.0
[1.2.3]: https://github.com/osmcode/libosmium/compare/v1.2.2...v1.2.3
[1.2.2]: https://github.com/osmcode/libosmium/compare/v1.2.1...v1.2.2
[1.2.1]: https://github.com/osmcode/libosmium/compare/v1.2.0...v1.2.1
[1.2.0]: https://github.com/osmcode/libosmium/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/osmcode/libosmium/compare/v1.0.0...v1.1.0

