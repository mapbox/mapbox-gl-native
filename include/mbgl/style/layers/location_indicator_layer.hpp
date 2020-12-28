// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/rotation.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TransitionOptions;

class LocationIndicatorLayer final : public Layer {
public:
    LocationIndicatorLayer(const std::string& layerID);
    ~LocationIndicatorLayer() override;

    // Layout properties

    static PropertyValue<expression::Image> getDefaultBearingImage();
    const PropertyValue<expression::Image>& getBearingImage() const;
    void setBearingImage(const PropertyValue<expression::Image>&);

    static PropertyValue<expression::Image> getDefaultShadowImage();
    const PropertyValue<expression::Image>& getShadowImage() const;
    void setShadowImage(const PropertyValue<expression::Image>&);

    static PropertyValue<expression::Image> getDefaultTopImage();
    const PropertyValue<expression::Image>& getTopImage() const;
    void setTopImage(const PropertyValue<expression::Image>&);

    // Paint properties

    static PropertyValue<float> getDefaultAccuracyRadius();
    const PropertyValue<float>& getAccuracyRadius() const;
    void setAccuracyRadius(const PropertyValue<float>&);
    void setAccuracyRadiusTransition(const TransitionOptions&);
    TransitionOptions getAccuracyRadiusTransition() const;

    static PropertyValue<Color> getDefaultAccuracyRadiusBorderColor();
    const PropertyValue<Color>& getAccuracyRadiusBorderColor() const;
    void setAccuracyRadiusBorderColor(const PropertyValue<Color>&);
    void setAccuracyRadiusBorderColorTransition(const TransitionOptions&);
    TransitionOptions getAccuracyRadiusBorderColorTransition() const;

    static PropertyValue<Color> getDefaultAccuracyRadiusColor();
    const PropertyValue<Color>& getAccuracyRadiusColor() const;
    void setAccuracyRadiusColor(const PropertyValue<Color>&);
    void setAccuracyRadiusColorTransition(const TransitionOptions&);
    TransitionOptions getAccuracyRadiusColorTransition() const;

    static PropertyValue<Rotation> getDefaultBearing();
    const PropertyValue<Rotation>& getBearing() const;
    void setBearing(const PropertyValue<Rotation>&);
    void setBearingTransition(const TransitionOptions&);
    TransitionOptions getBearingTransition() const;

    static PropertyValue<float> getDefaultBearingImageSize();
    const PropertyValue<float>& getBearingImageSize() const;
    void setBearingImageSize(const PropertyValue<float>&);
    void setBearingImageSizeTransition(const TransitionOptions&);
    TransitionOptions getBearingImageSizeTransition() const;

    static PropertyValue<float> getDefaultImageTiltDisplacement();
    const PropertyValue<float>& getImageTiltDisplacement() const;
    void setImageTiltDisplacement(const PropertyValue<float>&);
    void setImageTiltDisplacementTransition(const TransitionOptions&);
    TransitionOptions getImageTiltDisplacementTransition() const;

    static PropertyValue<std::array<double, 3>> getDefaultLocation();
    const PropertyValue<std::array<double, 3>>& getLocation() const;
    void setLocation(const PropertyValue<std::array<double, 3>>&);
    void setLocationTransition(const TransitionOptions&);
    TransitionOptions getLocationTransition() const;

    static PropertyValue<float> getDefaultPerspectiveCompensation();
    const PropertyValue<float>& getPerspectiveCompensation() const;
    void setPerspectiveCompensation(const PropertyValue<float>&);
    void setPerspectiveCompensationTransition(const TransitionOptions&);
    TransitionOptions getPerspectiveCompensationTransition() const;

    static PropertyValue<float> getDefaultShadowImageSize();
    const PropertyValue<float>& getShadowImageSize() const;
    void setShadowImageSize(const PropertyValue<float>&);
    void setShadowImageSizeTransition(const TransitionOptions&);
    TransitionOptions getShadowImageSizeTransition() const;

    static PropertyValue<float> getDefaultTopImageSize();
    const PropertyValue<float>& getTopImageSize() const;
    void setTopImageSize(const PropertyValue<float>&);
    void setTopImageSizeTransition(const TransitionOptions&);
    TransitionOptions getTopImageSizeTransition() const;

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    LocationIndicatorLayer(Immutable<Impl>);
    std::unique_ptr<Layer> cloneRef(const std::string& id) const final;

protected:
    // Dynamic properties
    optional<conversion::Error> setPropertyInternal(const std::string& name, const conversion::Convertible& value) final;

    StyleProperty getProperty(const std::string& name) const final;
    Value serialize() const final;

    Mutable<Layer::Impl> mutableBaseImpl() const final;
};

} // namespace style
} // namespace mbgl

// clang-format on
