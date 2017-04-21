#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_source_observer.hpp>
#include <mbgl/tile/tile.hpp>

namespace mbgl {

static RenderSourceObserver nullObserver;

RenderSource::RenderSource(const style::Source::Impl& impl)
    : baseImpl(impl),
      observer(&nullObserver) {
}

void RenderSource::setObserver(RenderSourceObserver* observer_) {
    observer = observer_;
}

void RenderSource::onTileChanged(Tile& tile) {
    observer->onTileChanged(*this, tile.id);
}

void RenderSource::onTileError(Tile& tile, std::exception_ptr error) {
    observer->onTileError(*this, tile.id, error);
}

}
