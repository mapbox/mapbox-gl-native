#!/usr/bin/env bash
source "$(dirname "${BASH_SOURCE[0]}")/common.sh"

NAME=optional
VERSION=fe8fcaf41f271307936289d26846738182946150
ROOT=optional-lite-$VERSION

download "https://github.com/martinmoene/optional-lite/archive/$VERSION.tar.gz"
init
extract_gzip "$ROOT/include" "$ROOT/LICENSE.txt"
mkdir -p "include/experimental"
cat << EOF > "include/experimental/optional"
#pragma once

#include <nonstd/optional.hpp>

namespace std {
namespace experimental {

template <typename T>
using optional = nonstd::optional<T>;

using nullopt_t = nonstd::nullopt_t;
constexpr nullopt_t nullopt = nonstd::nullopt;

} // namespace experimental
} // namespace std
EOF
file_list include -type f
