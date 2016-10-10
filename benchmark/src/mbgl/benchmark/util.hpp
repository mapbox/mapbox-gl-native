#pragma once

namespace mbgl {

class Map;
class OffscreenView;

namespace benchmark {

void render(Map&, OffscreenView&);

} // namespace benchmark
} // namespace mbgl
