'use strict';

const fs = require('fs');
const ejs = require('ejs');
const spec = require('../mapbox-gl-js/js/style-spec/reference/latest');
const colorParser = require('csscolorparser');

require('./style-code');

function parseCSSColor(str) {
  const color = colorParser.parseCSSColor(str);
  return [
      color[0] / 255 * color[3], color[1] / 255 * color[3], color[2] / 255 * color[3], color[3]
  ];
}

global.isDataDriven = function (property) {
  return property['property-function'] === true;
};

global.evaluatedType = function (property) {
  if (/-translate-anchor$/.test(property.name)) {
    return 'TranslateAnchorType';
  }
  if (/-(rotation|pitch|illumination)-alignment$/.test(property.name)) {
    return 'AlignmentType';
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
      return `std::array<${evaluatedType({type: property.value})}, ${property.length}>`;
    } else {
      return `std::vector<${evaluatedType({type: property.value})}>`;
    }
  default: throw new Error(`unknown type for ${property.name}`)
  }
};

function attributeType(property, type) {
    const name = property.name.replace(type + '-', '').replace('-', '_');
    return `attributes::a_${name}${name === 'offset' ? '<1>' : ''}`;
}

global.layoutPropertyType = function (property) {
  if (isDataDriven(property)) {
    return `DataDrivenLayoutProperty<${evaluatedType(property)}>`;
  } else {
    return `LayoutProperty<${evaluatedType(property)}>`;
  }
};

global.paintPropertyType = function (property, type) {
  if (isDataDriven(property)) {
    return `DataDrivenPaintProperty<${evaluatedType(property)}, ${attributeType(property, type)}>`;
  } else if (/-pattern$/.test(property.name) || property.name === 'line-dasharray') {
    return `CrossFadedPaintProperty<${evaluatedType(property)}>`;
  } else {
    return `PaintProperty<${evaluatedType(property)}>`;
  }
};

global.propertyValueType = function (property) {
  if (isDataDriven(property)) {
    return `DataDrivenPropertyValue<${evaluatedType(property)}>`;
  } else {
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

  switch (property.type) {
  case 'number':
    return property.default;
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

const layers = Object.keys(spec.layer.type.values).map((type) => {
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

  writeIfModified(`include/mbgl/style/layers/${layerFileName}_layer.hpp`, layerHpp(layer));
  writeIfModified(`src/mbgl/style/layers/${layerFileName}_layer.cpp`, layerCpp(layer));

  writeIfModified(`src/mbgl/style/layers/${layerFileName}_layer_properties.hpp`, propertiesHpp(layer));
  writeIfModified(`src/mbgl/style/layers/${layerFileName}_layer_properties.cpp`, propertiesCpp(layer));
}

const propertySettersHpp = ejs.compile(fs.readFileSync('include/mbgl/style/conversion/make_property_setters.hpp.ejs', 'utf8'), {strict: true});
writeIfModified('include/mbgl/style/conversion/make_property_setters.hpp', propertySettersHpp({layers: layers}));
