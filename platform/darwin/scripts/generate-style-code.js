'use strict';

const fs = require('fs');
const ejs = require('ejs');
const spec = require('mapbox-gl-style-spec').latest;

const prefix = 'MGL';
const suffix = 'StyleLayer';

global.camelize = function (str) {
    return str.replace(/(?:^|-)(.)/g, function (_, x) {
        return x.toUpperCase();
    });
};

global.camelizeWithLeadingLowercase = function (str) {
    return str.replace(/-(.)/g, function (_, x) {
        return x.toUpperCase();
    });
};

global.objCName = function (property) { return camelizeWithLeadingLowercase(property.name); }

global.arrayType = function (property) {
    return property.type === 'array' ? property.name.split('-').pop() : false;
};

global.testImplementation = function (property, layerType) {
    switch (property.type) {
        case 'boolean':
            return `layer.${objCName(property)} = MGLRuntimeStylingHelper.testBool;`;
        case 'number':
            return `layer.${objCName(property)} = MGLRuntimeStylingHelper.testNumber;`;
        case 'string':
            return `layer.${objCName(property)} = MGLRuntimeStylingHelper.testString;`;
        case 'enum':
            let objCType = `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`;
            let objCEnum = `${objCType}${camelize(property.values[property.values.length-1])}`;
            return `layer.${objCName(property)} = [MGLRuntimeStylingHelper testEnum:${objCEnum} type:@encode(${objCType})];`;    
        case 'color':
            return `layer.${objCName(property)} = MGLRuntimeStylingHelper.testColor;`;
        case 'array':
            return testArrayImplementation(property);
        default: throw new Error(`unknown type for ${property.name}`);
    }
}

global.testGetterImplementation = function (property, layerType) {
    switch (property.type) {
        case 'boolean':
            return `XCTAssertEqualObjects(gLayer.${objCName(property)}, MGLRuntimeStylingHelper.testBool);`;
        case 'number':
            return `XCTAssertEqualObjects(gLayer.${objCName(property)}, MGLRuntimeStylingHelper.testNumber);`;
        case 'string':
            return `XCTAssertEqualObjects(gLayer.${objCName(property)}, MGLRuntimeStylingHelper.testString);`;
        case 'enum':
            let objCType = `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`;
            let objCEnum = `${objCType}${camelize(property.values[property.values.length-1])}`;
            return `XCTAssert([(NSValue *)gLayer.${objCName(property)} objCType] == [[MGLRuntimeStylingHelper testEnum:${objCEnum} type:@encode(${objCType})] objCType]);`;
        case 'color':
            return `XCTAssertEqualObjects(gLayer.${objCName(property)}, MGLRuntimeStylingHelper.testColor);`;
        case 'array':
            return testGetterArrayImplementation(property);
        default: throw new Error(`unknown type for ${property.name}`);
    }
}

global.testGetterArrayImplementation = function (property) {
    switch (arrayType(property)) {
        case 'dasharray':
            return `XCTAssertEqualObjects(gLayer.${objCName(property)}, MGLRuntimeStylingHelper.testDashArray);`;
        case 'font':
            return `XCTAssertEqualObjects(gLayer.${objCName(property)}, MGLRuntimeStylingHelper.testFont);`;
        case 'padding':
            return `XCTAssertEqualObjects(gLayer.${objCName(property)}, MGLRuntimeStylingHelper.testPadding);`;
        case 'offset':
        case 'translate':
            return `XCTAssertEqualObjects(gLayer.${objCName(property)}, MGLRuntimeStylingHelper.testOffset);`; // Default offset (dx, dy)
        default:
            throw new Error(`unknown array type for ${property.name}`);
    }
};

global.testArrayImplementation = function (property) {
    switch (arrayType(property)) {
        case 'dasharray':
            return `layer.${objCName(property)} = MGLRuntimeStylingHelper.testDashArray;`;
        case 'font':
            return `layer.${objCName(property)} = MGLRuntimeStylingHelper.testFont;`;
        case 'padding':
            return `layer.${objCName(property)} = MGLRuntimeStylingHelper.testPadding;`;
        case 'offset':
        case 'translate':
            return `layer.${objCName(property)} = MGLRuntimeStylingHelper.testOffset;`; // Default offset (dx, dy)
        default:
            throw new Error(`unknown array type for ${property.name}`);
    }
};

global.propertyDoc = function (property, layerType) {
    let doc = property.doc.replace(/`(.+?)`/g, function (m, symbol, offset, str) {
        if ('values' in property && property.values.indexOf(symbol) !== -1) {
            let objCType = `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`;
            return '`' + `${objCType}${camelize(symbol)}` + '`';
        }
        if (str.substr(offset - 4, 3) !== 'CSS') {
            symbol = camelizeWithLeadingLowercase(symbol);
        }
        return '`' + symbol + '`';
    });
    if ('units' in property) {
        if (!property.units.match(/s$/)) {
            property.units += 's';
        }
        doc += `

 This property is measured in ${property.units}.`;
    }
    return doc.replace(/(p)ixel/gi, '$1oint').replace(/(\d)px\b/g, '$1pt');
};

global.propertyReqs = function (property, layoutPropertiesByName, type) {
    return 'This property is only applied to the style if ' + property.requires.map(function (req) {
        if (typeof req === 'string') {
            return '`' + camelizeWithLeadingLowercase(req) + '` is non-`nil`';
        } else if ('!' in req) {
            return '`' + camelizeWithLeadingLowercase(req['!']) + '` is set to `nil`';
        } else {
            let name = Object.keys(req)[0];
            return '`' + camelizeWithLeadingLowercase(name) + '` is set to ' + describeValue(req[Object.keys(req)[0]], layoutPropertiesByName[name], type);
        }
    }).join(', and ') + '. Otherwise, it is ignored.';
};

global.parseColor = function (str) {
    let m = str.match(/^#(\d\d)(\d\d)(\d\d)$/);
    if (m) {
        return {
            r: parseInt(m[1], 16) / 255,
            g: parseInt(m[2], 16) / 255,
            b: parseInt(m[3], 16) / 255,
            a: 1.0,
        };
    }
    
    m = str.match(/^rgba\(\s*([-\d.]+),\s*([-\d.]+),\s*([-\d.]+),\s*([\d.]+)\s*\)$/);
    if (m) {
        return {
            r: parseFloat(m[1]) / 255,
            g: parseFloat(m[2]) / 255,
            b: parseFloat(m[3]) / 255,
            a: parseFloat(m[4]),
        };
    }
};

global.describeValue = function (value, property, layerType) {
    switch (property.type) {
        case 'boolean':
            return value ? '`YES`' : '`NO`';
        case 'number':
        case 'string':
            return '`' + value + '`';
        case 'enum':
            let objCType = `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`;
            return '`' + `${objCType}${camelize(value)}` + '`';
        case 'color':
            let color = parseColor(value);
            if (!color) {
                throw new Error(`unrecognized color format in default value of ${property.name}`);
            }
            if (color.r === 0 && color.g === 0 && color.b === 0 && color.a === 0) {
                return '`clearColor`';
            }
            if (color.r === 0 && color.g === 0 && color.b === 0 && color.a === 1) {
                return '`blackColor`';
            }
            if (color.r === 1 && color.g === 1 && color.b === 1 && color.a === 1) {
                return '`whiteColor`';
            }
            return 'an `NSColor` or `UIColor`' + ` object whose RGB value is ${color.r}, ${color.g}, ${color.b} and whose alpha value is ${color.a}`;
        case 'array':
            let units = property.units || '';
            if (units) {
                units = ` ${units}`.replace(/pixel/, 'point');
            }
            if (property.name.indexOf('padding') !== -1) {
                if (value[0] === 0 && value[1] === 0 && value[2] === 0 && value[3] === 0) {
                    return '`NSEdgeInsetsZero` or `UIEdgeInsetsZero`';
                }
                return `${value[0]}${units} on the top, ${value[3]}${units} on the left, ${value[2]}${units} on the bottom, and ${value[1]}${units} on the right`;
            }
            if (property.name.indexOf('offset') !== -1 || property.name.indexOf('translate') !== -1) {
                return `${value[0]}${units} from the left and ${value[1]}${units} from the top`;
            }
            return '`' + value.join('`, `') + '`';
        default:
            throw new Error(`unknown type for ${property.name}`);
    }
};

global.propertyDefault = function (property, layerType) {
    return describeValue(property.default, property, layerType);
};

global.propertyType = function (property, _private) {
    return _private ? `id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>` : `id <MGLStyleAttributeValue>`;
};

global.initLayer = function (layerType) {
    if (layerType == "background") {
        return `_layer = new mbgl::style::${camelize(layerType)}Layer(layerIdentifier.UTF8String);`
    } else {
        return `_layer = new mbgl::style::${camelize(layerType)}Layer(layerIdentifier.UTF8String, sourceIdentifier.UTF8String);`
    }
}

global.setterImplementation = function(property, layerType) {
    let implementation = '';
    switch (property.type) {
        case 'boolean':
            implementation = `self.layer->set${camelize(property.name)}(${objCName(property)}.mbgl_boolPropertyValue);`;
            break;
        case 'number':
            implementation = `self.layer->set${camelize(property.name)}(${objCName(property)}.mbgl_floatPropertyValue);`;
            break;
        case 'string':
            implementation = `self.layer->set${camelize(property.name)}(${objCName(property)}.mbgl_stringPropertyValue);`;
            break;
        case 'enum':
            let objCType = `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`;
            implementation = `MGLSetEnumProperty(${objCName(property)}, ${camelize(property.name)}, ${mbglType(property)}, ${objCType});`;
            break;
        case 'color':
            implementation = `self.layer->set${camelize(property.name)}(${objCName(property)}.mbgl_colorPropertyValue);`;
            break;
        case 'array':
            implementation = arraySetterImplementation(property);
            break;
        default: throw new Error(`unknown type for ${property.name}`)
    }
    implementation += "\n    [self update];"
    return implementation;
}

global.mbglType = function(property) {
    let mbglType = camelize(property.name) + 'Type';
    if (/-translate-anchor$/.test(property.name)) {
        mbglType = 'TranslateAnchorType';
    }
    if (/-(rotation|pitch)-alignment$/.test(property.name)) {
        mbglType = 'AlignmentType';
    }
    return mbglType;
}

global.arraySetterImplementation = function(property) {
    return `self.layer->set${camelize(property.name)}(${objCName(property)}.mbgl_${convertedType(property)}PropertyValue);`;
}

global.getterImplementation = function(property, layerType) {
    switch (property.type) {
        case 'boolean':
            return `return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->get${camelize(property.name)}()];`
        case 'number':
            return `return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->get${camelize(property.name)}()];`
        case 'string':
            return `return [MGLStyleAttribute mbgl_stringWithPropertyValueString:self.layer->get${camelize(property.name)}()];`
        case 'enum':
            let objCType = `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`;
            return `MGLGetEnumProperty(${camelize(property.name)}, ${mbglType(property)}, ${objCType});`;
        case 'color':
            return `return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->get${camelize(property.name)}()];`
        case 'array':
            return arrayGetterImplementation(property);
        default:
            throw new Error(`unknown type for ${property.name}`)
    }
}

global.arrayGetterImplementation = function(property) {
    return `return [MGLStyleAttribute mbgl_${convertedType(property)}WithPropertyValue${camelize(convertedType(property))}:self.layer->get${camelize(property.name)}()];`
}

global.convertedType = function(property) {
    switch (arrayType(property)) {
        case 'dasharray':
            return 'numberArray';
        case 'font':
            return 'stringArray';
        case 'padding':
            return 'padding';
        case 'offset':
        case 'translate':
            return 'offset';
        default:
            throw new Error(`unknown array type for ${property.name}`);
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
        layoutPropertiesByName: spec[`layout_${type}`],
        paintPropertiesByName: spec[`paint_${type}`],
    };
});

for (const layer of layers) {
    fs.writeFileSync(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}.h`, layerH(layer));
    fs.writeFileSync(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}.mm`, layerM(layer));
    fs.writeFileSync(`platform/darwin/test/${prefix}${camelize(layer.type)}${suffix}Tests.m`, testLayers(layer));
}
