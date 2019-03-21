#include <mbgl/gl/command_encoder.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {
namespace gl {

CommandEncoder::~CommandEncoder() {
    context.performCleanup();
}

} // namespace gl
} // namespace mbgl
