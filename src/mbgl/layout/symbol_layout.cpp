#include <mbgl/layout/symbol_layout.hpp>
#include <mbgl/layout/merge_lines.hpp>
#include <mbgl/layout/clip_lines.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/layers/render_symbol_layer.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/text/get_anchors.hpp>
#include <mbgl/text/shaping.hpp>
#include <mbgl/util/utf.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/i18n.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile.hpp>

#include <mapbox/polylabel.hpp>

namespace mbgl {

using namespace style;

template <class Property>
static bool has(const style::SymbolLayoutProperties::PossiblyEvaluated& layout) {
    return layout.get<Property>().match(
        [] (const typename Property::Type& t) { return !t.empty(); },
        [] (const auto&) { return true; }
    );
}

namespace {
expression::Value sectionOptionsToValue(const SectionOptions& options) {
    std::unordered_map<std::string, expression::Value> result;
    // TODO: Data driven properties that can be overridden on per section basis.
    // TextOpacity
    // TextHaloColor
    // TextHaloWidth
    // TextHaloBlur
    if (options.textColor) {
        result.emplace(expression::kFormattedSectionTextColor, *options.textColor);
    }
    return result;
}

inline const SymbolLayerProperties& toSymbolLayerProperties(const Immutable<LayerProperties>& layer) {
    return static_cast<const SymbolLayerProperties&>(*layer);
}

inline Immutable<style::SymbolLayoutProperties::PossiblyEvaluated> createLayout(const SymbolLayoutProperties::Unevaluated& unevaluated, float zoom) {
    auto layout = makeMutable<style::SymbolLayoutProperties::PossiblyEvaluated>(unevaluated.evaluate(PropertyEvaluationParameters(zoom)));

    if (layout->get<IconRotationAlignment>() == AlignmentType::Auto) {
        if (layout->get<SymbolPlacement>() != SymbolPlacementType::Point) {
            layout->get<IconRotationAlignment>() = AlignmentType::Map;
        } else {
            layout->get<IconRotationAlignment>() = AlignmentType::Viewport;
        }
    }

    if (layout->get<TextRotationAlignment>() == AlignmentType::Auto) {
        if (layout->get<SymbolPlacement>() != SymbolPlacementType::Point) {
            layout->get<TextRotationAlignment>() = AlignmentType::Map;
        } else {
            layout->get<TextRotationAlignment>() = AlignmentType::Viewport;
        }
    }

    // If unspecified `*-pitch-alignment` inherits `*-rotation-alignment`
    if (layout->get<TextPitchAlignment>() == AlignmentType::Auto) {
        layout->get<TextPitchAlignment>() = layout->get<TextRotationAlignment>();
    }
    if (layout->get<IconPitchAlignment>() == AlignmentType::Auto) {
        layout->get<IconPitchAlignment>() = layout->get<IconRotationAlignment>();
    }

    return std::move(layout);
}

} // namespace

SymbolLayout::SymbolLayout(const BucketParameters& parameters,
                           const std::vector<Immutable<style::LayerProperties>>& layers,
                           std::unique_ptr<GeometryTileLayer> sourceLayer_,
                           const LayoutParameters& layoutParameters)
    : bucketLeaderID(layers.front()->baseImpl->id),
      sourceLayer(std::move(sourceLayer_)),
      overscaling(parameters.tileID.overscaleFactor()),
      zoom(parameters.tileID.overscaledZ),
      mode(parameters.mode),
      pixelRatio(parameters.pixelRatio),
      tileSize(util::tileSize * overscaling),
      tilePixelRatio(float(util::EXTENT) / tileSize),
      layout(createLayout(toSymbolLayerProperties(layers.at(0)).layerImpl().layout, zoom)) {
    const SymbolLayer::Impl& leader = toSymbolLayerProperties(layers.at(0)).layerImpl();

    textSize = leader.layout.get<TextSize>();
    iconSize = leader.layout.get<IconSize>();
    textRadialOffset = leader.layout.get<TextRadialOffset>();

    const bool hasText = has<TextField>(*layout) && has<TextFont>(*layout);
    const bool hasIcon = has<IconImage>(*layout);

    if (!hasText && !hasIcon) {
        return;
    }

    const bool hasSymbolSortKey = !leader.layout.get<SymbolSortKey>().isUndefined();
    const auto symbolZOrder = layout->get<SymbolZOrder>();
    const bool sortFeaturesByKey = symbolZOrder != SymbolZOrderType::ViewportY && hasSymbolSortKey;
    const bool zOrderByViewportY = symbolZOrder == SymbolZOrderType::ViewportY || (symbolZOrder == SymbolZOrderType::Auto && !sortFeaturesByKey);
    sortFeaturesByY = zOrderByViewportY && (layout->get<TextAllowOverlap>() || layout->get<IconAllowOverlap>() ||
        layout->get<TextIgnorePlacement>() || layout->get<IconIgnorePlacement>());
    if (layout->get<SymbolPlacement>() == SymbolPlacementType::Point) {
        auto modes = layout->get<TextWritingMode>();
        // Remove duplicates and preserve order.
        std::set<style::TextWritingModeType> seen;
        auto end = std::remove_if(modes.begin(),
                                  modes.end(),
                                  [&seen, this](const auto& placementMode) {
                                    allowVerticalPlacement = allowVerticalPlacement || placementMode == style::TextWritingModeType::Vertical;
                                    return !seen.insert(placementMode).second;
                                  });
        modes.erase(end, modes.end());
        placementModes = std::move(modes);
    }

    for (const auto& layer : layers) {
        layerPaintProperties.emplace(layer->baseImpl->id, layer);
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
            auto formatted = layout->evaluate<TextField>(zoom, ft, layoutParameters.availableImages);
            auto textTransform = layout->evaluate<TextTransform>(zoom, ft);
            FontStack baseFontStack = layout->evaluate<TextFont>(zoom, ft);

            ft.formattedText = TaggedString();
            for (const auto & section : formatted.sections) {
                if (!section.image) {
                    std::string u8string = section.text;
                    if (textTransform == TextTransformType::Uppercase) {
                        u8string = platform::uppercase(u8string);
                    } else if (textTransform == TextTransformType::Lowercase) {
                        u8string = platform::lowercase(u8string);
                    }

                    ft.formattedText->addTextSection(applyArabicShaping(util::convertUTF8ToUTF16(u8string)),
                                                     section.fontScale ? *section.fontScale : 1.0,
                                                     section.fontStack ? *section.fontStack : baseFontStack,
                                                     section.textColor);
                } else {
                    layoutParameters.imageDependencies.emplace(section.image->id(), ImageType::Icon);
                    ft.formattedText->addImageSection(section.image->id());
                }
            }

            const bool canVerticalizeText = layout->get<TextRotationAlignment>() == AlignmentType::Map
                                         && layout->get<SymbolPlacement>() != SymbolPlacementType::Point
                                         && ft.formattedText->allowsVerticalWritingMode();

            // Loop through all characters of this text and collect unique codepoints.
            for (std::size_t j = 0; j < ft.formattedText->length(); j++) {
                const auto& section = formatted.sections[ft.formattedText->getSectionIndex(j)];
                if (section.image) continue;

                const auto& sectionFontStack = section.fontStack;
                GlyphIDs& dependencies =
                    layoutParameters.glyphDependencies[sectionFontStack ? *sectionFontStack : baseFontStack];
                char16_t codePoint = ft.formattedText->getCharCodeAt(j);
                dependencies.insert(codePoint);
                if (canVerticalizeText || (allowVerticalPlacement && ft.formattedText->allowsVerticalWritingMode())) {
                    if (char16_t verticalChr = util::i18n::verticalizePunctuation(codePoint)) {
                        dependencies.insert(verticalChr);
                    }
                }
            }
        }

        if (hasIcon) {
            ft.icon = layout->evaluate<IconImage>(zoom, ft, layoutParameters.availableImages);
            layoutParameters.imageDependencies.emplace(ft.icon->id(), ImageType::Icon);
        }

        if (ft.formattedText || ft.icon) {
            if (sortFeaturesByKey) {
                ft.sortKey = layout->evaluate<SymbolSortKey>(zoom, ft);
                const auto lowerBound = std::lower_bound(features.begin(), features.end(), ft);
                features.insert(lowerBound, std::move(ft));
            } else {
                features.push_back(std::move(ft));
            }
        }
    }

    if (layout->get<SymbolPlacement>() == SymbolPlacementType::Line) {
        util::mergeLines(features);
    }
}

bool SymbolLayout::hasDependencies() const {
    return features.size() != 0;
}

bool SymbolLayout::hasSymbolInstances() const {
    return !symbolInstances.empty();
}

namespace {

// The radial offset is to the edge of the text box
// In the horizontal direction, the edge of the text box is where glyphs start
// But in the vertical direction, the glyphs appear to "start" at the baseline
// We don't actually load baseline data, but we assume an offset of ONE_EM - 17
// (see "yOffset" in shaping.js)
const float baselineOffset = 7.0f;

// We don't care which shaping we get because this is used for collision purposes
// and all the justifications have the same collision box.
const Shaping& getDefaultHorizontalShaping(const ShapedTextOrientations& shapedTextOrientations) {
    if (shapedTextOrientations.right) return shapedTextOrientations.right;
    if (shapedTextOrientations.center) return shapedTextOrientations.center;
    if (shapedTextOrientations.left) return shapedTextOrientations.left;
    return shapedTextOrientations.horizontal;
}

Shaping& shapingForTextJustifyType(ShapedTextOrientations& shapedTextOrientations, style::TextJustifyType type) {
    switch(type) {
    case style::TextJustifyType::Right: return shapedTextOrientations.right;
    case style::TextJustifyType::Left: return shapedTextOrientations.left;
    case style::TextJustifyType::Center: return shapedTextOrientations.center;
    default:
        assert(false);
        return shapedTextOrientations.horizontal;
    }
}

std::array<float, 2> evaluateRadialOffset(style::SymbolAnchorType anchor, float radialOffset) { 
    std::array<float, 2> result{{0.0f, 0.0f}};
    if (radialOffset < 0.0f) radialOffset = 0.0f; // Ignore negative offset.
    // solve for r where r^2 + r^2 = radialOffset^2
    const float sqrt2 = 1.41421356237f;
    const float hypotenuse = radialOffset / sqrt2;

    switch (anchor) {
    case SymbolAnchorType::TopRight:
    case SymbolAnchorType::TopLeft:
        result[1] = hypotenuse - baselineOffset;
        break;
    case SymbolAnchorType::BottomRight:
    case SymbolAnchorType::BottomLeft:
        result[1] = -hypotenuse + baselineOffset;
        break;
    case SymbolAnchorType::Bottom:
        result[1] = -radialOffset + baselineOffset;
        break;
    case SymbolAnchorType::Top:
        result[1] = radialOffset - baselineOffset;
        break;
    default:
        break;
    }

    switch (anchor) {
    case SymbolAnchorType::TopRight:
    case SymbolAnchorType::BottomRight:
        result[0] = -hypotenuse;
        break;
    case SymbolAnchorType::TopLeft:
    case SymbolAnchorType::BottomLeft:
        result[0] = hypotenuse;
        break;
    case SymbolAnchorType::Left:
        result[0] = radialOffset;
        break;
    case SymbolAnchorType::Right:
        result[0] = -radialOffset;
        break;
    default:
        break;
    }

    return result;
}

} // namespace

// static
std::array<float, 2> SymbolLayout::evaluateVariableOffset(style::SymbolAnchorType anchor, std::array<float, 2> offset) {
    if (offset[1] == INVALID_OFFSET_VALUE) {
        return evaluateRadialOffset(anchor, offset[0]);
    }
    std::array<float, 2> result{{0.0f, 0.0f}};
    offset[0] = std::abs(offset[0]);
    offset[1] = std::abs(offset[1]);

    switch (anchor) {
    case SymbolAnchorType::TopRight:
    case SymbolAnchorType::TopLeft:
    case SymbolAnchorType::Top:
        result[1] = offset[1] - baselineOffset;
        break;
    case SymbolAnchorType::BottomRight:
    case SymbolAnchorType::BottomLeft:
    case SymbolAnchorType::Bottom:
        result[1] = -offset[1] + baselineOffset;
        break;
    case SymbolAnchorType::Center:
    case SymbolAnchorType::Left:
    case SymbolAnchorType::Right:
        break;
    }

    switch (anchor) {
    case SymbolAnchorType::TopRight:
    case SymbolAnchorType::BottomRight:
    case SymbolAnchorType::Right:
        result[0] = -offset[0];
        break;
    case SymbolAnchorType::TopLeft:
    case SymbolAnchorType::BottomLeft:
    case SymbolAnchorType::Left:
        result[0] = offset[0];
        break;
    case SymbolAnchorType::Center:
    case SymbolAnchorType::Top:
    case SymbolAnchorType::Bottom:
        break;
    }

    return result;
}

void SymbolLayout::prepareSymbols(const GlyphMap& glyphMap,
                                  const GlyphPositions& glyphPositions,
                                  const ImageMap& imageMap,
                                  const ImagePositions& imagePositions) {
    const bool isPointPlacement = layout->get<SymbolPlacement>() == SymbolPlacementType::Point;
    const bool textAlongLine = layout->get<TextRotationAlignment>() == AlignmentType::Map && !isPointPlacement;

    for (auto it = features.begin(); it != features.end(); ++it) {
        auto& feature = *it;
        if (feature.geometry.empty()) continue;

        ShapedTextOrientations shapedTextOrientations;
        optional<PositionedIcon> shapedIcon;
        std::array<float, 2> textOffset{{0.0f, 0.0f}};
        const float layoutTextSize = layout->evaluate<TextSize>(zoom + 1, feature);
        const float layoutTextSizeAtBucketZoomLevel = layout->evaluate<TextSize>(zoom, feature);
        const float layoutIconSize = layout->evaluate<IconSize>(zoom + 1, feature);

        // if feature has text, shape the text
        if (feature.formattedText && layoutTextSize > 0.0f) {
            const float lineHeight = layout->get<TextLineHeight>() * util::ONE_EM;
            const float spacing = util::i18n::allowsLetterSpacing(feature.formattedText->rawText()) ? layout->evaluate<TextLetterSpacing>(zoom, feature) * util::ONE_EM : 0.0f;

            auto applyShaping = [&](const TaggedString& formattedText,
                                    WritingModeType writingMode,
                                    SymbolAnchorType textAnchor,
                                    TextJustifyType textJustify) {
                const Shaping result = getShaping(
                    /* string */ formattedText,
                    /* maxWidth: ems */
                    isPointPlacement ? layout->evaluate<TextMaxWidth>(zoom, feature) * util::ONE_EM : 0.0f,
                    /* ems */ lineHeight,
                    textAnchor,
                    textJustify,
                    /* ems */ spacing,
                    /* translate */ textOffset,
                    /* writingMode */ writingMode,
                    /* bidirectional algorithm object */ bidi,
                    glyphMap,
                    /* glyphs */ glyphPositions,
                    /* images */ imagePositions,
                    layoutTextSize,
                    layoutTextSizeAtBucketZoomLevel,
                    allowVerticalPlacement);

                return result;
            };

            const std::vector<style::TextVariableAnchorType> variableTextAnchor = layout->evaluate<TextVariableAnchor>(zoom, feature);
            const SymbolAnchorType textAnchor = layout->evaluate<TextAnchor>(zoom, feature); 
            if (variableTextAnchor.empty()) {
                // Layers with variable anchors use the `text-radial-offset` property and the [x, y] offset vector
                // is calculated at placement time instead of layout time
                const float radialOffset = layout->evaluate<TextRadialOffset>(zoom, feature);
                if (radialOffset > 0.0f) {
                    // The style spec says don't use `text-offset` and `text-radial-offset` together
                    // but doesn't actually specify what happens if you use both. We go with the radial offset.
                    textOffset = evaluateRadialOffset(textAnchor, radialOffset * util::ONE_EM);
                } else {
                    textOffset = {{layout->evaluate<TextOffset>(zoom, feature)[0] * util::ONE_EM,
                                   layout->evaluate<TextOffset>(zoom, feature)[1] * util::ONE_EM}};
                }
            }
            TextJustifyType textJustify = textAlongLine ? TextJustifyType::Center : layout->evaluate<TextJustify>(zoom, feature);

            const auto addVerticalShapingForPointLabelIfNeeded = [&] {
                if (allowVerticalPlacement && feature.formattedText->allowsVerticalWritingMode()) {
                    feature.formattedText->verticalizePunctuation();
                    // Vertical POI label placement is meant to be used for scripts that support vertical
                    // writing mode, thus, default style::TextJustifyType::Left justification is used. If Latin
                    // scripts would need to be supported, this should take into account other justifications.
                    shapedTextOrientations.vertical = applyShaping(*feature.formattedText, WritingModeType::Vertical, textAnchor, style::TextJustifyType::Left);
                }
            };

            // If this layer uses text-variable-anchor, generate shapings for all justification possibilities.
            if (!textAlongLine && !variableTextAnchor.empty()) {
                std::vector<TextJustifyType> justifications;
                if (textJustify != TextJustifyType::Auto) {
                    justifications.push_back(textJustify);
                } else {
                    for (auto anchor : variableTextAnchor) {
                        justifications.push_back(getAnchorJustification(anchor));
                    }
                }
                for (TextJustifyType justification: justifications) {
                    Shaping& shapingForJustification = shapingForTextJustifyType(shapedTextOrientations, justification);
                    if (shapingForJustification) {
                        continue;
                    }
                    // If using text-variable-anchor for the layer, we use a center anchor for all shapings and apply
                    // the offsets for the anchor in the placement step.
                    Shaping shaping = applyShaping(*feature.formattedText, WritingModeType::Horizontal, SymbolAnchorType::Center, justification);
                    if (shaping) {
                        shapingForJustification = std::move(shaping);
                        if (shapingForJustification.positionedLines.size() == 1u) {
                            shapedTextOrientations.singleLine = true;
                            break;
                        }
                    }
                }

                // Vertical point label shaping if allowVerticalPlacement is enabled.
                addVerticalShapingForPointLabelIfNeeded();
            } else {
                if (textJustify == TextJustifyType::Auto) {
                    textJustify = getAnchorJustification(textAnchor);
                }

                // Horizontal point or line label.
                Shaping shaping = applyShaping(*feature.formattedText, WritingModeType::Horizontal, textAnchor, textJustify);
                if (shaping) {
                    shapedTextOrientations.horizontal = std::move(shaping);
                }

                // Vertical point label shaping if allowVerticalPlacement is enabled.
                addVerticalShapingForPointLabelIfNeeded();

                // Verticalized line label.
                if (textAlongLine && feature.formattedText->allowsVerticalWritingMode()) {
                    feature.formattedText->verticalizePunctuation();
                    shapedTextOrientations.vertical = applyShaping(*feature.formattedText, WritingModeType::Vertical, textAnchor, textJustify);
                }
            }
        }

        // if feature has icon, get sprite atlas position
        SymbolContent iconType{SymbolContent::None};
        if (feature.icon) {
            auto image = imageMap.find(feature.icon->id());
            if (image != imageMap.end()) {
                iconType = SymbolContent::IconRGBA;
                shapedIcon = PositionedIcon::shapeIcon(imagePositions.at(feature.icon->id()),
                                                       layout->evaluate<IconOffset>(zoom, feature),
                                                       layout->evaluate<IconAnchor>(zoom, feature),
                                                       layout->evaluate<IconRotate>(zoom, feature) * util::DEG2RAD);
                if (image->second->sdf) {
                    iconType = SymbolContent::IconSDF;
                }
                if (image->second->pixelRatio != pixelRatio) {
                    iconsNeedLinear = true;
                } else if (layout->get<IconRotate>().constantOr(1) != 0) {
                    iconsNeedLinear = true;
                }
            }
        }

        // if either shapedText or icon position is present, add the feature
        const Shaping& defaultShaping = getDefaultHorizontalShaping(shapedTextOrientations);
        iconsInText = defaultShaping ? defaultShaping.iconsInText : false;
        if (defaultShaping || shapedIcon) {
            addFeature(std::distance(features.begin(), it),
                       feature,
                       shapedTextOrientations,
                       std::move(shapedIcon),
                       imageMap,
                       textOffset,
                       layoutTextSize,
                       layoutIconSize,
                       iconType);
        }

        feature.geometry.clear();
    }

    compareText.clear();
}

void SymbolLayout::addFeature(const std::size_t layoutFeatureIndex,
                              const SymbolFeature& feature,
                              const ShapedTextOrientations& shapedTextOrientations,
                              optional<PositionedIcon> shapedIcon,
                              const ImageMap& imageMap,
                              std::array<float, 2> textOffset,
                              float layoutTextSize,
                              float layoutIconSize,
                              const SymbolContent iconType) {
    const float minScale = 0.5f;
    const float glyphSize = 24.0f;

    const std::array<float, 2> iconOffset = layout->evaluate<IconOffset>(zoom, feature);

    // To reduce the number of labels that jump around when zooming we need
    // to use a text-size value that is the same for all zoom levels.
    // This calculates text-size at a high zoom level so that all tiles can
    // use the same value when calculating anchor positions.
    const float textMaxSize = layout->evaluate<TextSize>(18, feature);

    const float fontScale = layoutTextSize / glyphSize;
    const float textBoxScale = tilePixelRatio * fontScale;
    const float textMaxBoxScale = tilePixelRatio * textMaxSize / glyphSize;
    const float iconBoxScale = tilePixelRatio * layoutIconSize;
    const float symbolSpacing = tilePixelRatio * layout->get<SymbolSpacing>();
    const float textPadding = layout->get<TextPadding>() * tilePixelRatio;
    const float iconPadding = layout->get<IconPadding>() * tilePixelRatio;
    const float textMaxAngle = layout->get<TextMaxAngle>() * util::DEG2RAD;
    const float iconRotation = layout->evaluate<IconRotate>(zoom, feature);
    const float textRotation = layout->evaluate<TextRotate>(zoom, feature);
    std::array<float, 2> variableTextOffset;
    if (!textRadialOffset.isUndefined()) {
        variableTextOffset = {{layout->evaluate<TextRadialOffset>(zoom, feature) * util::ONE_EM,
                               INVALID_OFFSET_VALUE}};
    } else {
        variableTextOffset = {{layout->evaluate<TextOffset>(zoom, feature)[0] * util::ONE_EM,
                               layout->evaluate<TextOffset>(zoom, feature)[1] * util::ONE_EM}};
    }

    const SymbolPlacementType textPlacement = layout->get<TextRotationAlignment>() != AlignmentType::Map
                                                  ? SymbolPlacementType::Point
                                                  : layout->get<SymbolPlacement>();

    const float textRepeatDistance = symbolSpacing / 2;
    const auto evaluatedLayoutProperties = layout->evaluate(zoom, feature);
    IndexedSubfeature indexedFeature(feature.index, sourceLayer->getName(), bucketLeaderID, symbolInstances.size());

    const auto iconTextFit = evaluatedLayoutProperties.get<style::IconTextFit>();
    // Adjust shaped icon size when icon-text-fit is used.
    optional<PositionedIcon> verticallyShapedIcon;
    if (shapedIcon && iconTextFit != IconTextFitType::None) {
        // Create vertically shaped icon for vertical writing mode if needed.
        if (allowVerticalPlacement && shapedTextOrientations.vertical) {
            verticallyShapedIcon = shapedIcon;
            verticallyShapedIcon->fitIconToText(
                shapedTextOrientations.vertical, iconTextFit, layout->get<IconTextFitPadding>(), iconOffset, fontScale);
        }
        const auto shapedText = getDefaultHorizontalShaping(shapedTextOrientations);
        if (shapedText) {
            shapedIcon->fitIconToText(
                shapedText, iconTextFit, layout->get<IconTextFitPadding>(), iconOffset, fontScale);
        }
    }

    auto addSymbolInstance = [&] (Anchor& anchor, std::shared_ptr<SymbolInstanceSharedData> sharedData) {
        assert(sharedData);
        const bool anchorInsideTile = anchor.point.x >= 0 && anchor.point.x < util::EXTENT && anchor.point.y >= 0 && anchor.point.y < util::EXTENT;

        if (mode == MapMode::Tile || anchorInsideTile) {
            // For static/continuous rendering, only add symbols anchored within this tile:
            //  neighboring symbols will be added as part of the neighboring tiles.
            // In tiled rendering mode, add all symbols in the buffers so that we can:
            //  (1) render symbols that overlap into this tile
            //  (2) approximate collision detection effects from neighboring symbols
            symbolInstances.emplace_back(anchor, std::move(sharedData), shapedTextOrientations,
                    shapedIcon, verticallyShapedIcon,
                    textBoxScale, textPadding, textPlacement, textOffset,
                    iconBoxScale, iconPadding, iconOffset, indexedFeature,
                    layoutFeatureIndex, feature.index,
                    feature.formattedText ? feature.formattedText->rawText() : std::u16string(),
                    overscaling, iconRotation, textRotation, variableTextOffset, allowVerticalPlacement, iconType);
        }
    };

    const auto createSymbolInstanceSharedData = [&](GeometryCoordinates line) {
        return std::make_shared<SymbolInstanceSharedData>(std::move(line),
                                                          shapedTextOrientations,
                                                          shapedIcon,
                                                          verticallyShapedIcon,
                                                          evaluatedLayoutProperties,
                                                          textPlacement,
                                                          textOffset,
                                                          imageMap,
                                                          iconType,
                                                          allowVerticalPlacement);
    };

    const auto& type = feature.getType();

    if (layout->get<SymbolPlacement>() == SymbolPlacementType::Line) {
        auto clippedLines = util::clipLines(feature.geometry, 0, 0, util::EXTENT, util::EXTENT);
        for (auto& line : clippedLines) {
            Anchors anchors = getAnchors(line,
                                         symbolSpacing,
                                         textMaxAngle,
                                         (shapedTextOrientations.vertical ?: getDefaultHorizontalShaping(shapedTextOrientations)).left,
                                         (shapedTextOrientations.vertical ?: getDefaultHorizontalShaping(shapedTextOrientations)).right,
                                         (shapedIcon ? shapedIcon->left() : 0),
                                         (shapedIcon ? shapedIcon->right() : 0),
                                         glyphSize,
                                         textMaxBoxScale,
                                         overscaling);
            auto sharedData = createSymbolInstanceSharedData(std::move(line));
            for (auto& anchor : anchors) {
                if (!feature.formattedText || !anchorIsTooClose(feature.formattedText->rawText(), textRepeatDistance, anchor)) {
                    addSymbolInstance(anchor, sharedData);
                }
            }
        }
    } else if (layout->get<SymbolPlacement>() == SymbolPlacementType::LineCenter) {
        // No clipping, multiple lines per feature are allowed
        // "lines" with only one point are ignored as in clipLines
        for (const auto& line : feature.geometry) {
            if (line.size() > 1) {
                optional<Anchor> anchor = getCenterAnchor(line,
                                                          textMaxAngle,
                                                          (shapedTextOrientations.vertical ?: getDefaultHorizontalShaping(shapedTextOrientations)).left,
                                                          (shapedTextOrientations.vertical ?: getDefaultHorizontalShaping(shapedTextOrientations)).right,
                                                          (shapedIcon ? shapedIcon->left() : 0),
                                                          (shapedIcon ? shapedIcon->right() : 0),
                                                          glyphSize,
                                                          textMaxBoxScale);
                if (anchor) {
                    addSymbolInstance(*anchor, createSymbolInstanceSharedData(line));
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
            addSymbolInstance(anchor, createSymbolInstanceSharedData(polygon[0]));
        }
    } else if (type == FeatureType::LineString) {
        for (const auto& line : feature.geometry) {
            Anchor anchor(line[0].x, line[0].y, 0, minScale);
            addSymbolInstance(anchor, createSymbolInstanceSharedData(line));
        }
    } else if (type == FeatureType::Point) {
        for (const auto& points : feature.geometry) {
            for (const auto& point : points) {
                Anchor anchor(point.x, point.y, 0, minScale);
                addSymbolInstance(anchor, createSymbolInstanceSharedData({point}));
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
std::vector<float> SymbolLayout::calculateTileDistances(const GeometryCoordinates& line, const Anchor& anchor) {
    std::vector<float> tileDistances(line.size());
    if (anchor.segment) {
        std::size_t segment = *anchor.segment;
        assert(segment < line.size());
        auto sumForwardLength = (segment + 1 < line.size()) ? util::dist<float>(anchor.point, line[segment + 1]) : .0f;
        auto sumBackwardLength = util::dist<float>(anchor.point, line[segment]);
        for (std::size_t i = segment + 1; i < line.size(); ++i) {
            tileDistances[i] = sumForwardLength;
            if (i < line.size() - 1) {
                sumForwardLength += util::dist<float>(line[i + 1], line[i]);
            }
        }
        for (std::size_t i = ++segment; i-- > 0;) {
            tileDistances[i] = sumBackwardLength;
            if (i != 0u) {
                sumBackwardLength += util::dist<float>(line[i - 1], line[i]);
            }
        }
    }
    return tileDistances;
}

void SymbolLayout::createBucket(const ImagePositions&, std::unique_ptr<FeatureIndex>&, std::unordered_map<std::string, LayerRenderData>& renderData, const bool firstLoad, const bool showCollisionBoxes) {
    auto bucket = std::make_shared<SymbolBucket>(layout,
                                                 layerPaintProperties,
                                                 textSize,
                                                 iconSize,
                                                 zoom,
                                                 iconsNeedLinear,
                                                 sortFeaturesByY,
                                                 bucketLeaderID,
                                                 std::move(symbolInstances),
                                                 tilePixelRatio,
                                                 allowVerticalPlacement,
                                                 std::move(placementModes),
                                                 iconsInText);

    for (SymbolInstance &symbolInstance : bucket->symbolInstances) {
        const bool hasText = symbolInstance.hasText();
        const bool hasIcon = symbolInstance.hasIcon();
        const bool singleLine = symbolInstance.singleLine;

        const auto& feature = features.at(symbolInstance.layoutFeatureIndex);

        // Insert final placement into collision tree and add glyphs/icons to buffers

        // Process icon first, so that text symbols would have reference to iconIndex which
        // is used when dynamic vertices for icon-text-fit image have to be updated.
        if (hasIcon) {
            const Range<float> sizeData = bucket->iconSizeBinder->getVertexSizeData(feature);
            auto& iconBuffer = symbolInstance.hasSdfIcon() ? bucket->sdfIcon : bucket->icon;
            const auto placeIcon = [&](const SymbolQuad& iconQuad, auto& index, const WritingModeType writingMode) {
                iconBuffer.placedSymbols.emplace_back(symbolInstance.anchor.point,
                                                      symbolInstance.anchor.segment.value_or(0u),
                                                      sizeData.min,
                                                      sizeData.max,
                                                      symbolInstance.iconOffset,
                                                      writingMode,
                                                      symbolInstance.line(),
                                                      std::vector<float>());
                index = iconBuffer.placedSymbols.size() - 1;
                PlacedSymbol& iconSymbol = iconBuffer.placedSymbols.back();
                iconSymbol.angle = (allowVerticalPlacement && writingMode == WritingModeType::Vertical) ? M_PI_2 : 0;
                iconSymbol.vertexStartIndex = addSymbol(iconBuffer, sizeData, iconQuad,
                                                        symbolInstance.anchor, iconSymbol, feature.sortKey);
            };

            placeIcon(*symbolInstance.iconQuad(), symbolInstance.placedIconIndex, WritingModeType::None);
            if (symbolInstance.verticalIconQuad()) {
                placeIcon(*symbolInstance.verticalIconQuad(), symbolInstance.placedVerticalIconIndex, WritingModeType::Vertical);
            }

            for (auto& pair : bucket->paintProperties) {
                pair.second.iconBinders.populateVertexVectors(feature, iconBuffer.vertices.elements(),
                                                              symbolInstance.dataFeatureIndex, {}, {});
            }
        }

        if (hasText && feature.formattedText) {
            optional<std::size_t> lastAddedSection;
            if (singleLine) {
                optional<std::size_t> placedTextIndex;
                lastAddedSection = addSymbolGlyphQuads(*bucket, symbolInstance, feature, symbolInstance.writingModes, placedTextIndex, symbolInstance.rightJustifiedGlyphQuads(), lastAddedSection);
                symbolInstance.placedRightTextIndex = placedTextIndex;
                symbolInstance.placedCenterTextIndex = placedTextIndex;
                symbolInstance.placedLeftTextIndex = placedTextIndex;
            } else {
                if (symbolInstance.rightJustifiedGlyphQuadsSize) {
                    lastAddedSection = addSymbolGlyphQuads(*bucket, symbolInstance, feature, symbolInstance.writingModes, symbolInstance.placedRightTextIndex, symbolInstance.rightJustifiedGlyphQuads(), lastAddedSection);
                }
                if (symbolInstance.centerJustifiedGlyphQuadsSize) {
                    lastAddedSection = addSymbolGlyphQuads(*bucket, symbolInstance, feature, symbolInstance.writingModes, symbolInstance.placedCenterTextIndex, symbolInstance.centerJustifiedGlyphQuads(), lastAddedSection);
                }
                if (symbolInstance.leftJustifiedGlyphQuadsSize) {
                    lastAddedSection = addSymbolGlyphQuads(*bucket, symbolInstance, feature, symbolInstance.writingModes, symbolInstance.placedLeftTextIndex, symbolInstance.leftJustifiedGlyphQuads(), lastAddedSection);
                }
            }
            if (symbolInstance.writingModes & WritingModeType::Vertical && symbolInstance.verticalGlyphQuadsSize) {
                lastAddedSection = addSymbolGlyphQuads(*bucket, symbolInstance, feature, WritingModeType::Vertical, symbolInstance.placedVerticalTextIndex, symbolInstance.verticalGlyphQuads(), lastAddedSection);
            }
            assert(lastAddedSection); // True, as hasText == true;
            updatePaintPropertiesForSection(*bucket, feature, *lastAddedSection);
        }

        symbolInstance.releaseSharedData();
    }

    if (showCollisionBoxes) {
        addToDebugBuffers(*bucket);
    }
    if (bucket->hasData()){
        for (const auto& pair : layerPaintProperties) {
            if (!firstLoad) {
                bucket->justReloaded = true;
            }
            renderData.emplace(pair.first, LayerRenderData{bucket, pair.second});
        }
    }

}

void SymbolLayout::updatePaintPropertiesForSection(SymbolBucket& bucket,
                                                   const SymbolFeature& feature,
                                                   std::size_t sectionIndex) {
    const auto& formattedSection = sectionOptionsToValue((*feature.formattedText).sectionAt(sectionIndex));
    for (auto& pair : bucket.paintProperties) {
        pair.second.textBinders.populateVertexVectors(feature, bucket.text.vertices.elements(), feature.index, {}, {},
                                                      formattedSection);
    }
}

std::size_t SymbolLayout::addSymbolGlyphQuads(SymbolBucket& bucket,
                                              SymbolInstance& symbolInstance,
                                              const SymbolFeature& feature,
                                              WritingModeType writingMode,
                                              optional<size_t>& placedIndex,
                                              const SymbolQuads& glyphQuads,
                                              optional<std::size_t> lastAddedSection) {
    const Range<float> sizeData = bucket.textSizeBinder->getVertexSizeData(feature);
    const bool hasFormatSectionOverrides = bucket.hasFormatSectionOverrides();
    const auto& placedIconIndex = writingMode == WritingModeType::Vertical ? symbolInstance.placedVerticalIconIndex : symbolInstance.placedIconIndex;
    bucket.text.placedSymbols.emplace_back(symbolInstance.anchor.point,
                                           symbolInstance.anchor.segment.value_or(0u),
                                           sizeData.min,
                                           sizeData.max,
                                           symbolInstance.textOffset,
                                           writingMode,
                                           symbolInstance.line(),
                                           calculateTileDistances(symbolInstance.line(), symbolInstance.anchor),
                                           placedIconIndex);
    placedIndex = bucket.text.placedSymbols.size() - 1;
    PlacedSymbol& placedSymbol = bucket.text.placedSymbols.back();
    placedSymbol.angle = (allowVerticalPlacement && writingMode == WritingModeType::Vertical) ? M_PI_2 : 0;

    bool firstSymbol = true;
    for (const auto& symbolQuad : glyphQuads) {
        if (hasFormatSectionOverrides) {
            if (lastAddedSection && *lastAddedSection != symbolQuad.sectionIndex) {
                updatePaintPropertiesForSection(bucket, feature, *lastAddedSection);
            }
            lastAddedSection = symbolQuad.sectionIndex;
        }
        size_t index = addSymbol(bucket.text, sizeData, symbolQuad, symbolInstance.anchor, placedSymbol, feature.sortKey);
        if (firstSymbol) {
            placedSymbol.vertexStartIndex = index;
            firstSymbol = false;
        }
    }

    return lastAddedSection ? *lastAddedSection : 0u;
}

size_t SymbolLayout::addSymbol(SymbolBucket::Buffer& buffer,
                               const Range<float> sizeData,
                               const SymbolQuad& symbol,
                               const Anchor& labelAnchor,
                               PlacedSymbol& placedSymbol,
                               float sortKey) {
    constexpr const uint16_t vertexLength = 4;

    const auto &tl = symbol.tl;
    const auto &tr = symbol.tr;
    const auto &bl = symbol.bl;
    const auto &br = symbol.br;
    const auto &tex = symbol.tex;

    if (buffer.segments.empty() ||
        buffer.segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max() ||
        fabs(buffer.segments.back().sortKey - sortKey) > std::numeric_limits<float>::epsilon()) {
        buffer.segments.emplace_back(buffer.vertices.elements(), buffer.triangles.elements(), 0ul, 0ul, sortKey);
    }

    // We're generating triangle fans, so we always start with the first
    // coordinate in this polygon.
    auto& segment = buffer.segments.back();
    assert(segment.vertexLength <= std::numeric_limits<uint16_t>::max());
    uint16_t index = segment.vertexLength;

    // coordinates (2 triangles)
    buffer.vertices.emplace_back(SymbolSDFIconProgram::layoutVertex(
        labelAnchor.point, tl, symbol.glyphOffset.y, tex.x, tex.y, sizeData, symbol.isSDF));
    buffer.vertices.emplace_back(SymbolSDFIconProgram::layoutVertex(
        labelAnchor.point, tr, symbol.glyphOffset.y, tex.x + tex.w, tex.y, sizeData, symbol.isSDF));
    buffer.vertices.emplace_back(SymbolSDFIconProgram::layoutVertex(
        labelAnchor.point, bl, symbol.glyphOffset.y, tex.x, tex.y + tex.h, sizeData, symbol.isSDF));
    buffer.vertices.emplace_back(SymbolSDFIconProgram::layoutVertex(
        labelAnchor.point, br, symbol.glyphOffset.y, tex.x + tex.w, tex.y + tex.h, sizeData, symbol.isSDF));

    // Dynamic/Opacity vertices are initialized so that the vertex count always agrees with
    // the layout vertex buffer, but they will always be updated before rendering happens
    auto dynamicVertex = SymbolSDFIconProgram::dynamicLayoutVertex(labelAnchor.point, 0);
    buffer.dynamicVertices.emplace_back(dynamicVertex);
    buffer.dynamicVertices.emplace_back(dynamicVertex);
    buffer.dynamicVertices.emplace_back(dynamicVertex);
    buffer.dynamicVertices.emplace_back(dynamicVertex);

    auto opacityVertex = SymbolSDFIconProgram::opacityVertex(1.0, 1.0);
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

    for (const SymbolInstance& symbolInstance : symbolInstances) {
        auto populateCollisionBox = [&](const auto& feature, bool isText) {
            SymbolBucket::CollisionBuffer& collisionBuffer =
                feature.alongLine ? (isText ? static_cast<SymbolBucket::CollisionBuffer&>(bucket.getOrCreateTextCollisionCircleBuffer())
                                            : static_cast<SymbolBucket::CollisionBuffer&>(bucket.getOrCreateIconCollisionCircleBuffer()))
                                  : (isText ? static_cast<SymbolBucket::CollisionBuffer&>(bucket.getOrCreateTextCollisionBox())
                                            : static_cast<SymbolBucket::CollisionBuffer&>(bucket.getOrCreateIconCollisionBox()));

            for (const CollisionBox& box : feature.boxes) {
                auto& anchor = box.anchor;

                Point<float> tl{box.x1, box.y1};
                Point<float> tr{box.x2, box.y1};
                Point<float> bl{box.x1, box.y2};
                Point<float> br{box.x2, box.y2};

                static constexpr std::size_t vertexLength = 4;
                const std::size_t indexLength = feature.alongLine ? 6 : 8;

                if (collisionBuffer.segments.empty() || collisionBuffer.segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max()) {
                    collisionBuffer.segments.emplace_back(
                        collisionBuffer.vertices.elements(),
                        feature.alongLine
                            ? (isText ? bucket.textCollisionCircle->triangles.elements() : bucket.iconCollisionCircle->triangles.elements())
                            : (isText ? bucket.textCollisionBox->lines.elements() : bucket.iconCollisionBox->lines.elements()));
                }

                auto& segment = collisionBuffer.segments.back();
                uint16_t index = segment.vertexLength;

                collisionBuffer.vertices.emplace_back(CollisionBoxProgram::layoutVertex(anchor, symbolInstance.anchor.point, tl));
                collisionBuffer.vertices.emplace_back(CollisionBoxProgram::layoutVertex(anchor, symbolInstance.anchor.point, tr));
                collisionBuffer.vertices.emplace_back(CollisionBoxProgram::layoutVertex(anchor, symbolInstance.anchor.point, br));
                collisionBuffer.vertices.emplace_back(CollisionBoxProgram::layoutVertex(anchor, symbolInstance.anchor.point, bl));

                // Dynamic vertices are initialized so that the vertex count always agrees with
                // the layout vertex buffer, but they will always be updated before rendering happens
                auto dynamicVertex = CollisionBoxProgram::dynamicVertex(false, false, {});
                collisionBuffer.dynamicVertices.emplace_back(dynamicVertex);
                collisionBuffer.dynamicVertices.emplace_back(dynamicVertex);
                collisionBuffer.dynamicVertices.emplace_back(dynamicVertex);
                collisionBuffer.dynamicVertices.emplace_back(dynamicVertex);

                if (feature.alongLine) {
                    auto& collisionCircle = (isText ? bucket.textCollisionCircle : bucket.iconCollisionCircle);
                    collisionCircle->triangles.emplace_back(index, index + 1, index + 2);
                    collisionCircle->triangles.emplace_back(index, index + 2, index + 3);
                } else {
                    auto& collisionBox = (isText ? bucket.textCollisionBox : bucket.iconCollisionBox);
                    collisionBox->lines.emplace_back(index + 0, index + 1);
                    collisionBox->lines.emplace_back(index + 1, index + 2);
                    collisionBox->lines.emplace_back(index + 2, index + 3);
                    collisionBox->lines.emplace_back(index + 3, index + 0);
                }

                segment.vertexLength += vertexLength;
                segment.indexLength += indexLength;
            }
        };
        populateCollisionBox(symbolInstance.textCollisionFeature, true /*isText*/);
        if (symbolInstance.verticalTextCollisionFeature) {
            populateCollisionBox(*symbolInstance.verticalTextCollisionFeature, true /*isText*/);
        }
        if (symbolInstance.verticalIconCollisionFeature) {
            populateCollisionBox(*symbolInstance.verticalIconCollisionFeature, false /*isText*/);
        }
        populateCollisionBox(symbolInstance.iconCollisionFeature, false /*isText*/);
    }
}

} // namespace mbgl
