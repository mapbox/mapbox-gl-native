#include <mbgl/layout/layout.hpp>
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

template <class Property>
static bool has(const style::SymbolLayoutProperties::PossiblyEvaluated& layout) {
    return layout.get<Property>().match(
        [&] (const typename Property::Type& t) { return !t.empty(); },
        [&] (const auto&) { return true; }
    );
}

SymbolLayout::SymbolLayout(const BucketParameters& parameters,
                           const std::vector<const RenderLayer*>& layers,
                           std::unique_ptr<GeometryTileLayer> sourceLayer_,
                           ImageDependencies& imageDependencies,
                           GlyphDependencies& glyphDependencies)
    : Layout(),
      bucketLeaderID(layers.at(0)->getID()),
      sourceLayer(std::move(sourceLayer_)),
      overscaling(parameters.tileID.overscaleFactor()),
      zoom(parameters.tileID.overscaledZ),
      mode(parameters.mode),
      pixelRatio(parameters.pixelRatio),
      tileSize(util::tileSize * overscaling),
      tilePixelRatio(float(util::EXTENT) / tileSize),
      textSize(toRenderSymbolLayer(layers.at(0))->impl().layout.get<TextSize>()),
      iconSize(toRenderSymbolLayer(layers.at(0))->impl().layout.get<IconSize>())
    {

    const SymbolLayer::Impl& leader = toRenderSymbolLayer(layers.at(0))->impl();

    layout = leader.layout.evaluate(PropertyEvaluationParameters(zoom));

    if (layout.get<IconRotationAlignment>() == AlignmentType::Auto) {
        if (layout.get<SymbolPlacement>() != SymbolPlacementType::Point) {
            layout.get<IconRotationAlignment>() = AlignmentType::Map;
        } else {
            layout.get<IconRotationAlignment>() = AlignmentType::Viewport;
        }
    }

    if (layout.get<TextRotationAlignment>() == AlignmentType::Auto) {
        if (layout.get<SymbolPlacement>() != SymbolPlacementType::Point) {
            layout.get<TextRotationAlignment>() = AlignmentType::Map;
        } else {
            layout.get<TextRotationAlignment>() = AlignmentType::Viewport;
        }
    }

    // If unspecified `*-pitch-alignment` inherits `*-rotation-alignment`
    if (layout.get<TextPitchAlignment>() == AlignmentType::Auto) {
        layout.get<TextPitchAlignment>() = layout.get<TextRotationAlignment>();
    }
    if (layout.get<IconPitchAlignment>() == AlignmentType::Auto) {
        layout.get<IconPitchAlignment>() = layout.get<IconRotationAlignment>();
    }

    const bool hasText = has<TextField>(layout) && has<TextFont>(layout);
    const bool hasIcon = has<IconImage>(layout);

    if (!hasText && !hasIcon) {
        return;
    }

    for (const auto& layer : layers) {
        layerPaintProperties.emplace(layer->getID(), std::make_pair(
            toRenderSymbolLayer(layer)->iconPaintProperties(),
            toRenderSymbolLayer(layer)->textPaintProperties()
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
            auto formatted = layout.evaluate<TextField>(zoom, ft);
            auto textTransform = layout.evaluate<TextTransform>(zoom, ft);
            FontStack baseFontStack = layout.evaluate<TextFont>(zoom, ft);
            FontStackHash baseFontStackHash = FontStackHasher()(baseFontStack);

            ft.formattedText = TaggedString();
            for (std::size_t j = 0; j < formatted.sections.size(); j++) {
                const auto& section = formatted.sections[j];
                std::string u8string = section.text;
                if (textTransform == TextTransformType::Uppercase) {
                    u8string = platform::uppercase(u8string);
                } else if (textTransform == TextTransformType::Lowercase) {
                    u8string = platform::lowercase(u8string);
                }

                ft.formattedText->addSection(applyArabicShaping(util::convertUTF8ToUTF16(u8string)),
                                             section.fontScale ? *section.fontScale : 1.0,
                                             section.fontStack ? FontStackHasher()(*section.fontStack) : baseFontStackHash);

            }


            const bool canVerticalizeText = layout.get<TextRotationAlignment>() == AlignmentType::Map
                                         && layout.get<SymbolPlacement>() != SymbolPlacementType::Point
                                         && util::i18n::allowsVerticalWritingMode(ft.formattedText->rawText());

            // Loop through all characters of this text and collect unique codepoints.
            for (std::size_t j = 0; j < ft.formattedText->length(); j++) {
                const auto& sectionFontStack = formatted.sections[ft.formattedText->getSectionIndex(j)].fontStack;
                GlyphIDs& dependencies = glyphDependencies[sectionFontStack ? *sectionFontStack : baseFontStack];
                char16_t codePoint = ft.formattedText->getCharCodeAt(j);
                dependencies.insert(codePoint);
                if (canVerticalizeText) {
                    if (char16_t verticalChr = util::i18n::verticalizePunctuation(codePoint)) {
                        dependencies.insert(verticalChr);
                    }
                }
            }
        }

        if (hasIcon) {
            ft.icon = layout.evaluate<IconImage>(zoom, ft);
            imageDependencies.emplace(*ft.icon, ImageType::Icon);
        }

        if (ft.formattedText || ft.icon) {
            features.push_back(std::move(ft));
        }
    }

    if (layout.get<SymbolPlacement>() == SymbolPlacementType::Line) {
        util::mergeLines(features);
    }
}

bool SymbolLayout::hasDependencies() const {
    return features.size() != 0;
}

bool SymbolLayout::hasSymbolInstances() const {
    return !symbolInstances.empty();
}

void SymbolLayout::prepareSymbols(const GlyphMap& glyphMap, const GlyphPositions& glyphPositions,
                           const ImageMap& imageMap, const ImagePositions& imagePositions) {
    const bool textAlongLine = layout.get<TextRotationAlignment>() == AlignmentType::Map &&
        layout.get<SymbolPlacement>() != SymbolPlacementType::Point;

    for (auto it = features.begin(); it != features.end(); ++it) {
        auto& feature = *it;
        if (feature.geometry.empty()) continue;

        std::pair<Shaping, Shaping> shapedTextOrientations;
        optional<PositionedIcon> shapedIcon;

        // if feature has text, shape the text
        if (feature.formattedText) {
            auto applyShaping = [&] (const TaggedString& formattedText, WritingModeType writingMode) {
                const float oneEm = 24.0f;
                const Shaping result = getShaping(
                    /* string */ formattedText,
                    /* maxWidth: ems */ layout.get<SymbolPlacement>() == SymbolPlacementType::Point ?
                        layout.evaluate<TextMaxWidth>(zoom, feature) * oneEm : 0,
                    /* lineHeight: ems */ layout.get<TextLineHeight>() * oneEm,
                    /* anchor */ layout.evaluate<TextAnchor>(zoom, feature),
                    /* justify */ layout.evaluate<TextJustify>(zoom, feature),
                    /* spacing: ems */ util::i18n::allowsLetterSpacing(feature.formattedText->rawText()) ? layout.evaluate<TextLetterSpacing>(zoom, feature) * oneEm : 0.0f,
                    /* translate */ Point<float>(layout.evaluate<TextOffset>(zoom, feature)[0] * oneEm, layout.evaluate<TextOffset>(zoom, feature)[1] * oneEm),
                    /* verticalHeight */ oneEm,
                    /* writingMode */ writingMode,
                    /* bidirectional algorithm object */ bidi,
                    /* glyphs */ glyphMap);

                return result;
            };

            shapedTextOrientations.first = applyShaping(*feature.formattedText, WritingModeType::Horizontal);

            if (util::i18n::allowsVerticalWritingMode(feature.formattedText->rawText()) && textAlongLine) {
                feature.formattedText->verticalizePunctuation();
                shapedTextOrientations.second = applyShaping(*feature.formattedText, WritingModeType::Vertical);
            }
        }

        // if feature has icon, get sprite atlas position
        if (feature.icon) {
            auto image = imageMap.find(*feature.icon);
            if (image != imageMap.end()) {
                shapedIcon = PositionedIcon::shapeIcon(
                    imagePositions.at(*feature.icon),
                    layout.evaluate<IconOffset>(zoom, feature),
                    layout.evaluate<IconAnchor>(zoom, feature),
                    layout.evaluate<IconRotate>(zoom, feature) * util::DEG2RAD);
                if (image->second->sdf) {
                    sdfIcons = true;
                }
                if (image->second->pixelRatio != pixelRatio) {
                    iconsNeedLinear = true;
                } else if (layout.get<IconRotate>().constantOr(1) != 0) {
                    iconsNeedLinear = true;
                }
            }
        }

        // if either shapedText or icon position is present, add the feature
        if (shapedTextOrientations.first || shapedIcon) {
            addFeature(std::distance(features.begin(), it), feature, shapedTextOrientations, shapedIcon, glyphPositions);
        }

        feature.geometry.clear();
    }

    compareText.clear();
}

void SymbolLayout::addFeature(const std::size_t layoutFeatureIndex,
                              const SymbolFeature& feature,
                              const std::pair<Shaping, Shaping>& shapedTextOrientations,
                              optional<PositionedIcon> shapedIcon,
                              const GlyphPositions& glyphPositions) {
    const float minScale = 0.5f;
    const float glyphSize = 24.0f;

    const float layoutTextSize = layout.evaluate<TextSize>(zoom + 1, feature);
    const float layoutIconSize = layout.evaluate<IconSize>(zoom + 1, feature);
    const std::array<float, 2> textOffset = layout.evaluate<TextOffset>(zoom, feature);
    const std::array<float, 2> iconOffset = layout.evaluate<IconOffset>(zoom, feature);

    // To reduce the number of labels that jump around when zooming we need
    // to use a text-size value that is the same for all zoom levels.
    // This calculates text-size at a high zoom level so that all tiles can
    // use the same value when calculating anchor positions.
    const float textMaxSize = layout.evaluate<TextSize>(18, feature);

    const float fontScale = layoutTextSize / glyphSize;
    const float textBoxScale = tilePixelRatio * fontScale;
    const float textMaxBoxScale = tilePixelRatio * textMaxSize / glyphSize;
    const float iconBoxScale = tilePixelRatio * layoutIconSize;
    const float symbolSpacing = tilePixelRatio * layout.get<SymbolSpacing>();
    const float textPadding = layout.get<TextPadding>() * tilePixelRatio;
    const float iconPadding = layout.get<IconPadding>() * tilePixelRatio;
    const float textMaxAngle = layout.get<TextMaxAngle>() * util::DEG2RAD;
    const float rotation = layout.evaluate<IconRotate>(zoom, feature);
    const SymbolPlacementType textPlacement = layout.get<TextRotationAlignment>() != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout.get<SymbolPlacement>();

    const float textRepeatDistance = symbolSpacing / 2;
    IndexedSubfeature indexedFeature(feature.index, sourceLayer->getName(), bucketLeaderID, symbolInstances.size());

    auto addSymbolInstance = [&] (const GeometryCoordinates& line, Anchor& anchor) {
        const bool anchorInsideTile = anchor.point.x >= 0 && anchor.point.x < util::EXTENT && anchor.point.y >= 0 && anchor.point.y < util::EXTENT;

        if (mode == MapMode::Tile || anchorInsideTile) {
            // For static/continuous rendering, only add symbols anchored within this tile:
            //  neighboring symbols will be added as part of the neighboring tiles.
            // In tiled rendering mode, add all symbols in the buffers so that we can:
            //  (1) render symbols that overlap into this tile
            //  (2) approximate collision detection effects from neighboring symbols
            symbolInstances.emplace_back(anchor, line, shapedTextOrientations, shapedIcon,
                    layout.evaluate(zoom, feature), layoutTextSize,
                    textBoxScale, textPadding, textPlacement, textOffset,
                    iconBoxScale, iconPadding, iconOffset,
                    glyphPositions, indexedFeature, layoutFeatureIndex, feature.index,
                    feature.formattedText ? feature.formattedText->rawText() : std::u16string(), overscaling, rotation);
        }
    };

    const auto& type = feature.getType();

    if (layout.get<SymbolPlacement>() == SymbolPlacementType::Line) {
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
                if (!feature.formattedText || !anchorIsTooClose(feature.formattedText->rawText(), textRepeatDistance, anchor)) {
                    addSymbolInstance(line, anchor);
                }
            }
        }
    } else if (layout.get<SymbolPlacement>() == SymbolPlacementType::LineCenter) {
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

void SymbolLayout::createBucket(const ImagePositions&, std::unique_ptr<FeatureIndex>&, std::unordered_map<std::string, std::shared_ptr<Bucket>>& buckets, const bool firstLoad, const bool showCollisionBoxes) {
    const bool zOrderByViewport = layout.get<SymbolZOrder>() == SymbolZOrderType::ViewportY;
    const bool sortFeaturesByY = zOrderByViewport && (layout.get<TextAllowOverlap>() || layout.get<IconAllowOverlap>() ||
        layout.get<TextIgnorePlacement>() || layout.get<IconIgnorePlacement>());

    auto bucket = std::make_shared<SymbolBucket>(layout, layerPaintProperties, textSize, iconSize, zoom, sdfIcons, iconsNeedLinear, sortFeaturesByY, bucketLeaderID, std::move(symbolInstances));

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
                iconSymbol.vertexStartIndex = addSymbol(bucket->icon, sizeData, *symbolInstance.iconQuad,
                                                        symbolInstance.anchor, iconSymbol);
            }
        }

        for (auto& pair : bucket->paintPropertyBinders) {
            pair.second.first.populateVertexVectors(feature, bucket->icon.vertices.vertexSize(), {}, {});
            pair.second.second.populateVertexVectors(feature, bucket->text.vertices.vertexSize(), {}, {});
        }
    }

    if (showCollisionBoxes) {
        addToDebugBuffers(*bucket);
    }
    if (bucket->hasData()){
        for (const auto& pair : layerPaintProperties) {
            if (!firstLoad) {
                bucket->justReloaded = true;
            }
            buckets.emplace(pair.first, bucket);
        }
    }

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
