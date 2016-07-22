'use strict';

const fs = require('fs');
const ejs = require('ejs');
const spec = require('mapbox-gl-style-spec').latest;

var prefix = 'MGL';
var suffix = 'StyleLayer';

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
    switch (property.type) {
        case 'string':
            return true;
        case 'color':
            return true;
        case 'array':
            return true;
        default:
            return false;
    }
}

global.propertyType = function (property, layerType = null) {
    switch (property.type) {
        case 'boolean':
            return 'BOOL';
        case 'number':
            return 'CGFloat';
        case 'string':
            return 'NSString';
        case 'enum':
            return `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`
        case 'color':
            return `MGLColor`;
        case 'array':
            return 'NSArray';
        default: throw new Error(`unknown type for ${property.name}`)
    }
}

global.setterImplementation = function(property, layerType = null) {
    switch (property.type) {
        case 'boolean':
            return `${layerType}Layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)});`;
        case 'number':
            return `${layerType}Layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)});`;
        case 'string':
            return `${layerType}Layer->set${camelize(property.name)}(std::string(${camelizeWithLeadingLowercase(property.name)}.UTF8String));`;
        case 'enum':
            return `// TODO: setterEnum`; 
        case 'color':
            return `${layerType}Layer->set${camelize(property.name)}(${camelizeWithLeadingLowercase(property.name)}.mbgl_color);`;
        case 'array':
            return '// TODO: setterArray';
        default: throw new Error(`unknown type for ${property.name}`)
    }
}

global.getterImplementation = function(property, layerType = null) {
    switch (property.type) {
        case 'boolean':
            return `return ${layerType}Layer->get${camelize(property.name)}().asConstant();`;
        case 'number':
            return `return ${layerType}Layer->get${camelize(property.name)}().asConstant();`;
        case 'string':
            return `return @(std::string(${layerType}Layer->get${camelize(property.name)}().asConstant()).c_str());`
        case 'enum':
            return `return ${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}${camelize(property.values[0])};`
        case 'color':
            return `return [MGLColor mbgl_color:${camelizeWithLeadingLowercase(layerType)}Layer->get${camelize(property.name)}().asConstant()];`;
        case 'array':
            return 'return @[]; // TODO: getterArray';
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