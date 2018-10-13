#pragma once

#include <mbgl/util/string.hpp>

namespace mbgl {

template <typename>
class ActorRef;
class FileSourceRequest;

void requestLocalFile(const std::string&, ActorRef<FileSourceRequest>);

} // namespace mbgl
