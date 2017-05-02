'use strict';

const fs = require('fs');
const ejs = require('ejs');
const spec = require('../../../mapbox-gl-js/src/style-spec/reference/v8');
const _ = require('lodash');

require('../../../scripts/style-code');

// Specification parsing //

// Collect layer types from spec
var layers = Object.keys(spec.layer.type.values).map((type) => {
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
    doc: spec.layer.type.values[type].doc,
    layoutProperties: layoutProperties,
    paintProperties: paintProperties,
    properties: layoutProperties.concat(paintProperties)
  };
});

// Process all layer properties
const layoutProperties = _(layers).map('layoutProperties').flatten().value();
const paintProperties = _(layers).map('paintProperties').flatten().value();
const allProperties = _(layoutProperties).union(paintProperties).value();
const enumProperties = _(allProperties).filter({'type': 'enum'}).value();

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
        return snakeCaseUpper(property.name) + "_" + snakeCaseUpper(Object.keys(property.values)[0]);
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

/**
 * Produces documentation for property factory methods
 */
global.propertyFactoryMethodDoc = function (property) {
    var replaceIfPixels = function (doc) {
      return doc.replace('pixels', 'density-independent pixels')
    }
    let doc = replaceIfPixels(property.doc);
    // Match other items in back ticks
    doc = doc.replace(/`(.+?)`/g, function (m, symbol, offset, str) {
        if (str.substr(offset - 4, 3) !== 'CSS' && symbol[0].toUpperCase() != symbol[0] && _(enumProperties).filter({'name': symbol}).value().length > 0) {
            // Property 'enums'
            symbol = snakeCaseUpper(symbol);
            return '{@link Property.' + symbol + '}';
        } else if( _(allProperties).filter({'name': symbol}).value().length > 0) {
            // Other properties
            return '{@link PropertyFactory#' + camelizeWithLeadingLowercase(symbol) + '}';
        } else {
            // Left overs
            return '`' + symbol + '`';
        }
    });
    return doc;
};

/**
 * Produces documentation for property value constants
 */
global.propertyValueDoc = function (property, value) {

    // Match references to other property names & values.
    // Requires the format 'When `foo` is set to `bar`,'.
    let doc = property.values[value].doc.replace(/When `(.+?)` is set to `(.+?)`,/g, function (m, peerPropertyName, propertyValue, offset, str) {
        let otherProperty = snakeCaseUpper(peerPropertyName);
        let otherValue = snakeCaseUpper(peerPropertyName) + '_' + snakeCaseUpper(propertyValue);
        return 'When {@link ' + `${otherProperty}` + '} is set to {@link Property#' + `${otherValue}` + '},';
    });

    // Match references to our own property values.
    // Requires the format 'is equivalent to `bar`'.
    doc = doc.replace(/is equivalent to `(.+?)`/g, function(m, propertyValue, offset, str) {
        propertyValue = snakeCaseUpper(property.name) + '_' + snakeCaseUpper(propertyValue);
        return 'is equivalent to {@link Property#' + propertyValue + '}';
    });

    // Match other items in back ticks
    doc = doc.replace(/`(.+?)`/g, function (m, symbol, offset, str) {
        if ('values' in property && Object.keys(property.values).indexOf(symbol) !== -1) {
            // Property values
            propertyValue = snakeCaseUpper(property.name) + '_' + snakeCaseUpper(symbol);
            console.log("Transforming", symbol, propertyValue);
            return '{@link Property#' + `${propertyValue}` + '}';
        } else if (str.substr(offset - 4, 3) !== 'CSS' && symbol[0].toUpperCase() != symbol[0]) {
            // Property 'enums'
            symbol = snakeCaseUpper(symbol);
            return '{@link ' + symbol + '}';
        } else {
            // Left overs
            return symbol
        }
    });
    return doc;
};

global.supportsZoomFunction = function (property) {
  return property['zoom-function'] === true;
};

global.supportsPropertyFunction = function (property) {
  return property['property-function'] === true;
};

// Template processing //

// Java + JNI Layers (Peer model)
const layerHpp = ejs.compile(fs.readFileSync('platform/android/src/style/layers/layer.hpp.ejs', 'utf8'), {strict: true});
const layerCpp = ejs.compile(fs.readFileSync('platform/android/src/style/layers/layer.cpp.ejs', 'utf8'), {strict: true});
const layerJava = ejs.compile(fs.readFileSync('platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/layer.java.ejs', 'utf8'), {strict: true});
const layerJavaUnitTests = ejs.compile(fs.readFileSync('platform/android/MapboxGLAndroidSDKTestApp/src/androidTest/java/com/mapbox/mapboxsdk/testapp/style/layer.junit.ejs', 'utf8'), {strict: true});

for (const layer of layers) {
  writeIfModified(`platform/android/src/style/layers/${layer.type.replace('-', '_')}_layer.hpp`, layerHpp(layer));
  writeIfModified(`platform/android/src/style/layers/${layer.type.replace('-', '_')}_layer.cpp`, layerCpp(layer));
  writeIfModified(`platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/${camelize(layer.type)}Layer.java`, layerJava(layer));
  writeIfModified(`platform/android/MapboxGLAndroidSDKTestApp/src/androidTest/java/com/mapbox/mapboxsdk/testapp/style/${camelize(layer.type)}LayerTest.java`, layerJavaUnitTests(layer));
}


// Java PropertyFactory
const propertiesTemplate = ejs.compile(fs.readFileSync('platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/property_factory.java.ejs', 'utf8'), {strict: true});
writeIfModified(
    `platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/PropertyFactory.java`,
    propertiesTemplate({layoutProperties: layoutProperties, paintProperties: paintProperties})
);

// Java Property
const enumPropertyJavaTemplate = ejs.compile(fs.readFileSync('platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/property.java.ejs', 'utf8'), {strict: true});
writeIfModified(
    `platform/android/MapboxGLAndroidSDK/src/main/java/com/mapbox/mapboxsdk/style/layers/Property.java`,
    enumPropertyJavaTemplate({properties: enumProperties})
);

// De-duplicate enum properties before processing jni property templates
const enumPropertiesDeDup = _(enumProperties).uniqBy(global.propertyNativeType).value();

// JNI Enum property conversion templates
const enumPropertyHppTypeStringValueTemplate = ejs.compile(fs.readFileSync('platform/android/src/style/conversion/types_string_values.hpp.ejs', 'utf8'), {strict: true});
writeIfModified(
    `platform/android/src/style/conversion/types_string_values.hpp`,
    enumPropertyHppTypeStringValueTemplate({properties: enumPropertiesDeDup})
);

// JNI property value types conversion templates
const enumPropertyHppTypeTemplate = ejs.compile(fs.readFileSync('platform/android/src/style/conversion/types.hpp.ejs', 'utf8'), {strict: true});
writeIfModified(
    `platform/android/src/style/conversion/types.hpp`,
    enumPropertyHppTypeTemplate({properties: enumPropertiesDeDup})
);
