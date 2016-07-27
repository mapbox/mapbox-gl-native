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

global.isObject = function (property) {
    return false;
    switch (property.type) {
        case 'string':
            return true;
        case 'color':
            return true;
        case 'array':
            return true;
        case 'number':
            return true;
        case 'boolean':
            return true;
        case 'enum':
            return true;
        default:
            return false;
    }
}

global.propertyType = function (property, layerType = null) {
    return `id <MGLStyleAttributeValue>`;
    switch (property.type) {
        case 'boolean':
            //return `id <MGLStyleAttributeValue>`;
            return 'MGLNumberStyleAttributeValue';
        case 'number':
            return 'MGLNumberStyleAttributeValue';
        case 'string':
            return 'MGLStringStyleAttributeValue';
        case 'enum':
            //return `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`
            return 'MGLNumberStyleAttributeValue';
        case 'color':
            return `${colorType}`;
        case 'array':
            return `MGLNumberStyleAttributeValue`;
        default: throw new Error(`unknown type for ${property.name}`)
    }
}

global.setterImplementation = function(property, layerType = null) {
    switch (property.type) {
        case 'boolean':
            return `${layerType}Layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.numberValue.mbgl_booleanPropertyValue);`;
        case 'number':
            return `${layerType}Layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.numberValue.mbgl_numberPropertyValue);`;
        case 'string':
            return `${layerType}Layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.stringValue.mbgl_stringPropertyValue);`;
        case 'enum':
            return `// TODO: setterEnum`; 
        case 'color':
            return `${layerType}Layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.colorValue.mbgl_propertyValue);`;
        case 'array':
            return '// TODO: setterArray';
        default: throw new Error(`unknown type for ${property.name}`)
    }
}

global.getterImplementation = function(property, layerType = null) {
    switch (property.type) {
        case 'boolean':
            return `return [NSNumber mbgl_booleanWithPropertyValue:${layerType}Layer->get${camelize(property.name)}()];`;
        case 'number':
            return `return [NSNumber mbgl_numberWithPropertyValue:${layerType}Layer->get${camelize(property.name)}()];`;
        case 'string':
            return `return [NSString mbgl_stringWithPropertyValue:${layerType}Layer->get${camelize(property.name)}()];`;
        case 'enum':
            return `auto rawValue = ${layerType}Layer->get${camelize(property.name)}();
    const char *type = @encode(${prefix}${camelize(layerType)}${suffix}${camelize(property.name)});
    return [NSValue value:&rawValue withObjCType:type];`
        case 'color':
            return `return [MGLColor mbgl_propertyValue:${camelizeWithLeadingLowercase(layerType)}Layer->get${camelize(property.name)}()];`
        case 'array':
            //return `return [NSArray mbgl_arrayWithPropertyValue:${layerType}Layer->get${camelize(property.name)}()];`;
            return `return @[];`;
        default:
         throw new Error(`unknown type for ${property.name}`)
    }
}

const layerH = ejs.compile(fs.readFileSync('platform/darwin/src/MGLStyleLayer.h.ejs', 'utf8'), { strict: true });
const layerM = ejs.compile(fs.readFileSync('platform/darwin/src/MGLStyleLayer.mm.ejs', 'utf8'), { strict: true});

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
}