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
  if (/-(rotation|pitch)-alignment$/.test(property.name)) {
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
      return `std::array<${propertyType({type: property.value})}, ${property.length}>`;
    } else {
      return `std::vector<${propertyType({type: property.value})}>`;
    }
  default: throw new Error(`unknown type for ${property.name}`)
  }
}

global.defaultValue = function (property) {
  // https://github.com/mapbox/mapbox-gl-native/issues/5258
  if (property.name === 'line-round-limit') {
    return 1;
  }

  switch (property.type) {
  case 'number':
    return property.default;
  case 'string':
    return JSON.stringify(property.default || "");
  case 'enum':
    if (property.default === undefined) {
      return `${propertyType(property)}::Undefined`;
    } else {
      return `${propertyType(property)}::${camelize(property.default)}`;
    }
  case 'color':
    return `{ ${parseCSSColor(property.default).join(', ')} }`
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

const layerHpp = ejs.compile(fs.readFileSync('include/mbgl/style/layers/layer.hpp.ejs', 'utf8'), {strict: true});
const layerCpp = ejs.compile(fs.readFileSync('src/mbgl/style/layers/layer.cpp.ejs', 'utf8'), {strict: true});
const propertiesHpp = ejs.compile(fs.readFileSync('src/mbgl/style/layers/layer_properties.hpp.ejs', 'utf8'), {strict: true});
const propertiesCpp = ejs.compile(fs.readFileSync('src/mbgl/style/layers/layer_properties.cpp.ejs', 'utf8'), {strict: true});

const layers = spec.layer.type.values.map((type) => {
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
  };
});

for (const layer of layers) {
  fs.writeFileSync(`include/mbgl/style/layers/${layer.type}_layer.hpp`, layerHpp(layer));
  fs.writeFileSync(`src/mbgl/style/layers/${layer.type}_layer.cpp`, layerCpp(layer));

  fs.writeFileSync(`src/mbgl/style/layers/${layer.type}_layer_properties.hpp`, propertiesHpp(layer));
  fs.writeFileSync(`src/mbgl/style/layers/${layer.type}_layer_properties.cpp`, propertiesCpp(layer));
}

const nodeStyleCpp = ejs.compile(fs.readFileSync('platform/node/src/node_style_properties.hpp.ejs', 'utf8'), {strict: true});
fs.writeFileSync('platform/node/src/node_style_properties.hpp', nodeStyleCpp({layers: layers}));
