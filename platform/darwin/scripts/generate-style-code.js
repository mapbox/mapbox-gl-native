'use strict';

const fs = require('fs');
const ejs = require('ejs');
const _ = require('lodash');
const colorParser = require('csscolorparser');
const cocoaConventions = require('./style-spec-cocoa-conventions-v8.json');
let spec = _.merge(require('mapbox-gl-style-spec').latest, require('./style-spec-overrides-v8.json'));
const prefix = 'MGL';
const suffix = 'StyleLayer';

// Rename properties and keep `original` for use with setters and getters
_.forOwn(cocoaConventions, function (properties, kind) {
    _.forOwn(properties, function (newName, oldName) {
        let property = spec[kind][oldName];
        if (newName.startsWith('is-')) {
            property.getter = newName;
            newName = newName.substr(3);
        }
        if (newName !== oldName) {
            property.original = oldName;
        }
        delete spec[kind][oldName];
        spec[kind][newName] = property;
        
        // Update requirements in other properties.
        let updateRequirements = function (property, name) {
            let requires = property.requires || [];
            for (let i = 0; i < requires.length; i++) {
                if (requires[i] === oldName) {
                    property.requires[i] = newName;
                }
                if (typeof requires[i] !== 'string') {
                    let prop = name;
                    _.forOwn(requires[i], function (values, name, require) {
                        if (name === oldName) {
                            require[newName] = values;
                            delete require[name];
                        }
                    });
                }
            }
        };
        _.forOwn(spec[kind.replace(/^layout_/, 'paint_')], updateRequirements);
        _.forOwn(spec[kind.replace(/^paint_/, 'layout_')], updateRequirements);
    })
});

String.prototype.wrap = function (cols, indent) {
	let wrapRe = new RegExp(`(.{1,${cols - indent}})(?: +|\n|$)`, "gm");
	return this.replace(wrapRe, "$1\n").replace(/\s+$/, "").indent(indent);
};

String.prototype.indent = function (cols) {
	return this.replace(/^|\n/g, "$&" + " ".repeat(cols));
};

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

global.objCName = function (property) {
    return camelizeWithLeadingLowercase(property.name);
}

global.objCGetter = function (property) {
    return camelizeWithLeadingLowercase(property.getter || property.name);
}

global.objCType = function (layerType, propertyName) {
    return `${prefix}${camelize(propertyName)}`;
}

global.arrayType = function (property) {
    return property.type === 'array' ? originalPropertyName(property).split('-').pop() : false;
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
        return `XCTAssert([gLayer.${objCName(property)} isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.${objCName(property)}, ${value});`;
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
            let objCType = global.objCType(layerType, property.name);
            let objCEnum = `${objCType}${camelize(Object.keys(property.values)[Object.keys(property.values).length-1])}`;
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

global.propertyDoc = function (propertyName, property, layerType, kind) {
    // Match references to other property names & values. 
    // Requires the format 'When `foo` is set to `bar`,'.
    let doc = property.doc.replace(/`([^`]+?)` is set to `([^`]+?)`/g, function (m, peerPropertyName, propertyValue, offset, str) {
        let otherProperty = camelizeWithLeadingLowercase(peerPropertyName);
        let otherValue = objCType(layerType, peerPropertyName) + camelize(propertyValue);
        return '`' + `${otherProperty}` + '` is set to `' + `${otherValue}` + '`';
    });
    // Match references to our own property values.
    // Requires the format 'is equivalent to `bar`'.
    doc = doc.replace(/is equivalent to `(.+?)`/g, function(m, propertyValue, offset, str) {
        propertyValue = objCType(layerType, propertyName) + camelize(propertyValue);
        return 'is equivalent to `' + propertyValue + '`';
    });
    // Format everything else: our property name & its possible values.
    // Requires symbols to be surrounded by backticks.
    doc = doc.replace(/`(.+?)`/g, function (m, symbol, offset, str) {
        if ('values' in property && Object.keys(property.values).indexOf(symbol) !== -1) {
            let objCType = objCType(layerType, property.name);
            return '`' + `${objCType}${camelize(symbol)}` + '`';
        }
        if (str.substr(offset - 4, 3) !== 'CSS') {
            symbol = camelizeWithLeadingLowercase(symbol);
        }
        return '`' + symbol + '`';
    });
    // Format references to units.
    if ('units' in property) {
        if (!property.units.match(/s$/)) {
            property.units += 's';
        }
        doc += `\n\nThis property is measured in ${property.units}.`;
    }
    doc = doc.replace(/(p)ixel/gi, '$1oint').replace(/(\d)px\b/g, '$1pt');
    if (kind !== 'enum') {
        if ('default' in property) {
            doc += `\n\nThe default value of this property is ${propertyDefault(property, layerType)}.`;
            if (!property.required) {
                doc += ' Set this property to `nil` to reset it to the default value.';
            }
        }
        if ('requires' in property) {
            doc += '\n\n' + propertyReqs(property, spec[`${kind}_${layerType}`], layerType);
        }
        if ('original' in property) {
            doc += `\n\nThis attribute corresponds to the <a href="https://www.mapbox.com/mapbox-gl-style-spec/#layout-${layerType}-${property.original}"><code>${property.original}</code></a> layout property in the Mapbox Style Specification.`;
        }
    }
    return doc;
};

global.propertyReqs = function (property, propertiesByName, type) {
    return 'This property is only applied to the style if ' + property.requires.map(function (req) {
        if (typeof req === 'string') {
            return '`' + camelizeWithLeadingLowercase(req) + '` is non-`nil`';
        } else if ('!' in req) {
            return '`' + camelizeWithLeadingLowercase(req['!']) + '` is set to `nil`';
        } else {
            let name = Object.keys(req)[0];
            return '`' + camelizeWithLeadingLowercase(name) + '` is set to an `MGLStyleValue` object containing ' + describeValue(req[name], propertiesByName[name], type);
        }
    }).join(', and ') + '. Otherwise, it is ignored.';
};

global.parseColor = function (str) {
    let color = colorParser.parseCSSColor(str);
    return {
        r: color[0] / 255,
        g: color[1] / 255,
        b: color[2] / 255,
        a: color[3],
    };
};

global.describeValue = function (value, property, layerType) {
    switch (property.type) {
        case 'boolean':
            return 'an `NSNumber` object containing ' + (value ? '`YES`' : '`NO`');
        case 'number':
            return 'an `NSNumber` object containing the float `' + value + '`';
        case 'string':
            if (value === '') {
                return 'the empty string';
            }
            return 'the string `' + value + '`';
        case 'enum':
            let displayValue;
            if (Array.isArray(value)) {
              let separator = (value.length === 2) ? ' ' : ', ';
              displayValue = value.map((possibleValue, i) => {
                let conjunction = '';
                if (value.length === 2 && i === 0) conjunction = 'either ';
                if (i === value.length - 1) conjunction = 'or ';
                let objCType = global.objCType(layerType, property.name);
                return `${conjunction}\`${objCType}${camelize(possibleValue)}\``;
              }).join(separator);
            } else {
              let objCType = global.objCType(layerType, property.name);
              displayValue = `\`${objCType}${camelize(value)}\``;
            }
            return `an \`NSValue\` object containing ${displayValue}`;
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
    return 'an `MGLStyleValue` object containing ' + describeValue(property.default, property, layerType);
};

global.originalPropertyName = function (property) {
    return property.original || property.name;
}

global.propertyType = function (property) {
    switch (property.type) {
        case 'boolean':
            return 'NSNumber *';
        case 'number':
            return 'NSNumber *';
        case 'string':
            return 'NSString *';
        case 'enum':
            return 'NSValue *';
        case 'color':
            return 'MGLColor *';
        case 'array':
            switch (arrayType(property)) {
                case 'dasharray':
                    return 'NSArray<NSNumber *> *';
                case 'font':
                    return 'NSArray<NSString *> *';
                case 'padding':
                    return 'NSValue *';
                case 'offset':
                case 'translate':
                    return 'NSValue *';
                default:
                    throw new Error(`unknown array type for ${property.name}`);
            }
        default:
            throw new Error(`unknown type for ${property.name}`);
    }
};

global.valueTransformerArguments = function (property) {
    let objCType = propertyType(property);
    switch (property.type) {
        case 'boolean':
            return ['bool', objCType];
        case 'number':
            return ['float', objCType];
        case 'string':
            return ['std::string', objCType];
        case 'enum':
            return [`mbgl::style::${mbglType(property)}`, objCType];
        case 'color':
            return ['mbgl::Color', objCType];
        case 'array':
            switch (arrayType(property)) {
                case 'dasharray':
                    return ['std::vector<float>', objCType, 'float'];
                case 'font':
                    return ['std::vector<std::string>', objCType, 'std::string'];
                case 'padding':
                    return ['std::array<float, 4>', objCType];
                case 'offset':
                case 'translate':
                    return ['std::array<float, 2>', objCType];
                default:
                    throw new Error(`unknown array type for ${property.name}`);
            }
        default:
            throw new Error(`unknown type for ${property.name}`);
    }
};

global.initLayer = function (layerType) {
    if (layerType == "background") {
       return `_layer = new mbgl::style::${camelize(layerType)}Layer(identifier.UTF8String);`
    } else {
        return `_layer = new mbgl::style::${camelize(layerType)}Layer(identifier.UTF8String, source.identifier.UTF8String);`
    }
}

global.setSourceLayer = function() {
   return `_layer->setSourceLayer(sourceLayer.UTF8String);`
}

global.mbglType = function(property) {
    let mbglType = camelize(originalPropertyName(property)) + 'Type';
    if (/-translate-anchor$/.test(originalPropertyName(property))) {
        mbglType = 'TranslateAnchorType';
    }
    if (/-(rotation|pitch)-alignment$/.test(originalPropertyName(property))) {
        mbglType = 'AlignmentType';
    }
    return mbglType;
}

const layerH = ejs.compile(fs.readFileSync('platform/darwin/src/MGLStyleLayer.h.ejs', 'utf8'), { strict: true });
const layerM = ejs.compile(fs.readFileSync('platform/darwin/src/MGLStyleLayer.mm.ejs', 'utf8'), { strict: true});
const testLayers = ejs.compile(fs.readFileSync('platform/darwin/test/MGLStyleLayerTests.m.ejs', 'utf8'), { strict: true});
const categoryH = ejs.compile(fs.readFileSync('platform/darwin/src/NSValue+MGLStyleEnumAttributeAdditions.h.ejs', 'utf8'), { strict: true});
const categoryM = ejs.compile(fs.readFileSync('platform/darwin/src/NSValue+MGLStyleEnumAttributeAdditions.mm.ejs', 'utf8'), { strict: true});
const guideMD = ejs.compile(fs.readFileSync('platform/darwin/docs/guides/For Style Authors.md.ejs', 'utf8'), { strict: true });

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
        doc: spec.layer.type.values[type].doc,
        type: type,
        layoutProperties: _.sortBy(layoutProperties, ['name']),
        paintProperties: _.sortBy(paintProperties, ['name']),
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

var allLayoutProperties = [];
var allPaintProperties = [];
var allTypes = [];
var allRenamedProperties = {};

for (let layer of layers) {
    allLayoutProperties.push(layer.layoutProperties);
    allPaintProperties.push(layer.paintProperties);
    allTypes.push(layer.type);
    let properties = _.concat(layer.layoutProperties, layer.paintProperties);
    layer.containsEnumerationProperties = _.some(properties, prop => prop.type === "enum");
    
    let renamedProperties = {};
    _.assign(renamedProperties, _.filter(properties, prop => 'original' in prop || 'getter' in prop));
    if (!_.isEmpty(renamedProperties)) {
        allRenamedProperties[layer.type] = renamedProperties;
    }

    fs.writeFileSync(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}.h`, duplicatePlatformDecls(layerH(layer)));
    fs.writeFileSync(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}.mm`, layerM(layer));
    fs.writeFileSync(`platform/darwin/test/${prefix}${camelize(layer.type)}${suffix}Tests.m`, testLayers(layer));
}

fs.writeFileSync(`platform/darwin/src/NSValue+MGLStyleEnumAttributeAdditions.h`, categoryH({
    layoutProperties: _.flatten(allLayoutProperties),
    paintProperties: _.flatten(allPaintProperties),
    types: allTypes
}));
fs.writeFileSync(`platform/darwin/src/NSValue+MGLStyleEnumAttributeAdditions.mm`, categoryM({
    layoutProperties: _.flatten(allLayoutProperties),
    paintProperties: _.flatten(allPaintProperties)
}));

fs.writeFileSync(`platform/darwin/docs/guides/For Style Authors.md`, guideMD({
    renamedProperties: allRenamedProperties,
    types: allTypes,
}));
