#include <mbgl/storage/resource_transform.hpp>

namespace mbgl {

ResourceTransform::ResourceTransform(ActorRef<ResourceTransform>, TransformCallback&& callback)
    : transformCallback(std::move(callback)) {
}

void ResourceTransform::transform(Resource::Kind kind, const std::string&& url, FinishedCallback&& finished) {
    finished(transformCallback(kind, std::move(url)));
}

} // namespace mbgl
