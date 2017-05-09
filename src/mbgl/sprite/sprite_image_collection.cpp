#include <mbgl/sprite/sprite_image_collection.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

void addSpriteImage(Images& images, const std::string& id,
                           std::unique_ptr<style::Image> image_,
                           std::function<void (style::Image&)> onAdded) {

    auto it = images.find(id);
    if (it == images.end()) {
        // Add new
        it = images.emplace(id, std::move(image_)).first;
        onAdded(*it->second.get());
        return;
    }

    std::unique_ptr<style::Image>& image = it->second;

    // There is already a sprite with that name in our store.
    if (image->getImage().size != image_->getImage().size) {
        Log::Warning(Event::Sprite, "Can't change sprite dimensions for '%s'", id.c_str());
    }

    // Update existing
    image = std::move(image_);
    onAdded(*it->second.get());
}

void removeSpriteImage(Images& images,
                              const std::string& id,
                              std::function<void ()> onRemoved) {
    if (images.erase(id) > 0) {
        onRemoved();
    }
}



} // namespace mbgl
