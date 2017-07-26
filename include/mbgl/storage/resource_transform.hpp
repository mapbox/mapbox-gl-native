#pragma once

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/storage/resource.hpp>

#include <functional>
#include <string>

namespace mbgl {

class Mailbox;

class ResourceTransform {
public:
    using TransformCallback = std::function<std::string(ResourceKind kind, const std::string&& url)>;
    using FinishedCallback = std::function<void(const std::string&&)>;

    ResourceTransform(ActorRef<ResourceTransform>, TransformCallback&&);

    void transform(ResourceKind, const std::string&& url, FinishedCallback&&);

private:
    TransformCallback transformCallback;
};

} // namespace mbgl
