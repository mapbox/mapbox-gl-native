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

global.testImplementation = function (property, layerType, isFunction) {
    let helperMsg = testHelperMessage(property, layerType, isFunction);
    return `layer.${objCName(property)} = [MGLRuntimeStylingHelper ${helperMsg}];`;
}

global.testGetterImplementation = function (property, layerType, isFunction) {
    let helperMsg = testHelperMessage(property, layerType, isFunction);
    let value = `[MGLRuntimeStylingHelper ${helperMsg}]`;
    if (property.type === 'enum') {
        if (isFunction) {
            return `XCTAssertEqualObjects(gLayer.${objCName(property)}, ${value});`;
        }
        return `XCTAssert([(NSValue *)gLayer.${objCName(property)} isEqualToValue:${value}], @"%@ is not equal to %@", gLayer.${objCName(property)}, ${value});`;
    }
    return `XCTAssertEqualObjects(gLayer.${objCName(property)}, ${value});`;
}

global.testHelperMessage = function (property, layerType, isFunction) {
    let fnSuffix = isFunction ? 'Function' : '';
    switch (property.type) {
        case 'boolean':
            return 'testBool' + fnSuffix;
        case 'number':
            return 'testNumber' + fnSuffix;
        case 'string':
            return 'testString' + fnSuffix;
        case 'enum':
            let objCType = `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`;
            let objCEnum = `${objCType}${camelize(property.values[property.values.length-1])}`;
            return `testEnum${fnSuffix}:${objCEnum} type:@encode(${objCType})`;
        case 'color':
            return 'testColor' + fnSuffix;
        case 'array':
            switch (arrayType(property)) {
                case 'dasharray':
                    return 'testDashArray' + fnSuffix;
                case 'font':
                    return 'testFont' + fnSuffix;
                case 'padding':
                    return 'testPadding' + fnSuffix;
                case 'offset':
                case 'translate':
                    return 'testOffset' + fnSuffix;
                default:
                    throw new Error(`unknown array type for ${property.name}`);
            }
        default:
            throw new Error(`unknown type for ${property.name}`);
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
            return 'an `NSNumber` object containing ' + (value ? '`YES`' : '`NO`');
        case 'number':
            return 'an `NSNumber` object containing the float `' + value + '`';
        case 'string':
            return 'the string `' + value + '`';
        case 'enum':
            let objCType = `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`;
            return 'an `NSValue` object containing `' + `${objCType}${camelize(value)}` + '`';
        case 'color':
            let color = parseColor(value);
            if (!color) {
                throw new Error(`unrecognized color format in default value of ${property.name}`);
            }
            if (color.r === 0 && color.g === 0 && color.b === 0 && color.a === 0) {
                return '`MGLColor.clearColor`';
            }
            if (color.r === 0 && color.g === 0 && color.b === 0 && color.a === 1) {
                return '`MGLColor.blackColor`';
            }
            if (color.r === 1 && color.g === 1 && color.b === 1 && color.a === 1) {
                return '`MGLColor.whiteColor`';
            }
            return 'an `MGLColor`' + ` object whose RGB value is ${color.r}, ${color.g}, ${color.b} and whose alpha value is ${color.a}`;
        case 'array':
            let units = property.units || '';
            if (units) {
                units = ` ${units}`.replace(/pixel/, 'point');
            }
            if (property.name.indexOf('padding') !== -1) {
                if (value[0] === 0 && value[1] === 0 && value[2] === 0 && value[3] === 0) {
                    return 'an `NSValue` object containing `NSEdgeInsetsZero` or `UIEdgeInsetsZero`';
                }
                return 'an `NSValue` object containing an `NSEdgeInsets` or `UIEdgeInsets` struct set to' + ` ${value[0]}${units} on the top, ${value[3]}${units} on the left, ${value[2]}${units} on the bottom, and ${value[1]}${units} on the right`;
            }
            if (property.name.indexOf('offset') !== -1 || property.name.indexOf('translate') !== -1) {
                return 'an `NSValue` object containing a `CGVector` struct set to' + ` ${value[0]}${units} from the left and ${value[1]}${units} from the top`;
            }
            return 'the array `' + value.join('`, `') + '`';
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

global.styleAttributeFactory = function (property, layerType) {
    switch (property.type) {
        case 'boolean':
            return 'mbgl_boolWithPropertyValueBool';
        case 'number':
            return 'mbgl_numberWithPropertyValueNumber';
        case 'string':
            return 'mbgl_stringWithPropertyValueString';
        case 'enum':
            throw new Error('Use MGLGetEnumProperty() for enums.');
        case 'color':
            return 'mbgl_colorWithPropertyValueColor';
        case 'array':
            return `mbgl_${convertedType(property)}WithPropertyValue${camelize(convertedType(property))}`;
        default:
            throw new Error(`unknown type for ${property.name}`);
    }
};

global.getterImplementation = function(property, layerType) {
    if (property.type === 'enum') {
        let objCType = `${prefix}${camelize(layerType)}${suffix}${camelize(property.name)}`;
        return `MGLGetEnumProperty(${camelize(property.name)}, ${mbglType(property)}, ${objCType});`;
    }
    let rawValue = `self.layer->get${camelize(property.name)}() ?: self.layer->getDefault${camelize(property.name)}()`;
    return `return [MGLStyleAttribute ${styleAttributeFactory(property, layerType)}:${rawValue}];`;
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

function duplicatePlatformDecls(src) {
    // Look for a documentation comment that contains “MGLColor” and the
    // subsequent function, method, or property declaration. Try not to match
    // greedily.
    return src.replace(/(\/\*\*(?:\*[^\/]|[^*])*?\bMGLColor\b[\s\S]*?\*\/)(\s*.+?;)/g,
                       (match, comment, decl) => {
        let iosComment = comment.replace(/\bMGLColor\b/g, 'UIColor')
            // Use the correct indefinite article.
            .replace(/\b(a)n(\s+`?UIColor)\b/gi, '$1$2');
        let macosComment = comment.replace(/\bMGLColor\b/g, 'NSColor');
        return `\
#if TARGET_OS_IPHONE
${iosComment}${decl}
#else
${macosComment}${decl}
#endif`;
    });
}

for (var layer of layers) {
    fs.writeFileSync(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}.h`, duplicatePlatformDecls(layerH(layer)));
    fs.writeFileSync(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}.mm`, layerM(layer));
    fs.writeFileSync(`platform/darwin/test/${prefix}${camelize(layer.type)}${suffix}Tests.m`, testLayers(layer));
}
