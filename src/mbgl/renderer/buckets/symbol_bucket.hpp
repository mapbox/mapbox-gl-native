#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/symbol_program.hpp>
#include <mbgl/programs/collision_box_program.hpp>
#include <mbgl/text/glyph_range.hpp>
#include <mbgl/style/layers/symbol_layer_properties.hpp>
#include <mbgl/layout/symbol_feature.hpp>
#include <mbgl/layout/symbol_instance.hpp>

#include <vector>

namespace mbgl {

class CrossTileSymbolLayerIndex;

class PlacedSymbol {
public:
    PlacedSymbol(Point<float> anchorPoint_, uint16_t segment_, float lowerSize_, float upperSize_,
            std::array<float, 2> lineOffset_, WritingModeType writingModes_, GeometryCoordinates line_, std::vector<float> tileDistances_) :
        anchorPoint(anchorPoint_), segment(segment_), lowerSize(lowerSize_), upperSize(upperSize_),
        lineOffset(lineOffset_), writingModes(writingModes_), line(std::move(line_)), tileDistances(std::move(tileDistances_)), hidden(false), vertexStartIndex(0)
    {
    }
    Point<float> anchorPoint;
    uint16_t segment;
    float lowerSize;
    float upperSize;
    std::array<float, 2> lineOffset;
    WritingModeType writingModes;
    GeometryCoordinates line;
    std::vector<float> tileDistances;
    std::vector<float> glyphOffsets;
    bool hidden;
    size_t vertexStartIndex;
    // The crossTileID is only filled/used on the foreground for variable text anchors
    uint32_t crossTileID = 0u;
};

class SymbolBucket final : public Bucket {
public:
    SymbolBucket(Immutable<style::SymbolLayoutProperties::PossiblyEvaluated>,
                 const std::map<std::string, Immutable<style::LayerProperties>>&,
                 const style::PropertyValue<float>& textSize,
                 const style::PropertyValue<float>& iconSize,
                 float zoom,
                 bool sdfIcons,
                 bool iconsNeedLinear,
                 bool sortFeaturesByY,
                 const std::string bucketLeaderID,
                 const std::vector<SymbolInstance>&&,
                 const float tilePixelRatio);
    ~SymbolBucket() override;

    void upload(gfx::UploadPass&) override;
    bool hasData() const override;
    std::pair<uint32_t, bool> registerAtCrossTileIndex(CrossTileSymbolLayerIndex&, const OverscaledTileID&, uint32_t& maxCrossTileID) override;
    void place(Placement&, const BucketPlacementParameters&, std::set<uint32_t>&) override;
    void updateVertices(Placement&, bool updateOpacities, const TransformState&, const RenderTile&, std::set<uint32_t>&) override;
    bool hasTextData() const;
    bool hasIconData() const;
    bool hasCollisionBoxData() const;
    bool hasCollisionCircleData() const;
    bool hasFormatSectionOverrides() const;


    void sortFeatures(const float angle);
    // The result contains references to the `symbolInstances` items, sorted by viewport Y.
    std::vector<std::reference_wrapper<SymbolInstance>> getSortedSymbols(const float angle);

    Immutable<style::SymbolLayoutProperties::PossiblyEvaluated> layout;
    const std::string bucketLeaderID;
    float sortedAngle = std::numeric_limits<float>::max();

    // Flags
    const bool sdfIcons : 1;
    const bool iconsNeedLinear : 1;
    const bool sortFeaturesByY : 1;
    bool staticUploaded : 1;
    bool placementChangesUploaded : 1;
    bool dynamicUploaded : 1;
    bool sortUploaded : 1;
    bool justReloaded : 1;
    bool hasVariablePlacement : 1;

    std::vector<SymbolInstance> symbolInstances;

    struct PaintProperties {
        SymbolIconProgram::Binders iconBinders;
        SymbolSDFTextProgram::Binders textBinders;
    };
    std::map<std::string, PaintProperties> paintProperties;

    std::unique_ptr<SymbolSizeBinder> textSizeBinder;

    struct Buffer {
        gfx::VertexVector<SymbolLayoutVertex> vertices;
        gfx::VertexVector<gfx::Vertex<SymbolDynamicLayoutAttributes>> dynamicVertices;
        gfx::VertexVector<gfx::Vertex<SymbolOpacityAttributes>> opacityVertices;
        gfx::IndexVector<gfx::Triangles> triangles;
        SegmentVector<SymbolTextAttributes> segments;
        std::vector<PlacedSymbol> placedSymbols;

        optional<gfx::VertexBuffer<SymbolLayoutVertex>> vertexBuffer;
        optional<gfx::VertexBuffer<gfx::Vertex<SymbolDynamicLayoutAttributes>>> dynamicVertexBuffer;
        optional<gfx::VertexBuffer<gfx::Vertex<SymbolOpacityAttributes>>> opacityVertexBuffer;
        optional<gfx::IndexBuffer> indexBuffer;
    } text;

    std::unique_ptr<SymbolSizeBinder> iconSizeBinder;

    struct IconBuffer : public Buffer {
    } icon;

    struct CollisionBuffer {
        gfx::VertexVector<gfx::Vertex<CollisionBoxLayoutAttributes>> vertices;
        gfx::VertexVector<gfx::Vertex<CollisionBoxDynamicAttributes>> dynamicVertices;
        SegmentVector<CollisionBoxProgram::AttributeList> segments;

        optional<gfx::VertexBuffer<gfx::Vertex<CollisionBoxLayoutAttributes>>> vertexBuffer;
        optional<gfx::VertexBuffer<gfx::Vertex<CollisionBoxDynamicAttributes>>> dynamicVertexBuffer;
    };

    struct CollisionBoxBuffer : public CollisionBuffer {
        gfx::IndexVector<gfx::Lines> lines;
        optional<gfx::IndexBuffer> indexBuffer;
    };
    std::unique_ptr<CollisionBoxBuffer> collisionBox;

    CollisionBoxBuffer& getOrCreateCollisionBox() {
        if (!collisionBox) collisionBox = std::make_unique<CollisionBoxBuffer>();
        return *collisionBox;
    }

    struct CollisionCircleBuffer : public CollisionBuffer {
        gfx::IndexVector<gfx::Triangles> triangles;
        optional<gfx::IndexBuffer> indexBuffer;
    };
    std::unique_ptr<CollisionCircleBuffer> collisionCircle;

    CollisionCircleBuffer& getOrCreateCollisionCircleBuffer() {
        if (!collisionCircle) collisionCircle = std::make_unique<CollisionCircleBuffer>();
        return *collisionCircle;
    }

    const float tilePixelRatio;
    uint32_t bucketInstanceId;

    mutable optional<bool> hasFormatSectionOverrides_;

    std::shared_ptr<std::vector<size_t>> featureSortOrder;
};

} // namespace mbgl
