#pragma once

#include <mbgl/style/sources/custom_vector_source.hpp>
#include <mbgl/style/source_impl.hpp>

namespace mbgl {
  namespace style {
    
    class CustomVectorSource::Impl : public Source::Impl {
    public:
      Impl(std::string id, Source&, CustomVectorSourceOptions options, std::function<void(uint8_t, uint32_t, uint32_t)> fetchTile);
      
      uint16_t getTileSize() const final {
        return util::tileSize;
      }
      void loadDescription(FileSource&) final {}

    private:
//      std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;
      CustomVectorSourceOptions options;
      std::function<void(uint8_t, uint32_t, uint32_t)> fetchTile;
      
      Range<uint8_t> getZoomRange() final;
      std::unique_ptr<Tile> createTile(const OverscaledTileID&, const UpdateParameters&) final;
      
    };
    
  } // namespace style
} // namespace mbgl
