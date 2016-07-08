'use strict';

const fs = require('fs');
const ejs = require('ejs');
const spec = require('mapbox-gl-style-spec').latest;
const _ = require('lodash');

global.iff = function (condition, val) {
  return condition() ? val : "";
}


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

global.snakeCaseUpper = function (str) {
  return str.replace(/-/g, "_").toUpperCase();
}

global.propertyType = function propertyType(property) {
//TODO: Doe we want these exceptions?
//  if (/-translate-anchor$/.test(property.name)) {
     //    return 'TranslateAnchorType';
     //  }
     //  if (/-(rotation|pitch)-alignment$/.test(property.name)) {
     //    return 'AlignmentType';
     //  }
  switch (property.type) {
      case 'boolean':
        return 'Boolean';
      case 'number':
        return 'Float';
      case 'string':
        return 'String';
      case 'enum':
        return `String`;
      case 'color':
        return `String`;
      case 'array':
        return `${propertyType({type:property.value})}[]`;
      default:
        throw new Error(`unknown type for ${property.name}`);
  }
}

global.propertyTypeAnnotation = function propertyTypeAnnotation(property) {
  switch (property.type) {
      case 'enum':
        return `@Property.${snakeCaseUpper(property.name)}`;
      default:
        return "";
  }
};

//Process Layers
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

const layerHpp = ejs.compile(fs.readFileSync('platform/android/scripts/layer.hpp.ejs', 'utf8'), {strict: true});
const layerCpp = ejs.compile(fs.readFileSync('platform/android/scripts/layer.cpp.ejs', 'utf8'), {strict: true});
const layerJava = ejs.compile(fs.readFileSync('platform/android/scripts/layer.java.ejs', 'utf8'), {strict: true});

for (const layer of layers) {
  fs.writeFileSync(`platform/android/src/style/layers/${layer.type}_layer.hpp`, layerHpp(layer));
  fs.writeFileSync(`platform/android/src/style/layers/${layer.type}_layer.cpp`, layerCpp(layer));
  fs.writeFileSync(`platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/${camelize(layer.type)}Layer.java`, layerJava(layer));
}

//Process all layer properties
const layoutProperties = _(layers).map('layoutProperties').flatten().value();
const paintProperties = _(layers).map('paintProperties').flatten().value();

const propertiesTemplate = ejs.compile(fs.readFileSync('platform/android/scripts/layer_property_factory.java.ejs', 'utf8'), {strict: true});
fs.writeFileSync(
    `platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/PropertyFactory.java`,
    propertiesTemplate({layoutProperties: layoutProperties, paintProperties: paintProperties})
);

//Create types for the enum properties
const enumProperties = _(layoutProperties).union(paintProperties).filter({'type': 'enum'}).value();
const enumPropertyTemplate = ejs.compile(fs.readFileSync('platform/android/scripts/layer_property.java.ejs', 'utf8'), {strict: true});
fs.writeFileSync(
    `platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/Property.java`,
    enumPropertyTemplate({properties: enumProperties})
);

