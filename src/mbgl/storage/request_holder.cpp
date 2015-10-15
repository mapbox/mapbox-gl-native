#include <mbgl/storage/request_holder.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/thread_context.hpp>

namespace mbgl {

void RequestHolder::Deleter::operator()(Request* req) const {
    // This function is called by the unique_ptr's Deleter.
    util::ThreadContext::getFileSource()->cancel(req);
}

}
