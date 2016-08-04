'use strict';

const fs = require('fs');
const ejs = require('ejs');
const spec = require('mapbox-gl-style-spec').latest;

var prefix = 'MGL';
var suffix = 'StyleLayer';

var colorType = 'MGLStyleAttributeColor';

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

global.testImplementation = function (property) {
    switch (property.type) {
        case 'boolean':
            return `layer.${camelizeWithLeadingLowercase(property.name)} = MGLRuntimeStylingHelper.testBool;`;
        case 'number':
            return `layer.${camelizeWithLeadingLowercase(property.name)} = MGLRuntimeStylingHelper.testNumber;`;
        case 'string':
            return `layer.${camelizeWithLeadingLowercase(property.name)} = MGLRuntimeStylingHelper.testString;`;
        case 'enum':
            return `#warning Missing setter for ${camelizeWithLeadingLowercase(property.name)} enum`;
        case 'color':
            return `layer.${camelizeWithLeadingLowercase(property.name)} = MGLRuntimeStylingHelper.testColor;`;
        case 'array':
            return testArrayImplementation(property);
        default: throw new Error(`unknown type for ${property.name}`)
    }
}

global.testGetterImplementation = function (property) {
    switch (property.type) {
        case 'boolean':
            return `XCTAssertEqualObjects(gLayer.${camelizeWithLeadingLowercase(property.name)}, MGLRuntimeStylingHelper.testBool);`;
        case 'number':
            return `XCTAssertEqualObjects(gLayer.${camelizeWithLeadingLowercase(property.name)}, MGLRuntimeStylingHelper.testNumber);`;
        case 'string':
            return `XCTAssertEqualObjects(gLayer.${camelizeWithLeadingLowercase(property.name)}, MGLRuntimeStylingHelper.testString);`;
        case 'enum':
            return `#warning Missing getter for ${camelizeWithLeadingLowercase(property.name)} enum`;
        case 'color':
            return `XCTAssertEqualObjects(gLayer.${camelizeWithLeadingLowercase(property.name)}, MGLRuntimeStylingHelper.testColor);`;
        case 'array':
            return testGetterArrayImplementation(property);
        default: throw new Error(`unknown type for ${property.name}`)
    }
}

global.testGetterArrayImplementation = function (property) {
    switch (property.name) {
        case 'icon-text-fit-padding':
            return `XCTAssertEqualObjects(gLayer.${camelizeWithLeadingLowercase(property.name)}, MGLRuntimeStylingHelper.testPadding);`;
        case 'line-dasharray':
            return `XCTAssertEqualObjects(gLayer.${camelizeWithLeadingLowercase(property.name)}, MGLRuntimeStylingHelper.testDashArray);`;
        case 'text-font':
            return `XCTAssertEqualObjects(gLayer.${camelizeWithLeadingLowercase(property.name)}, MGLRuntimeStylingHelper.testFont);`;
        default:
            return `XCTAssertEqualObjects(gLayer.${camelizeWithLeadingLowercase(property.name)}, MGLRuntimeStylingHelper.testOffset);`; // Default offset (dx, dy)
    }
}

global.testArrayImplementation = function (property) {
    switch (property.name) {
        case 'icon-text-fit-padding':
            return `layer.${camelizeWithLeadingLowercase(property.name)} = MGLRuntimeStylingHelper.testPadding;`;
        case 'line-dasharray':
            return `layer.${camelizeWithLeadingLowercase(property.name)} = MGLRuntimeStylingHelper.testDashArray;`;
        case 'text-font':
            return `layer.${camelizeWithLeadingLowercase(property.name)} = MGLRuntimeStylingHelper.testFont;`;
        default:
            return `layer.${camelizeWithLeadingLowercase(property.name)} = MGLRuntimeStylingHelper.testOffset;`; // Default offset (dx, dy)
    }
}

global.propertyType = function (property, _private) {
    return _private ? `id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>` : `id <MGLStyleAttributeValue>`;
}

global.initLayer = function (layerType) {
    if (layerType == "background") {
        return `_layer = new mbgl::style::${camelize(layerType)}Layer(layerIdentifier.UTF8String);`
    } else {
        return `_layer = new mbgl::style::${camelize(layerType)}Layer(layerIdentifier.UTF8String, sourceIdentifier.UTF8String);`
    }
}

global.setterImplementation = function(property, layerType) {
    switch (property.type) {
        case 'boolean':
            return `self.layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.mbgl_boolPropertyValue);`;
        case 'number':
            return `self.layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.mbgl_floatPropertyValue);`;
        case 'string':
            return `self.layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.mbgl_stringPropertyValue);`;
        case 'enum':
            var mbglType = camelize(property.name) + 'Type';
            if (/-translate-anchor$/.test(property.name)) {
                mbglType = 'TranslateAnchorType';
            }
            if (/-(rotation|pitch)-alignment$/.test(property.name)) {
                mbglType = 'AlignmentType';
            }
            var objCType = `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`;
            return `MGLSetEnumProperty(${camelizeWithLeadingLowercase(property.name)}, ${camelize(property.name)}, ${mbglType}, ${objCType});`; 
        case 'color':
            return `self.layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.mbgl_colorPropertyValue);`;
        case 'array':
            return arraySetterImplementation(property);
        default: throw new Error(`unknown type for ${property.name}`)
    }
}

global.arraySetterImplementation = function(property) {
    switch (property.name) {
        case 'icon-text-fit-padding':
            return `self.layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.mbgl_paddingPropertyValue);`;
        case 'line-dasharray':
            return `self.layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.mbgl_numberArrayPropertyValue);`;
        case 'text-font':
            return `self.layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.mbgl_stringArrayPropertyValue);`;
        default:
            return `self.layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.mbgl_offsetPropertyValue);`;
    }
}

global.getterImplementation = function(property, layerType) {
    switch (property.type) {
        case 'boolean':
            return `return [MGLStyleAttribute mbgl_boolPropertyValueWith:self.layer->get${camelize(property.name)}()];`
        case 'number':
            return `return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->get${camelize(property.name)}()];`
        case 'string':
            return `return [MGLStyleAttribute mbgl_stringPropertyValueWith:self.layer->get${camelize(property.name)}()];`
        case 'enum':
            return `auto rawValue = self.layer->get${camelize(property.name)}();
    const char *type = @encode(${prefix}${camelize(layerType)}${suffix}${camelize(property.name)});
    return [NSValue value:&rawValue withObjCType:type];`
        case 'color':
            return `return [MGLStyleAttribute mbgl_colorPropertyValueWith:self.layer->get${camelize(property.name)}()];`
        case 'array':
            return arrayGetterImplementation(property);
        default:
         throw new Error(`unknown type for ${property.name}`)
    }
}

global.arrayGetterImplementation = function(property) {
    switch (property.name) {
        case 'icon-text-fit-padding':
            return `return [MGLStyleAttribute mbgl_paddingPropertyValueWith:self.layer->get${camelize(property.name)}()];`
        case 'line-dasharray':
            return `return [NSArray mbgl_numberArrayPropertyValue:self.layer->get${camelize(property.name)}()];`;
        case 'text-font':
            return `return [MGLStyleAttribute mbgl_stringArrayPropertyValueWith:self.layer->get${camelize(property.name)}()];`
        default:
            return `return [MGLStyleAttribute mbgl_offsetPropertyValueWith:self.layer->get${camelize(property.name)}()];`
    }
}

const layerH = ejs.compile(fs.readFileSync('platform/darwin/src/MGLStyleLayer.h.ejs', 'utf8'), { strict: true });
const layerM = ejs.compile(fs.readFileSync('platform/darwin/src/MGLStyleLayer.mm.ejs', 'utf8'), { strict: true});
const testLayers = ejs.compile(fs.readFileSync('platform/darwin/src/MGLRuntimeStylingTests.m.ejs', 'utf8'), { strict: true});

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
    fs.writeFileSync(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}.h`, layerH(layer));
    fs.writeFileSync(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}.mm`, layerM(layer));
    fs.writeFileSync(`platform/darwin/test/${prefix}${camelize(layer.type)}${suffix}Tests.m`, testLayers(layer));
}
