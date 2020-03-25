// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/expression/formatted.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace style {

class TransitionOptions;

class LocationComponentLayer : public Layer {
public:
    LocationComponentLayer(const std::string& layerID);
    ~LocationComponentLayer() final;

    // Layout properties

    static PropertyValue<float> getDefaultAccuracyRadius();
    const PropertyValue<float>& getAccuracyRadius() const;
    void setAccuracyRadius(const PropertyValue<float>&);

    static PropertyValue<float> getDefaultBearing();
    const PropertyValue<float>& getBearing() const;
    void setBearing(const PropertyValue<float>&);

    static PropertyValue<expression::Image> getDefaultBearingImage();
    const PropertyValue<expression::Image>& getBearingImage() const;
    void setBearingImage(const PropertyValue<expression::Image>&);

    static PropertyValue<float> getDefaultBearingImageSize();
    const PropertyValue<float>& getBearingImageSize() const;
    void setBearingImageSize(const PropertyValue<float>&);

    static PropertyValue<float> getDefaultImageTiltDisplacement();
    const PropertyValue<float>& getImageTiltDisplacement() const;
    void setImageTiltDisplacement(const PropertyValue<float>&);

    static PropertyValue<std::array<double, 3>> getDefaultLocation();
    const PropertyValue<std::array<double, 3>>& getLocation() const;
    void setLocation(const PropertyValue<std::array<double, 3>>&);

    static PropertyValue<float> getDefaultPerspectiveCompensation();
    const PropertyValue<float>& getPerspectiveCompensation() const;
    void setPerspectiveCompensation(const PropertyValue<float>&);

    static PropertyValue<expression::Image> getDefaultShadowImage();
    const PropertyValue<expression::Image>& getShadowImage() const;
    void setShadowImage(const PropertyValue<expression::Image>&);

    static PropertyValue<float> getDefaultShadowImageSize();
    const PropertyValue<float>& getShadowImageSize() const;
    void setShadowImageSize(const PropertyValue<float>&);

    static PropertyValue<expression::Image> getDefaultTopImage();
    const PropertyValue<expression::Image>& getTopImage() const;
    void setTopImage(const PropertyValue<expression::Image>&);

    static PropertyValue<float> getDefaultTopImageSize();
    const PropertyValue<float>& getTopImageSize() const;
    void setTopImageSize(const PropertyValue<float>&);

    // Paint properties

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

    // Private implementation

    class Impl;
    const Impl& impl() const;

    Mutable<Impl> mutableImpl() const;
    LocationComponentLayer(Immutable<Impl>);
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
