#include <llmr/renderer/line_bucket.hpp>
#include <llmr/geometry/line_buffer.hpp>
#include <llmr/geometry/geometry.hpp>

#include <llmr/renderer/painter.hpp>
#include <llmr/style/style.hpp>

#include <llmr/platform/gl.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include <cassert>

struct geometry_too_long_exception : std::exception {};

using namespace llmr;

LineBucket::LineBucket(const std::shared_ptr<LineBuffer>& buffer)
    : buffer(buffer) {
}

void LineBucket::addGeometry(pbf& geom) {

}

void LineBucket::render(Painter& painter, const std::string& layer_name) {
    painter.renderLine(*this, layer_name);
}
