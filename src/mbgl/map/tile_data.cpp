#include <mbgl/map/tile_data.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/work_request.hpp>

using namespace mbgl;

TileData::TileData(const TileID& id_, const SourceInfo& source_)
    : id(id_),
      name(id),
      source(source_),
      state(State::initial),
      debugBucket(debugFontBuffer) {
    // Initialize tile debug coordinates
    debugFontBuffer.addText(name.c_str(), 50, 200, 5);
}

TileData::~TileData() {
    cancel();
}

const std::string TileData::toString() const {
    return std::string { "[tile " } + name + "]";
}

void TileData::setState(const State& state_) {
    assert(!isImmutable());

    state = state_;
}

void TileData::cancel() {
    if (state != State::obsolete) {
        state = State::obsolete;
    }
    if (req) {
        util::ThreadContext::getFileSource()->cancel(req);
        req = nullptr;
    }
    workRequest.reset();
}

bool TileData::mayStartParsing() {
    return !parsing.test_and_set(std::memory_order_acquire);
}

void TileData::endParsing() {
    parsing.clear(std::memory_order_release);
}

void TileData::setError(const std::string& message) {
    error = message;
    setState(State::obsolete);
}
