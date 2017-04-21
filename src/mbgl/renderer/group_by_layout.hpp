#pragma once

#include <vector>
#include <memory>

namespace mbgl {

class RenderLayer;

std::vector<std::vector<const RenderLayer*>> groupByLayout(const std::vector<std::unique_ptr<RenderLayer>>&);

} // namespace mbgl
