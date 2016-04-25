'use strict';

const fs = require('fs');
const ejs = require('ejs');
const spec = require('mapbox-gl-style-spec').latest;
var parseCSSColor = require('csscolorparser').parseCSSColor;

global.camelize = function (str) {
  return str.replace(/(?:^|-)(.)/g, function (_, x) {
    return x.toUpperCase();
  });
}

global.camelizeWithLeadingLowercase = function (str) {
  return str.replace(/-(.)/g, function (_, x) {
    return x.toUpperCase();
  });
}

global.propertyType = function (property) {
  if (/-translate-anchor$/.test(property.name)) {
    return 'TranslateAnchorType';
  }
  if (/-rotation-alignment$/.test(property.name)) {
    return 'RotationAlignmentType';
  }
  switch (property.type) {
  case 'boolean':
    return 'bool';
  case 'number':
    return 'float';
  case 'string':
    return 'std::string';
  case 'enum':
    return `${camelize(property.name)}Type`;
  case 'color':
    return `Color`;
  case 'array':
    if (property.length) {
      return `std::array<${propertyType({type: property.value})}, ${property.length}>`;
    } else {
      return `std::vector<${propertyType({type: property.value})}>`;
    }
  default: throw new Error(`unknown type for ${property.name}`)
  }
}

global.defaultValue = function (property) {
  switch (property.type) {
  case 'number':
    return property.default;
  case 'string':
    return JSON.stringify(property.default || "");
  case 'enum':
    return `${propertyType(property)}::${camelize(property.default)}`;
  case 'color':
    return `{{ ${parseCSSColor(property.default).join(', ')} }}`
  case 'array':
    const defaults = (property.default || []).map((e) => defaultValue({ type: property.value, default: e }));
    if (property.length) {
      return `{{ ${defaults.join(', ')} }}`;
    } else {
      return `{ ${defaults.join(', ')} }`;
    }
  default:
    return property.default;
  }
}

const layerHpp = ejs.compile(`<%
  const type = locals.type;
  const layoutProperties = locals.layoutProperties;
  const paintProperties = locals.paintProperties;
-%>
// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/property_value.hpp>

<% if (type === 'line' || type === 'symbol') { -%>
#include <vector>

<% } -%>
namespace mbgl {

class <%- camelize(type) %>Layer : public Layer {
public:
    <%- camelize(type) %>Layer(const std::string& layerID);
    ~<%- camelize(type) %>Layer() final;

<% if (type === 'raster') { -%>
    // Source

    void setSource(const std::string& sourceID);
    const std::string& getSourceID() const;

<% } else if (type !== 'background') { -%>
    // Source

    void setSource(const std::string& sourceID, const std::string& sourceLayer);
    const std::string& getSourceID() const;
    const std::string& getSourceLayer() const;

<% } -%>
<% if (layoutProperties.length) { -%>
    // Layout properties

<% for (const property of layoutProperties) { -%>
    PropertyValue<<%- propertyType(property) %>> get<%- camelize(property.name) %>() const;
    void set<%- camelize(property.name) %>(PropertyValue<<%- propertyType(property) %>>);

<% } -%>
<% } -%>
    // Paint properties

<% for (const property of paintProperties) { -%>
    PropertyValue<<%- propertyType(property) %>> get<%- camelize(property.name) %>() const;
    void set<%- camelize(property.name) %>(PropertyValue<<%- propertyType(property) %>>);

<% } -%>
    // Private implementation

    class Impl;
    Impl* const impl;

    <%- camelize(type) %>Layer(const Impl&);
    <%- camelize(type) %>Layer(const <%- camelize(type) %>Layer&) = delete;
};

template <>
inline bool Layer::is<<%- camelize(type) %>Layer>() const {
    return type == Type::<%- camelize(type) %>;
}

} // namespace mbgl
`, {strict: true});

const layerCpp = ejs.compile(`<%
  const type = locals.type;
  const layoutProperties = locals.layoutProperties;
  const paintProperties = locals.paintProperties;
-%>
// This file is generated. Edit scripts/generate-style-code.js, then run \`make style-code\`.

#include <mbgl/layer/<%- type %>_layer.hpp>
#include <mbgl/layer/<%- type %>_layer_impl.hpp>

namespace mbgl {

<%- camelize(type) %>Layer::<%- camelize(type) %>Layer(const std::string& layerID)
    : Layer(Type::<%- camelize(type) %>, std::make_unique<Impl>())
    , impl(static_cast<Impl*>(baseImpl.get())) {
    impl->id = layerID;
}

<%- camelize(type) %>Layer::<%- camelize(type) %>Layer(const Impl& other)
    : Layer(Type::<%- camelize(type) %>, std::make_unique<Impl>(other))
    , impl(static_cast<Impl*>(baseImpl.get())) {
}

<%- camelize(type) %>Layer::~<%- camelize(type) %>Layer() = default;

std::unique_ptr<Layer> <%- camelize(type) %>Layer::Impl::clone() const {
    return std::make_unique<<%- camelize(type) %>Layer>(*this);
}

<% if (type === 'raster') { -%>
// Source

void <%- camelize(type) %>Layer::setSource(const std::string& sourceID) {
    impl->source = sourceID;
}

const std::string& <%- camelize(type) %>Layer::getSourceID() const {
    return impl->source;
}
<% } else if (type !== 'background') { -%>
// Source

void <%- camelize(type) %>Layer::setSource(const std::string& sourceID, const std::string& sourceLayer) {
    impl->source = sourceID;
    impl->sourceLayer = sourceLayer;
}

const std::string& <%- camelize(type) %>Layer::getSourceID() const {
    return impl->source;
}

const std::string& <%- camelize(type) %>Layer::getSourceLayer() const {
    return impl->sourceLayer;
}
<% } -%>

// Layout properties

<% for (const property of layoutProperties) { -%>
PropertyValue<<%- propertyType(property) %>> <%- camelize(type) %>Layer::get<%- camelize(property.name) %>() const {
    return impl->layout.<%- camelizeWithLeadingLowercase(property.name) %>.get();
}

void <%- camelize(type) %>Layer::set<%- camelize(property.name) %>(PropertyValue<<%- propertyType(property) %>> value) {
    impl->layout.<%- camelizeWithLeadingLowercase(property.name) %>.set(value);
}
<% } -%>

// Paint properties
<% for (const property of paintProperties) { %>
PropertyValue<<%- propertyType(property) %>> <%- camelize(type) %>Layer::get<%- camelize(property.name) %>() const {
    return impl->paint.<%- camelizeWithLeadingLowercase(property.name) %>.get();
}

void <%- camelize(type) %>Layer::set<%- camelize(property.name) %>(PropertyValue<<%- propertyType(property) %>> value) {
    impl->paint.<%- camelizeWithLeadingLowercase(property.name) %>.set(value);
}
<% } -%>

} // namespace mbgl
`, {strict: true});

const propertiesHpp = ejs.compile(`<%
  const type = locals.type;
  const layoutProperties = locals.layoutProperties;
  const paintProperties = locals.paintProperties;
-%>
// This file is generated. Edit scripts/generate-style-code.js, then run \`make style-code\`.

#pragma once

#include <mbgl/style/layout_property.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/util/rapidjson.hpp>

namespace mbgl {

class StyleCascadeParameters;
class StyleCalculationParameters;

<% if (layoutProperties.length) { -%>
class <%- camelize(type) %>LayoutProperties {
public:
    void parse(const JSValue&);
    void recalculate(const StyleCalculationParameters&);

<% for (const property of layoutProperties) { -%>
    LayoutProperty<<%- propertyType(property) %>> <%- camelizeWithLeadingLowercase(property.name) %> { <%- defaultValue(property) %> };
<% } -%>
};

<% } -%>
class <%- camelize(type) %>PaintProperties {
public:
    void parse(const JSValue&);
    void cascade(const StyleCascadeParameters&);
    bool recalculate(const StyleCalculationParameters&);

<% for (const property of paintProperties) { -%>
<% if (/-pattern$/.test(property.name) || property.name === 'line-dasharray') { -%>
    PaintProperty<<%- propertyType(property) %>, CrossFadedPropertyEvaluator> <%- camelizeWithLeadingLowercase(property.name) %> { <%- defaultValue(property) %> };
<% } else if (property.name === 'fill-outline-color') { -%>
    PaintProperty<<%- propertyType(property) %>> <%- camelizeWithLeadingLowercase(property.name) %> { {{ 0, 0, 0, -1 }} };
<% } else { -%>
    PaintProperty<<%- propertyType(property) %>> <%- camelizeWithLeadingLowercase(property.name) %> { <%- defaultValue(property) %> };
<% } -%>
<% } -%>
};

} // namespace mbgl
`, {strict: true});

const propertiesCpp = ejs.compile(`<%
  const type = locals.type;
  const layoutProperties = locals.layoutProperties;
  const paintProperties = locals.paintProperties;
-%>
// This file is generated. Edit scripts/generate-style-code.js, then run \`make style-code\`.

#include <mbgl/layer/<%- type %>_layer_properties.hpp>

namespace mbgl {

<% if (layoutProperties.length) { -%>
void <%- camelize(type) %>LayoutProperties::parse(const JSValue& value) {
<% for (const property of layoutProperties) { -%>
    <%- camelizeWithLeadingLowercase(property.name) %>.parse(<%- JSON.stringify(property.name) %>, value);
<% } -%>
}

void <%- camelize(type) %>LayoutProperties::recalculate(const StyleCalculationParameters& parameters) {
<% for (const property of layoutProperties) { -%>
    <%- camelizeWithLeadingLowercase(property.name) %>.calculate(parameters);
<% } -%>
}

<% } -%>
void <%- camelize(type) %>PaintProperties::parse(const JSValue& value) {
<% for (const property of paintProperties) { -%>
    <%- camelizeWithLeadingLowercase(property.name) %>.parse(<%- JSON.stringify(property.name) %>, value);
<% } -%>
}

void <%- camelize(type) %>PaintProperties::cascade(const StyleCascadeParameters& parameters) {
<% for (const property of paintProperties) { -%>
    <%- camelizeWithLeadingLowercase(property.name) %>.cascade(parameters);
<% } -%>
}

bool <%- camelize(type) %>PaintProperties::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

<% for (const property of paintProperties) { -%>
    hasTransitions |= <%- camelizeWithLeadingLowercase(property.name) %>.calculate(parameters);
<% } -%>

    return hasTransitions;
}

} // namespace mbgl
`, {strict: true});

for (const type of spec.layer.type.values) {
  const layoutProperties = Object.keys(spec[`layout_${type}`]).reduce((memo, name) => {
    if (name !== 'visibility') {
      spec[`layout_${type}`][name].name = name;
      memo.push(spec[`layout_${type}`][name]);
    }
    return memo;
  }, []);

  const paintProperties = Object.keys(spec[`paint_${type}`]).reduce((memo, name) => {
    spec[`paint_${type}`][name].name = name;
    memo.push(spec[`paint_${type}`][name]);
    return memo;
  }, []);

  const layer = {
    type: type,
    layoutProperties: layoutProperties,
    paintProperties: paintProperties,
  };

  fs.writeFileSync(`include/mbgl/layer/${type}_layer.hpp`, layerHpp(layer));
  fs.writeFileSync(`src/mbgl/layer/${type}_layer.cpp`, layerCpp(layer));

  fs.writeFileSync(`src/mbgl/layer/${type}_layer_properties.hpp`, propertiesHpp(layer));
  fs.writeFileSync(`src/mbgl/layer/${type}_layer_properties.cpp`, propertiesCpp(layer));
}
