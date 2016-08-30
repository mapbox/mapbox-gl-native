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

global.snakeCaseUpper = function snakeCaseUpper(str) {
  return str.replace(/-/g, "_").toUpperCase();
}

global.propertyType = function propertyType(property) {
  switch (property.type) {
      case 'boolean':
        return 'Boolean';
      case 'number':
        return 'Float';
      case 'string':
        return 'String';
      case 'enum':
        return 'String';
      case 'color':
        return 'String';
      case 'array':
        return `${propertyType({type:property.value})}[]`;
      default:
        throw new Error(`unknown type for ${property.name}`);
  }
}

global.propertyJNIType = function propertyJNIType(property) {
  switch (property.type) {
      case 'boolean':
        return 'jboolean';
      case 'jfloat':
        return 'Float';
      case 'String':
        return 'String';
      case 'enum':
        return 'String';
      case 'color':
        return 'String';
      case 'array':
        return `jarray<${propertyType({type:property.value})}[]>`;
      default:
        return 'jobject*';
  }
}

global.propertyNativeType = function (property) {
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

global.propertyTypeAnnotation = function propertyTypeAnnotation(property) {
  switch (property.type) {
      case 'enum':
        return `@Property.${snakeCaseUpper(property.name)}`;
      default:
        return "";
  }
};

global.defaultValueJava = function(property) {
    if(property.name.endsWith("-pattern")) {
        return '"pedestrian-polygon"';
    }
    if(property.name.endsWith("-font")) {
        return 'new String[]{"Open Sans Regular", "Arial Unicode MS Regular"}';
    }
     switch (property.type) {
      case 'boolean':
        return 'true';
      case 'number':
        return '0.3f';
      case 'string':
        return '"' + property['default'] + '"';
      case 'enum':
        return snakeCaseUpper(property.name) + "_" + snakeCaseUpper(property.values[0]);
      case 'color':
        return '"rgba(0, 0, 0, 1)"';
      case 'array':
             switch (property.value) {
              case 'string':
                return '[' + property['default'] + "]";
              case 'number':
                var result ='new Float[]{';
                for (var i = 0; i < property.length; i++) {
                    result += "0f";
                    if (i +1 != property.length) {
                        result += ",";
                    }
                }
                return result + "}";
             }
      default: throw new Error(`unknown type for ${property.name}`)
      }
}

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
    properties: layoutProperties.concat(paintProperties)
  };
});

const layerHpp = ejs.compile(fs.readFileSync('platform/android/src/style/layers/layer.hpp.ejs', 'utf8'), {strict: true});
const layerCpp = ejs.compile(fs.readFileSync('platform/android/src/style/layers/layer.cpp.ejs', 'utf8'), {strict: true});
const layerJava = ejs.compile(fs.readFileSync('platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/layer.java.ejs', 'utf8'), {strict: true});
const layerJavaUnitTests = ejs.compile(fs.readFileSync('platform/android/MapboxGLAndroidSDKTestApp/src/androidTest/java/com/mapbox/mapboxsdk/style/layer.junit.ejs', 'utf8'), {strict: true});

for (const layer of layers) {
  fs.writeFileSync(`platform/android/src/style/layers/${layer.type}_layer.hpp`, layerHpp(layer));
  fs.writeFileSync(`platform/android/src/style/layers/${layer.type}_layer.cpp`, layerCpp(layer));
  fs.writeFileSync(`platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/${camelize(layer.type)}Layer.java`, layerJava(layer));
  fs.writeFileSync(`platform/android/MapboxGLAndroidSDKTestApp/src/androidTest/java/com/mapbox/mapboxsdk/style/${camelize(layer.type)}LayerTest.java`, layerJavaUnitTests(layer));
}

//Process all layer properties
const layoutProperties = _(layers).map('layoutProperties').flatten().value();
const paintProperties = _(layers).map('paintProperties').flatten().value();

const propertiesTemplate = ejs.compile(fs.readFileSync('platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/property_factory.java.ejs', 'utf8'), {strict: true});
fs.writeFileSync(
    `platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/PropertyFactory.java`,
    propertiesTemplate({layoutProperties: layoutProperties, paintProperties: paintProperties})
);

//Create types for the enum properties
const enumProperties = _(layoutProperties).union(paintProperties).filter({'type': 'enum'}).value();
const enumPropertyJavaTemplate = ejs.compile(fs.readFileSync('platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/property.java.ejs', 'utf8'), {strict: true});
fs.writeFileSync(
    `platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/Property.java`,
    enumPropertyJavaTemplate({properties: enumProperties})
);

//De-dup types before generating cpp headers
const enumPropertiesDeDup = _(enumProperties).uniq(global.propertyNativeType).value();
const enumPropertyHppTypeStringValueTemplate = ejs.compile(fs.readFileSync('platform/android/src/style/conversion/types_string_values.hpp.ejs', 'utf8'), {strict: true});
fs.writeFileSync(
    `platform/android/src/style/conversion/types_string_values.hpp`,
    enumPropertyHppTypeStringValueTemplate({properties: enumPropertiesDeDup})
);

const enumPropertyHppTypeTemplate = ejs.compile(fs.readFileSync('platform/android/src/style/conversion/types.hpp.ejs', 'utf8'), {strict: true});
fs.writeFileSync(
    `platform/android/src/style/conversion/types.hpp`,
    enumPropertyHppTypeTemplate({properties: enumPropertiesDeDup})
);
