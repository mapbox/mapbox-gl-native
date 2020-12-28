#pragma once

namespace mbgl {
namespace platform {

// Called when a thread is created
void attachThread();

// Called when a thread is destroyed
void detachThread();

} // namespace platform
} // namespace mbgl
