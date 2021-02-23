#pragma once

#include <mbgl/storage/resource.hpp>

#include <functional>
#include <string>

namespace mbgl {

class ResourceTransform {
public:
    using FinishedCallback = std::function<void(const std::string&)>;
    using TransformCallback = std::function<void(Resource::Kind kind, const std::string& url, FinishedCallback)>;

    ResourceTransform(TransformCallback = {});
    void transform(Resource::Kind, const std::string& url, FinishedCallback);
    explicit operator bool() const { return bool(transformCallback); }

private:
    TransformCallback transformCallback;
};

} // namespace mbgl
