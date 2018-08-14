#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/layout/merge_lines.hpp>
#include <mbgl/layout/clip_lines.hpp>
#include <mbgl/renderer/buckets/symbol_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/style/layers/symbol_layer_impl.hpp>
#include <mbgl/text/get_anchors.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/utf.hpp>
#include <mbgl/util/std.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/math/minmax.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>

#include <mapbox/polylabel.hpp>

namespace mbgl {

using namespace style;

template <class T>
static bool has(const PossiblyEvaluatedPropertyValue<T>& value) {
    return value.match(
        [&] (const T& t) { return !t.empty(); },
        [&] (const auto&) { return true; }
    );
}

SymbolLayout::SymbolLayout(const BucketParameters& parameters,
                           const std::vector<const RenderLayer*>& layers,
                           std::unique_ptr<GeometryTileLayer> sourceLayer_,
                           ImageDependencies& imageDependencies,
                           GlyphDependencies& glyphDependencies)
    : bucketLeaderID(layers.at(0)->getID()),
      sourceLayer(std::move(sourceLayer_)),
      overscaling(parameters.tileID.overscaleFactor()),
      zoom(parameters.tileID.overscaledZ),
      mode(parameters.mode),
      pixelRatio(parameters.pixelRatio),
      tileSize(util::tileSize * overscaling),
      tilePixelRatio(float(util::EXTENT) / tileSize),
      textSize(layers.at(0)->as<RenderSymbolLayer>()->impl().layout.textSize),
      iconSize(layers.at(0)->as<RenderSymbolLayer>()->impl().layout.iconSize)
    {

    const SymbolLayer::Impl& leader = layers.at(0)->as<RenderSymbolLayer>()->impl();

    layout = leader.layout.evaluate(PropertyEvaluationParameters(zoom));

    if (layout.iconRotationAlignment == AlignmentType::Auto) {
        if (layout.symbolPlacement != SymbolPlacementType::Point) {
            layout.iconRotationAlignment = AlignmentType::Map;
        } else {
            layout.iconRotationAlignment = AlignmentType::Viewport;
        }
    }

    if (layout.textRotationAlignment == AlignmentType::Auto) {
        if (layout.symbolPlacement != SymbolPlacementType::Point) {
            layout.textRotationAlignment = AlignmentType::Map;
        } else {
            layout.textRotationAlignment = AlignmentType::Viewport;
        }
    }

    // If unspecified `*-pitch-alignment` inherits `*-rotation-alignment`
    if (layout.textPitchAlignment == AlignmentType::Auto) {
        layout.textPitchAlignment = layout.textRotationAlignment;
    }
    if (layout.iconPitchAlignment == AlignmentType::Auto) {
        layout.iconPitchAlignment = layout.iconRotationAlignment;
    }

    const bool hasText = has(layout.textField) && has(layout.textFont);
    const bool hasIcon = has(layout.iconImage);

    if (!hasText && !hasIcon) {
        return;
    }

    for (const auto& layer : layers) {
        layerPaintProperties.emplace(layer->getID(), std::make_pair(
            layer->as<RenderSymbolLayer>()->iconPaintProperties(),
            layer->as<RenderSymbolLayer>()->textPaintProperties()
        ));
    }

    // Determine glyph dependencies
    const size_t featureCount = sourceLayer->featureCount();
    for (size_t i = 0; i < featureCount; ++i) {
        auto feature = sourceLayer->getFeature(i);
        if (!leader.filter(expression::EvaluationContext { this->zoom, feature.get() }))
            continue;
        
        SymbolFeature ft(std::move(feature));

        ft.index = i;

        if (hasText) {
            std::string u8string = layout.textField.evaluate(ft, zoom, TextField::defaultValue());

            auto textTransform = layout.textTransform.evaluate(ft, zoom, TextTransform::defaultValue());
            if (textTransform == TextTransformType::Uppercase) {
                u8string = platform::uppercase(u8string);
            } else if (textTransform == TextTransformType::Lowercase) {
                u8string = platform::lowercase(u8string);
            }

            ft.text = applyArabicShaping(util::utf8_to_utf16::convert(u8string));
            const bool canVerticalizeText = layout.textRotationAlignment == AlignmentType::Map
                                         && layout.symbolPlacement != SymbolPlacementType::Point
                                         && util::i18n::allowsVerticalWritingMode(*ft.text);

            FontStack fontStack = layout.textFont.evaluate(ft, zoom, TextFont::defaultValue());
            GlyphIDs& dependencies = glyphDependencies[fontStack];

            // Loop through all characters of this text and collect unique codepoints.
            for (char16_t chr : *ft.text) {
                dependencies.insert(chr);
                if (canVerticalizeText) {
                    if (char16_t verticalChr = util::i18n::verticalizePunctuation(chr)) {
                        dependencies.insert(verticalChr);
                    }
                }
            }
        }

        if (hasIcon) {
            ft.icon = layout.iconImage.evaluate(ft, zoom, IconImage::defaultValue());
            imageDependencies.insert(*ft.icon);
        }

        if (ft.text || ft.icon) {
            features.push_back(std::move(ft));
        }
    }

    if (layout.symbolPlacement == SymbolPlacementType::Line) {
        util::mergeLines(features);
    }
}

bool SymbolLayout::hasSymbolInstances() const {
    return !symbolInstances.empty();
}

void SymbolLayout::prepare(const GlyphMap& glyphMap, const GlyphPositions& glyphPositions,
                           const ImageMap& imageMap, const ImagePositions& imagePositions) {
    const bool textAlongLine = layout.textRotationAlignment == AlignmentType::Map &&
        layout.symbolPlacement != SymbolPlacementType::Point;

    for (auto it = features.begin(); it != features.end(); ++it) {
        auto& feature = *it;
        if (feature.geometry.empty()) continue;

        FontStack fontStack = layout.textFont.evaluate(feature, zoom, TextFont::defaultValue());

        auto glyphMapIt = glyphMap.find(fontStack);
        const Glyphs& glyphs = glyphMapIt != glyphMap.end()
            ? glyphMapIt->second : Glyphs();

        auto glyphPositionsIt = glyphPositions.find(fontStack);
        const GlyphPositionMap& glyphPositionMap = glyphPositionsIt != glyphPositions.end()
            ? glyphPositionsIt->second : GlyphPositionMap();

        std::pair<Shaping, Shaping> shapedTextOrientations;
        optional<PositionedIcon> shapedIcon;

        // if feature has text, shape the text
        if (feature.text) {
            auto applyShaping = [&] (const std::u16string& text, WritingModeType writingMode) {
                const float oneEm = 24.0f;
                const auto offset = layout.textOffset.evaluate(feature, zoom, TextOffset::defaultValue());
                const Shaping result = getShaping(
                    /* string */ text,
                    /* maxWidth: ems */ layout.symbolPlacement == SymbolPlacementType::Point ?
                        layout.textMaxWidth.evaluate(feature, zoom, TextMaxWidth::defaultValue()) * oneEm : 0,
                    /* lineHeight: ems */ layout.textLineHeight * oneEm,
                    /* anchor */ layout.textAnchor.evaluate(feature, zoom, TextAnchor::defaultValue()),
                    /* justify */ layout.textJustify.evaluate(feature, zoom, TextJustify::defaultValue()),
                    /* spacing: ems */ util::i18n::allowsLetterSpacing(*feature.text) ? layout.textLetterSpacing.evaluate(feature, zoom, TextLetterSpacing::defaultValue()) * oneEm : 0.0f,
                    /* translate */ Point<float>(offset[0] * oneEm, offset[1] * oneEm),
                    /* verticalHeight */ oneEm,
                    /* writingMode */ writingMode,
                    /* bidirectional algorithm object */ bidi,
                    /* glyphs */ glyphs);

                return result;
            };

            shapedTextOrientations.first = applyShaping(*feature.text, WritingModeType::Horizontal);

            if (util::i18n::allowsVerticalWritingMode(*feature.text) && textAlongLine) {
                shapedTextOrientations.second = applyShaping(util::i18n::verticalizePunctuation(*feature.text), WritingModeType::Vertical);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.icon) {
            auto image = imageMap.find(*feature.icon);
            if (image != imageMap.end()) {
                shapedIcon = PositionedIcon::shapeIcon(
                    imagePositions.at(*feature.icon),
                    layout.iconOffset.evaluate(feature, zoom, IconOffset::defaultValue()),
                    layout.iconAnchor.evaluate(feature, zoom, IconAnchor::defaultValue()),
                    layout.iconRotate.evaluate(feature, zoom, IconRotate::defaultValue()) * util::DEG2RAD);
                if (image->second->sdf) {
                    sdfIcons = true;
                }
                if (image->second->pixelRatio != pixelRatio) {
                    iconsNeedLinear = true;
                } else if (layout.iconRotate.constantOr(1) != 0) {
                    iconsNeedLinear = true;
                }
            }
        }

        // if either shapedText or icon position is present, add the feature
        if (shapedTextOrientations.first || shapedIcon) {
            addFeature(std::distance(features.begin(), it), feature, shapedTextOrientations, shapedIcon, glyphPositionMap);
        }
        
        feature.geometry.clear();
    }

    compareText.clear();
}

void SymbolLayout::addFeature(const std::size_t layoutFeatureIndex,
                              const SymbolFeature& feature,
                              const std::pair<Shaping, Shaping>& shapedTextOrientations,
                              optional<PositionedIcon> shapedIcon,
                              const GlyphPositionMap& glyphPositionMap) {
    const float minScale = 0.5f;
    const float glyphSize = 24.0f;
    
    const float layoutTextSize = layout.textSize.evaluate(feature, zoom + 1, TextSize::defaultValue());
    const float layoutIconSize = layout.iconSize.evaluate(feature, zoom + 1, IconSize::defaultValue());
    const std::array<float, 2> textOffset = layout.textOffset.evaluate(feature, zoom, TextOffset::defaultValue());
    const std::array<float, 2> iconOffset = layout.iconOffset.evaluate(feature, zoom, IconOffset::defaultValue());
    
    // To reduce the number of labels that jump around when zooming we need
    // to use a text-size value that is the same for all zoom levels.
    // This calculates text-size at a high zoom level so that all tiles can
    // use the same value when calculating anchor positions.
    const float textMaxSize = layout.textSize.evaluate(feature, 18, TextSize::defaultValue());
    
    const float fontScale = layoutTextSize / glyphSize;
    const float textBoxScale = tilePixelRatio * fontScale;
    const float textMaxBoxScale = tilePixelRatio * textMaxSize / glyphSize;
    const float iconBoxScale = tilePixelRatio * layoutIconSize;
    const float symbolSpacing = tilePixelRatio * layout.symbolSpacing;
    // CJL: I'm not sure why SymbolPlacementType::Line -> avoidEdges = false. It seems redundant since
    // getAnchors will already avoid generating anchors outside the tile bounds.
    // However, SymbolPlacementType::LineCenter allows anchors outside tile boundaries, so its behavior
    // here should match SymbolPlacement::Point
    const bool avoidEdges = layout.symbolAvoidEdges && layout.symbolPlacement != SymbolPlacementType::Line;
    const float textPadding = layout.textPadding * tilePixelRatio;
    const float iconPadding = layout.iconPadding * tilePixelRatio;
    const float textMaxAngle = layout.textMaxAngle * util::DEG2RAD;
    const SymbolPlacementType textPlacement = layout.textRotationAlignment != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout.symbolPlacement;

    const float textRepeatDistance = symbolSpacing / 2;
    IndexedSubfeature indexedFeature(feature.index, sourceLayer->getName(), bucketLeaderID, symbolInstances.size());

    auto addSymbolInstance = [&] (const GeometryCoordinates& line, Anchor& anchor) {
        // https://github.com/mapbox/vector-tile-spec/tree/master/2.1#41-layers
        // +-------------------+ Symbols with anchors located on tile edges
        // |(0,0)             || are duplicated on neighbor tiles.
        // |                  ||
        // |                  || In continuous mode, to avoid overdraw we
        // |                  || skip symbols located on the extent edges.
        // |       Tile       || In still mode, we include the features in
        // |                  || the buffers for both tiles and clip them
        // |                  || at draw time.
        // |                  ||
        // +-------------------| In this scenario, the inner bounding box
        // +-------------------+ is called 'withinPlus0', and the outer
        //       (extent,extent) is called 'inside'.
        const bool withinPlus0 = anchor.point.x >= 0 && anchor.point.x < util::EXTENT && anchor.point.y >= 0 && anchor.point.y < util::EXTENT;
        const bool inside = withinPlus0 || anchor.point.x == util::EXTENT || anchor.point.y == util::EXTENT;

        if (avoidEdges && !inside) return;

        if (mode == MapMode::Tile || withinPlus0) {
            symbolInstances.emplace_back(anchor, line, shapedTextOrientations, shapedIcon,
                    layout.evaluate(zoom, feature), layoutTextSize,
                    textBoxScale, textPadding, textPlacement, textOffset,
                    iconBoxScale, iconPadding, iconOffset,
                    glyphPositionMap, indexedFeature, layoutFeatureIndex, feature.index, feature.text.value_or(std::u16string()), overscaling);
        }
    };
    
    const auto& type = feature.getType();

    if (layout.symbolPlacement == SymbolPlacementType::Line) {
        auto clippedLines = util::clipLines(feature.geometry, 0, 0, util::EXTENT, util::EXTENT);
        for (const auto& line : clippedLines) {
            Anchors anchors = getAnchors(line,
                                         symbolSpacing,
                                         textMaxAngle,
                                         (shapedTextOrientations.second ?: shapedTextOrientations.first).left,
                                         (shapedTextOrientations.second ?: shapedTextOrientations.first).right,
                                         (shapedIcon ? shapedIcon->left() : 0),
                                         (shapedIcon ? shapedIcon->right() : 0),
                                         glyphSize,
                                         textMaxBoxScale,
                                         overscaling);

            for (auto& anchor : anchors) {
                if (!feature.text || !anchorIsTooClose(*feature.text, textRepeatDistance, anchor)) {
                    addSymbolInstance(line, anchor);
                }
            }
        }
    } else if (layout.symbolPlacement == SymbolPlacementType::LineCenter) {
        // No clipping, multiple lines per feature are allowed
        // "lines" with only one point are ignored as in clipLines
        for (const auto& line : feature.geometry) {
            if (line.size() > 1) {
                optional<Anchor> anchor = getCenterAnchor(line,
                                                          textMaxAngle,
                                                          (shapedTextOrientations.second ?: shapedTextOrientations.first).left,
                                                          (shapedTextOrientations.second ?: shapedTextOrientations.first).right,
                                                          (shapedIcon ? shapedIcon->left() : 0),
                                                          (shapedIcon ? shapedIcon->right() : 0),
                                                          glyphSize,
                                                          textMaxBoxScale);
                if (anchor) {
                    addSymbolInstance(line, *anchor);
                }
            }
        }
    } else if (type == FeatureType::Polygon) {
        for (const auto& polygon : classifyRings(feature.geometry)) {
            Polygon<double> poly;
            for (const auto& ring : polygon) {
                LinearRing<double> r;
                for (const auto& p : ring) {
                    r.push_back(convertPoint<double>(p));
                }
                poly.push_back(r);
            }

            // 1 pixel worth of precision, in tile coordinates
            auto poi = mapbox::polylabel(poly, double(util::EXTENT / util::tileSize));
            Anchor anchor(poi.x, poi.y, 0, minScale);
            addSymbolInstance(polygon[0], anchor);
        }
    } else if (type == FeatureType::LineString) {
        for (const auto& line : feature.geometry) {
            Anchor anchor(line[0].x, line[0].y, 0, minScale);
            addSymbolInstance(line, anchor);
        }
    } else if (type == FeatureType::Point) {
        for (const auto& points : feature.geometry) {
            for (const auto& point : points) {
                Anchor anchor(point.x, point.y, 0, minScale);
                addSymbolInstance({point}, anchor);
            }
        }
    }
}

bool SymbolLayout::anchorIsTooClose(const std::u16string& text, const float repeatDistance, const Anchor& anchor) {
    if (compareText.find(text) == compareText.end()) {
        compareText.emplace(text, Anchors());
    } else {
        auto otherAnchors = compareText.find(text)->second;
        for (const Anchor& otherAnchor : otherAnchors) {
            if (util::dist<float>(anchor.point, otherAnchor.point) < repeatDistance) {
                return true;
            }
        }
    }
    compareText[text].push_back(anchor);
    return false;
}

// Analog of `addToLineVertexArray` in JS. This version doesn't need to build up a line array like the
// JS version does, but it uses the same logic to calculate tile distances.
std::vector<float> CalculateTileDistances(const GeometryCoordinates& line, const Anchor& anchor) {
    std::vector<float> tileDistances(line.size());
    if (anchor.segment != -1) {
        auto sumForwardLength = util::dist<float>(anchor.point, line[anchor.segment + 1]);
        auto sumBackwardLength = util::dist<float>(anchor.point, line[anchor.segment]);
        for (size_t i = anchor.segment + 1; i < line.size(); i++) {
            tileDistances[i] = sumForwardLength;
            if (i < line.size() - 1) {
                sumForwardLength += util::dist<float>(line[i + 1], line[i]);
            }
        }
        for (auto i = anchor.segment; i >= 0; i--) {
            tileDistances[i] = sumBackwardLength;
            if (i > 0) {
                sumBackwardLength += util::dist<float>(line[i - 1], line[i]);
            }
        }
    }
    return tileDistances;
}

std::unique_ptr<SymbolBucket> SymbolLayout::place(const bool showCollisionBoxes) {
    const bool mayOverlap = layout.textAllowOverlap || layout.iconAllowOverlap ||
        layout.textIgnorePlacement || layout.iconIgnorePlacement;
    
    auto bucket = std::make_unique<SymbolBucket>(layout, layerPaintProperties, textSize, iconSize, zoom, sdfIcons, iconsNeedLinear, mayOverlap, bucketLeaderID, std::move(symbolInstances));

    for (SymbolInstance &symbolInstance : bucket->symbolInstances) {

        const bool hasText = symbolInstance.hasText;
        const bool hasIcon = symbolInstance.hasIcon;

        const auto& feature = features.at(symbolInstance.layoutFeatureIndex);

        // Insert final placement into collision tree and add glyphs/icons to buffers

        if (hasText) {
            const Range<float> sizeData = bucket->textSizeBinder->getVertexSizeData(feature);
            bucket->text.placedSymbols.emplace_back(symbolInstance.anchor.point, symbolInstance.anchor.segment, sizeData.min, sizeData.max,
                    symbolInstance.textOffset, symbolInstance.writingModes, symbolInstance.line, CalculateTileDistances(symbolInstance.line, symbolInstance.anchor));
            symbolInstance.placedTextIndex = bucket->text.placedSymbols.size() - 1;
            PlacedSymbol& horizontalSymbol = bucket->text.placedSymbols.back();

            bool firstHorizontal = true;
            for (const auto& symbol : symbolInstance.horizontalGlyphQuads) {
                size_t index = addSymbol(
                    bucket->text, sizeData, symbol,
                    symbolInstance.anchor, horizontalSymbol);
                if (firstHorizontal) {
                    horizontalSymbol.vertexStartIndex = index;
                    firstHorizontal = false;
                }
            }
            
            if (symbolInstance.writingModes & WritingModeType::Vertical) {
                bucket->text.placedSymbols.emplace_back(symbolInstance.anchor.point, symbolInstance.anchor.segment, sizeData.min, sizeData.max,
                        symbolInstance.textOffset, WritingModeType::Vertical, symbolInstance.line, CalculateTileDistances(symbolInstance.line, symbolInstance.anchor));
                symbolInstance.placedVerticalTextIndex = bucket->text.placedSymbols.size() - 1;
                
                PlacedSymbol& verticalSymbol = bucket->text.placedSymbols.back();
                bool firstVertical = true;
                
                for (const auto& symbol : symbolInstance.verticalGlyphQuads) {
                    size_t index = addSymbol(
                        bucket->text, sizeData, symbol,
                        symbolInstance.anchor, verticalSymbol);
                    
                    if (firstVertical) {
                        verticalSymbol.vertexStartIndex = index;
                        firstVertical = false;
                    }
                }
            }
        }

        if (hasIcon) {
            if (symbolInstance.iconQuad) {
                const Range<float> sizeData = bucket->iconSizeBinder->getVertexSizeData(feature);
                bucket->icon.placedSymbols.emplace_back(symbolInstance.anchor.point, symbolInstance.anchor.segment, sizeData.min, sizeData.max,
                        symbolInstance.iconOffset, WritingModeType::None, symbolInstance.line, std::vector<float>());
                symbolInstance.placedIconIndex = bucket->icon.placedSymbols.size() - 1;
                PlacedSymbol& iconSymbol = bucket->icon.placedSymbols.back();
                iconSymbol.vertexStartIndex = addSymbol(
                                                        bucket->icon, sizeData, *symbolInstance.iconQuad,
                                                        symbolInstance.anchor, iconSymbol);
            }
        }
        
        for (auto& pair : bucket->paintPropertyBinders) {
            pair.second.first.populateVertexVectors(feature, bucket->icon.vertices.vertexSize());
            pair.second.second.populateVertexVectors(feature, bucket->text.vertices.vertexSize());
        }
    }

    if (showCollisionBoxes) {
        addToDebugBuffers(*bucket);
    }

    return bucket;
}

template <typename Buffer>
size_t SymbolLayout::addSymbol(Buffer& buffer,
                             const Range<float> sizeData,
                             const SymbolQuad& symbol,
                             const Anchor& labelAnchor,
                             PlacedSymbol& placedSymbol) {
    constexpr const uint16_t vertexLength = 4;

    const auto &tl = symbol.tl;
    const auto &tr = symbol.tr;
    const auto &bl = symbol.bl;
    const auto &br = symbol.br;
    const auto &tex = symbol.tex;

    if (buffer.segments.empty() || buffer.segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max()) {
        buffer.segments.emplace_back(buffer.vertices.vertexSize(), buffer.triangles.indexSize());
    }

    // We're generating triangle fans, so we always start with the first
    // coordinate in this polygon.
    auto& segment = buffer.segments.back();
    assert(segment.vertexLength <= std::numeric_limits<uint16_t>::max());
    uint16_t index = segment.vertexLength;

    // coordinates (2 triangles)
    buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(labelAnchor.point, tl, symbol.glyphOffset.y, tex.x, tex.y, sizeData));
    buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(labelAnchor.point, tr, symbol.glyphOffset.y, tex.x + tex.w, tex.y, sizeData));
    buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(labelAnchor.point, bl, symbol.glyphOffset.y, tex.x, tex.y + tex.h, sizeData));
    buffer.vertices.emplace_back(SymbolLayoutAttributes::vertex(labelAnchor.point, br, symbol.glyphOffset.y, tex.x + tex.w, tex.y + tex.h, sizeData));
    
    // Dynamic/Opacity vertices are initialized so that the vertex count always agrees with
    // the layout vertex buffer, but they will always be updated before rendering happens
    auto dynamicVertex = SymbolDynamicLayoutAttributes::vertex(labelAnchor.point, 0);
    buffer.dynamicVertices.emplace_back(dynamicVertex);
    buffer.dynamicVertices.emplace_back(dynamicVertex);
    buffer.dynamicVertices.emplace_back(dynamicVertex);
    buffer.dynamicVertices.emplace_back(dynamicVertex);
    
    auto opacityVertex = SymbolOpacityAttributes::vertex(1.0, 1.0);
    buffer.opacityVertices.emplace_back(opacityVertex);
    buffer.opacityVertices.emplace_back(opacityVertex);
    buffer.opacityVertices.emplace_back(opacityVertex);
    buffer.opacityVertices.emplace_back(opacityVertex);

    // add the two triangles, referencing the four coordinates we just inserted.
    buffer.triangles.emplace_back(index + 0, index + 1, index + 2);
    buffer.triangles.emplace_back(index + 1, index + 2, index + 3);

    segment.vertexLength += vertexLength;
    segment.indexLength += 6;

    placedSymbol.glyphOffsets.push_back(symbol.glyphOffset.x);
    
    return index;
}

void SymbolLayout::addToDebugBuffers(SymbolBucket& bucket) {

    if (!hasSymbolInstances()) {
        return;
    }

    for (const SymbolInstance &symbolInstance : symbolInstances) {
        auto populateCollisionBox = [&](const auto& feature) {
            SymbolBucket::CollisionBuffer& collisionBuffer = feature.alongLine ?
                static_cast<SymbolBucket::CollisionBuffer&>(bucket.collisionCircle) :
                static_cast<SymbolBucket::CollisionBuffer&>(bucket.collisionBox);
            for (const CollisionBox &box : feature.boxes) {
                auto& anchor = box.anchor;

                Point<float> tl{box.x1, box.y1};
                Point<float> tr{box.x2, box.y1};
                Point<float> bl{box.x1, box.y2};
                Point<float> br{box.x2, box.y2};

                static constexpr std::size_t vertexLength = 4;
                const std::size_t indexLength = feature.alongLine ? 6 : 8;

                if (collisionBuffer.segments.empty() || collisionBuffer.segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max()) {
                    collisionBuffer.segments.emplace_back(collisionBuffer.vertices.vertexSize(),
                      feature.alongLine? bucket.collisionCircle.triangles.indexSize() : bucket.collisionBox.lines.indexSize());
                }

                auto& segment = collisionBuffer.segments.back();
                uint16_t index = segment.vertexLength;

                collisionBuffer.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, symbolInstance.anchor.point, tl));
                collisionBuffer.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, symbolInstance.anchor.point, tr));
                collisionBuffer.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, symbolInstance.anchor.point, br));
                collisionBuffer.vertices.emplace_back(CollisionBoxProgram::vertex(anchor, symbolInstance.anchor.point, bl));

                // Dynamic vertices are initialized so that the vertex count always agrees with
                // the layout vertex buffer, but they will always be updated before rendering happens
                auto dynamicVertex = CollisionBoxDynamicAttributes::vertex(false, false);
                collisionBuffer.dynamicVertices.emplace_back(dynamicVertex);
                collisionBuffer.dynamicVertices.emplace_back(dynamicVertex);
                collisionBuffer.dynamicVertices.emplace_back(dynamicVertex);
                collisionBuffer.dynamicVertices.emplace_back(dynamicVertex);

                if (feature.alongLine) {
                    bucket.collisionCircle.triangles.emplace_back(index, index + 1, index + 2);
                    bucket.collisionCircle.triangles.emplace_back(index, index + 2, index + 3);
                } else {
                    bucket.collisionBox.lines.emplace_back(index + 0, index + 1);
                    bucket.collisionBox.lines.emplace_back(index + 1, index + 2);
                    bucket.collisionBox.lines.emplace_back(index + 2, index + 3);
                    bucket.collisionBox.lines.emplace_back(index + 3, index + 0);
                }

                segment.vertexLength += vertexLength;
                segment.indexLength += indexLength;
            }
        };
        populateCollisionBox(symbolInstance.textCollisionFeature);
        populateCollisionBox(symbolInstance.iconCollisionFeature);
    }
}

} // namespace mbgl
