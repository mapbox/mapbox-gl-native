#!/usr/bin/env node
'use strict';

const fs = require('fs');
const ejs = require('ejs');
const spec = require('./style-spec');
const colorParser = require('csscolorparser');

// FIXME: https://github.com/mapbox/mapbox-gl-native/issues/15008
delete spec.layout_circle["circle-sort-key"]
delete spec.layout_line["line-sort-key"]
delete spec.layout_fill["fill-sort-key"]

require('./style-code');

function parseCSSColor(str) {
  const color = colorParser.parseCSSColor(str);
  return [
      color[0] / 255 * color[3], color[1] / 255 * color[3], color[2] / 255 * color[3], color[3]
  ];
}

global.isLightProperty = function (property) {
  return property['light-property'] === true;
};

global.isOverridable = function (property) {
    return ['text-color'].includes(property.name);
};

global.expressionType = function (property) {
    switch (property.type) {
        case 'boolean':
            return 'BooleanType';
        case 'number':
        case 'enum':
            return 'NumberType';
        case 'string':
            return 'StringType';
        case 'color':
            return `ColorType`;
        case 'formatted':
            return `FormattedType`;
        case 'array':
            return `Array<${expressionType({type: property.value})}>`;
        default: throw new Error(`unknown type for ${property.name}`)
    }
};

global.evaluatedType = function (property) {
  if (/-translate-anchor$/.test(property.name)) {
    return 'TranslateAnchorType';
  }
  if (/-(rotation|pitch|illumination)-alignment$/.test(property.name)) {
    return 'AlignmentType';
  }
  if (/^(text|icon)-anchor$/.test(property.name)) {
    return 'SymbolAnchorType';
  }
  if (/position/.test(property.name)) {
    return 'Position';
  }
  switch (property.type) {
  case 'boolean':
    return 'bool';
  case 'number':
    return 'float';
  case 'formatted':
    return 'expression::Formatted';
  case 'string':
    return 'std::string';
  case 'enum':
    return (isLightProperty(property) ? 'Light' : '') + `${camelize(property.name)}Type`;
  case 'color':
    return `Color`;
  case 'array':
    if (property.length) {
      return `std::array<${evaluatedType({type: property.value})}, ${property.length}>`;
    } else {
      return `std::vector<${evaluatedType({type: property.value, name: property.name})}>`;
    }
  default: throw new Error(`unknown type for ${property.name}`)
  }
};

function attributeUniformType(property, type) {
    const attributeNameExceptions = {
      'text-opacity': ['opacity'],
      'icon-opacity': ['opacity'],
      'text-color': ['fill_color'],
      'icon-color': ['fill_color'],
      'text-halo-color': ['halo_color'],
      'icon-halo-color': ['halo_color'],
      'text-halo-blur': ['halo_blur'],
      'icon-halo-blur': ['halo_blur'],
      'text-halo-width': ['halo_width'],
      'icon-halo-width': ['halo_width'],
      'line-gap-width': ['gapwidth'],
      'line-pattern': ['pattern_to', 'pattern_from'],
      'line-floor-width': ['floorwidth'],
      'fill-pattern': ['pattern_to', 'pattern_from'],
      'fill-extrusion-pattern': ['pattern_to', 'pattern_from']
    }
    const names = attributeNameExceptions[property.name] ||
       [ property.name.replace(type + '-', '').replace(/-/g, '_') ];

    return names.map(name => {
      return `attributes::${name}, uniforms::${name}`
    }).join(', ');
}

global.layoutPropertyType = function (property) {
  switch (property['property-type']) {
    case 'data-driven':
    case 'cross-faded-data-driven':
      return `DataDrivenLayoutProperty<${evaluatedType(property)}>`;
    default:
      return `LayoutProperty<${evaluatedType(property)}>`;
  }
};

global.paintPropertyType = function (property, type) {
  switch (property['property-type']) {
    case 'data-driven':
      if (isOverridable(property))
          return `DataDrivenPaintProperty<${evaluatedType(property)}, ${attributeUniformType(property, type)}, true>`;
      return `DataDrivenPaintProperty<${evaluatedType(property)}, ${attributeUniformType(property, type)}>`;
    case 'cross-faded-data-driven':
      return `CrossFadedDataDrivenPaintProperty<${evaluatedType(property)}, ${attributeUniformType(property, type)}>`;
    case 'cross-faded':
      return `CrossFadedPaintProperty<${evaluatedType(property)}>`;
    default:
      return `PaintProperty<${evaluatedType(property)}>`;
  }
};

global.propertyValueType = function (property) {
  switch (property['property-type']) {
    case 'color-ramp':
      return `ColorRampPropertyValue`;
    default:
      return `PropertyValue<${evaluatedType(property)}>`;
  }
};

global.defaultValue = function (property) {
  // https://github.com/mapbox/mapbox-gl-native/issues/5258
  if (property.name === 'line-round-limit') {
    return 1;
  }

  if (property.name === 'fill-outline-color') {
    return '{}';
  }

  if (property['property-type'] === 'color-ramp') {
      return '{}';
  }

  switch (property.type) {
  case 'number':
  if (property.default === undefined) {
    return 0;
  } else {
    return property.default;
  }
  case 'formatted':
  case 'string':
    return JSON.stringify(property.default || "");
  case 'enum':
    if (property.default === undefined) {
      return `${evaluatedType(property)}::Undefined`;
    } else {
      return `${evaluatedType(property)}::${camelize(property.default)}`;
    }
  case 'color':
    const color = parseCSSColor(property.default).join(', ');
    switch (color) {
    case '0, 0, 0, 0':
      return '{}';
    case '0, 0, 0, 1':
      return 'Color::black()';
    case '1, 1, 1, 1':
      return 'Color::white()';
    default:
      return `{ ${color} }`;
    }
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
};

const layerHpp = ejs.compile(fs.readFileSync('include/mbgl/style/layers/layer.hpp.ejs', 'utf8'), {strict: true});
const layerCpp = ejs.compile(fs.readFileSync('src/mbgl/style/layers/layer.cpp.ejs', 'utf8'), {strict: true});
const propertiesHpp = ejs.compile(fs.readFileSync('src/mbgl/style/layers/layer_properties.hpp.ejs', 'utf8'), {strict: true});
const propertiesCpp = ejs.compile(fs.readFileSync('src/mbgl/style/layers/layer_properties.cpp.ejs', 'utf8'), {strict: true});

const collator = new Intl.Collator("en-US");

// Add this mock property that our SDF line shader needs so that it gets added to the list of
// "data driven" properties.
spec.paint_line['line-floor-width'] = {
  "type": "number",
  "default": 1,
  "property-type": "data-driven"
};

const layers = Object.keys(spec.layer.type.values).map((type) => {
  const layoutProperties = Object.keys(spec[`layout_${type}`]).reduce((memo, name) => {
    if (name !== 'visibility') {
      spec[`layout_${type}`][name].name = name;
      memo.push(spec[`layout_${type}`][name]);
    }
    return memo;
  }, []);

  // JSON doesn't have a defined order. We're going to sort them alphabetically
  // to get a deterministic order.
  layoutProperties.sort((a, b) => collator.compare(a.name, b.name));

  const paintProperties = Object.keys(spec[`paint_${type}`]).reduce((memo, name) => {
    spec[`paint_${type}`][name].name = name;
    memo.push(spec[`paint_${type}`][name]);
    return memo;
  }, []);

  // JSON doesn't have a defined order. We're going to sort them alphabetically
  // to get a deterministic order.
  paintProperties.sort((a, b) => collator.compare(a.name, b.name));

  return {
    type: type,
    layoutProperties: layoutProperties,
    paintProperties: paintProperties,
    doc: spec.layer.type.values[type].doc,
    layoutPropertiesByName: spec[`layout_${type}`],
    paintPropertiesByName: spec[`paint_${type}`],
  };
});

for (const layer of layers) {
  const layerFileName = layer.type.replace('-', '_');

  writeIfModified(`src/mbgl/style/layers/${layerFileName}_layer_properties.hpp`, propertiesHpp(layer));
  writeIfModified(`src/mbgl/style/layers/${layerFileName}_layer_properties.cpp`, propertiesCpp(layer));

  // Remove our fake property for the external interace.
  if (layer.type === 'line') {
    layer.paintProperties = layer.paintProperties.filter(property => property.name !== 'line-floor-width');
  }

  writeIfModified(`include/mbgl/style/layers/${layerFileName}_layer.hpp`, layerHpp(layer));
  writeIfModified(`src/mbgl/style/layers/${layerFileName}_layer.cpp`, layerCpp(layer));
}

// Light
const lightProperties = Object.keys(spec[`light`]).reduce((memo, name) => {
  var property = spec[`light`][name];
  property.name = name;
  property['light-property'] = true;
  memo.push(property);
  return memo;
}, []);

// JSON doesn't have a defined order. We're going to sort them alphabetically
// to get a deterministic order.
lightProperties.sort((a, b) => collator.compare(a.name, b.name));

const lightHpp = ejs.compile(fs.readFileSync('include/mbgl/style/light.hpp.ejs', 'utf8'), {strict: true});
const lightCpp = ejs.compile(fs.readFileSync('src/mbgl/style/light.cpp.ejs', 'utf8'), {strict: true});
writeIfModified(`include/mbgl/style/light.hpp`, lightHpp({properties: lightProperties}));
writeIfModified(`src/mbgl/style/light.cpp`, lightCpp({properties: lightProperties}));
