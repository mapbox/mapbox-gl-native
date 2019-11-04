#!/usr/bin/env node
'use strict';

const fs = require('fs');
const ejs = require('ejs');
const _ = require('lodash');
const colorParser = require('csscolorparser');
const assert = require('assert');

require('../../../scripts/style-code');

const cocoaConventions = require('./style-spec-cocoa-conventions-v8.json');
const prefix = 'MGL';
const suffix = 'StyleLayer';

let spec = _.merge(require('../../../scripts/style-spec'), require('./style-spec-overrides-v8.json'));

// FIXME: https://github.com/mapbox/mapbox-gl-native/issues/15008
delete spec.layout_circle["circle-sort-key"]
delete spec.layout_line["line-sort-key"]
delete spec.layout_fill["fill-sort-key"]

class ConventionOverride {
    constructor(val) {
        if (typeof val === 'string') {
            this.name_ = val;
            this.enumName_ = null;
        } else if (val instanceof Object) {
            this.name_ = val.name;
            this.enumName_ = val.enumName;
        } else {
            assert(false);
        }
    }

    set name(name_) { this.name_ = name_; }
    get name() { return this.name_; }
    get enumName() { return this.enumName_ || this.name_; }
}

// Rename properties and keep `original` for use with setters and getters
_.forOwn(cocoaConventions, function (properties, kind) {
    _.forOwn(properties, function (newConvention, oldName) {
        let conventionOverride = new ConventionOverride(newConvention);
        let property = spec[kind][oldName];
        if (conventionOverride.name.startsWith('is-')) {
            property.getter = conventionOverride.name;
            conventionOverride.name = conventionOverride.name.substr(3);
        }

        // Override enum name based on style-spec-cocoa-conventions-v8.json
        property.enumName = conventionOverride.enumName;

        if (conventionOverride.name !== oldName) {
            property.original = oldName;
            delete spec[kind][oldName];
            spec[kind][conventionOverride.name] = property;
        }

        // Update cross-references to this property in other properties'
        // documentation and requirements.
        let renameCrossReferences = function (property, name) {
            property.doc = property.doc.replace(new RegExp('`' + oldName + '`', 'g'), '`' + conventionOverride.name + '`');
            let requires = property.requires || [];
            for (let i = 0; i < requires.length; i++) {
                if (requires[i] === oldName) {
                    property.requires[i] = conventionOverride.name;
                }
                if (typeof requires[i] !== 'string') {
                    _.forOwn(requires[i], function (values, name, require) {
                        if (name === oldName) {
                            require[conventionOverride.name] = values;
                            delete require[name];
                        }
                    });
                }
            }
        };
        _.forOwn(spec[kind.replace(/^layout_/, 'paint_')], renameCrossReferences);
        _.forOwn(spec[kind.replace(/^paint_/, 'layout_')], renameCrossReferences);
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

// Returns true only if property is an enum or if it is an array
// property with uniquely defined enum.
global.definesEnum = function(property, allProperties) {
    if (property.type === "enum") {
        return true;
    }

    if (property.type === 'array' && property.value === 'enum') {
        const uniqueArrayEnum = (prop, enums) => {
            if (prop.value !== 'enum') return false;
            const enumsEqual = (val1, val2) => val1.length === val1.length && val1.every((val, i) => val === val2[i]);
            return enums.filter(e => enumsEqual(Object.keys(prop.values).sort(), Object.keys(e.values).sort())).length == 0;
        };

        const allEnumProperties = _(allProperties).filter({'type': 'enum'}).value();
        const uniqueArrayEnumProperties = _(allProperties).filter({'type': 'array'}).filter(prop => uniqueArrayEnum(prop, allEnumProperties)).value();
        return _(uniqueArrayEnumProperties).filter({'name': property.name}).value().length != 0;
    }

    return false;
}

global.objCName = function (property) {
    return camelizeWithLeadingLowercase(property.name);
};

global.objCGetter = function (property) {
    return camelizeWithLeadingLowercase(property.getter || property.name);
};

global.objCType = function (layerType, propertyName) {
    return `${prefix}${camelize(propertyName)}`;
};

global.arrayType = function (property) {
    return property.type === 'array' ? originalPropertyName(property).split('-').pop() : false;
};

global.testImplementation = function (property, layerType, isFunction) {
    let helperMsg = testHelperMessage(property, layerType, isFunction);
    return `layer.${objCName(property)} = [MGLRuntimeStylingHelper ${helperMsg}];`;
};

global.objCTestValue = function (property, layerType, arraysAsStructs, indent) {
    let propertyName = originalPropertyName(property);
    switch (property.type) {
        case 'boolean':
            return property.default ? '@"false"' : '@"true"';
        case 'number':
            return '@"1"';
        case 'formatted':
            // Special 'string' case to handle constant expression text-field that automatically
            // converts Formatted back to string.
            return layerType === 'string' ?
                `@"'${_.startCase(propertyName)}'"` :
                `@"${_.startCase(propertyName)}"`;
        case 'resolvedImage':
            return layerType === 'string' ?
                `@"${_.startCase(propertyName)}"` :
                `@"MGL_FUNCTION('image', '${_.startCase(propertyName)}')"`;
        case 'string':
            return `@"'${_.startCase(propertyName)}'"`;
        case 'enum':
            return `@"'${_.last(_.keys(property.values))}'"`;
        case 'color':
            return '@"%@", [MGLColor redColor]';
        case 'array':
            switch (arrayType(property)) {
                case 'dasharray':
                    return '@"{1, 2}"';
                case 'font':
                    return `@"{'${_.startCase(propertyName)}', '${_.startCase(_.reverse(propertyName.split('')).join(''))}'}"`;
                case 'padding': {
                    if (arraysAsStructs) {
                        let iosValue = '[NSValue valueWithUIEdgeInsets:UIEdgeInsetsMake(1, 1, 1, 1)]'.indent(indent * 4);
                        let macosValue = '[NSValue valueWithEdgeInsets:NSEdgeInsetsMake(1, 1, 1, 1)]'.indent(indent * 4);
                        return `@"%@",\n#if TARGET_OS_IPHONE\n${iosValue}\n#else\n${macosValue}\n#endif\n${''.indent((indent - 1) * 4)}`;
                    }
                    return '@"{1, 1, 1, 1}"';
                }
                case 'offset':
                case 'translate': {
                    if (arraysAsStructs) {
                        let iosValue = '[NSValue valueWithCGVector:CGVectorMake(1, 1)]'.indent(indent * 4);
                        let macosValue = '[NSValue valueWithMGLVector:CGVectorMake(1, -1)]'.indent(indent * 4);
                        return `@"%@",\n#if TARGET_OS_IPHONE\n${iosValue}\n#else\n${macosValue}\n#endif\n${''.indent((indent - 1) * 4)}`;
                    }
                    return '@"{1, 1}"';
                }
                case 'anchor':
                    return `@"{'top','bottom'}"`;
                case 'mode':
                    return `@"{'horizontal','vertical'}"`;
                default:
                    throw new Error(`unknown array type for ${property.name}`);
            }
        default:
            throw new Error(`unknown type for ${property.name}`);
    }
};

global.mbglTestValue = function (property, layerType) {
    let propertyName = originalPropertyName(property);
    switch (property.type) {
        case 'boolean':
            return property.default ? 'false' : 'true';
        case 'number':
            return '1.0';
        case 'formatted':
        case 'string':
        case 'resolvedImage':
            return `"${_.startCase(propertyName)}"`;
        case 'enum': {
            let type = camelize(originalPropertyName(property));
            if (/-translate-anchor$/.test(originalPropertyName(property))) {
                type = 'TranslateAnchor';
            }
            if (/-(rotation|pitch)-alignment$/.test(originalPropertyName(property))) {
                type = 'Alignment';
            }
            if (/^(text|icon)-anchor$/.test(originalPropertyName(property))) {
                type = 'SymbolAnchor'
            }
            let value = camelize(_.last(_.keys(property.values)));
            if (property['light-property']) {
                return `mbgl::style::Light${type}Type::${value}`;
            }
            return `mbgl::style::${type}Type::${value}`;
        }
        case 'color':
            return '{ 1, 0, 0, 1 }';
        case 'array':
            switch (arrayType(property)) {
                case 'dasharray':
                    return '{1, 2}';
                case 'font':
                    return `{ "${_.startCase(propertyName)}", "${_.startCase(_.reverse(propertyName.split('')).join(''))}" }`;
                case 'padding':
                    return '{ 1, 1, 1, 1 }';
                case 'offset':
                case 'translate':
                    return '{ 1, 1 }';
                case 'anchor':
                    return '{ mbgl::style::SymbolAnchorType::Top, mbgl::style::SymbolAnchorType::Bottom }';
                case 'mode':
                    return '{ mbgl::style::TextWritingModeType::Horizontal, mbgl::style::TextWritingModeType::Vertical }';
                default:
                    throw new Error(`unknown array type for ${property.name}`);
            }
        default:
            throw new Error(`unknown type for ${property.name}`);
    }
};

global.mbglExpressionTestValue = function (property, layerType) {
    let propertyName = originalPropertyName(property);
    switch (property.type) {
        case 'enum':
            return `"${_.last(_.keys(property.values))}"`;
        case 'color':
            return 'mbgl::Color(1, 0, 0, 1)';
        case 'array':
            switch (arrayType(property)) {
                case 'anchor':
                    return `{"top", "bottom"}`;
                case 'mode':
                    return `{"horizontal", "vertical"}`;
                default:
                    break;
            }
        default:
            return global.mbglTestValue(property, layerType);
    }
};

global.testGetterImplementation = function (property, layerType, isFunction) {
    let helperMsg = testHelperMessage(property, layerType, isFunction);
    let value = `[MGLRuntimeStylingHelper ${helperMsg}]`;
    if (property.type === 'enum') {
        if (isFunction) {
            return `XCTAssertEqualObjects(gLayer.${objCName(property)}, ${value});`;
        }
        return `XCTAssert([gLayer.${objCName(property)} isKindOfClass:[MGLConstantStyleValue class]]);
    XCTAssertEqualObjects(gLayer.${objCName(property)}, ${value});`;
    }
    return `XCTAssertEqualObjects(gLayer.${objCName(property)}, ${value});`;
};

global.testHelperMessage = function (property, layerType, isFunction) {
    let fnSuffix = isFunction ? 'Function' : '';
    switch (property.type) {
        case 'boolean':
            return 'testBool' + fnSuffix;
        case 'number':
            return 'testNumber' + fnSuffix;
        case 'formatted':
        case 'string':
        case 'resolvedImage':
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
    let doc = property.doc.replace(/`([^`]+?)` is set to `([^`]+?)`(?: or `([^`]+?)`)?/g, function (m, peerPropertyName, propertyValue, secondPropertyValue, offset, str) {
        let otherProperty = camelizeWithLeadingLowercase(peerPropertyName);
        let otherValue = objCType(layerType, peerPropertyName) + camelize(propertyValue);
        if (property.type == 'array' && kind == 'light') {
            otherValue = propertyValue;
        }
        const firstPropertyValue = '`' + `${otherProperty}` + '` is set to `' + `${otherValue}` + '`';
        if (secondPropertyValue) {
            return firstPropertyValue + ' or `' +
                objCType(layerType, peerPropertyName) + camelize(secondPropertyValue) +
                '`';
        } else {
            return firstPropertyValue;
        }
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
        if (kind === 'enum') {
            let layoutProperties = spec[`layout_${layerType}`] || [];
            let paintProperties = spec[`paint_${layerType}`] || [];
            if (symbol in layoutProperties || symbol in paintProperties) {
                return '`MGL' + camelize(layerType) + 'StyleLayer.' + camelizeWithLeadingLowercase(symbol) + '`';
            }
        }
        if ('values' in property && Object.keys(property.values).indexOf(symbol) !== -1) {
            let objCType = global.objCType(layerType, property.name);
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
            if (!property.required && kind != 'light') {
                doc += ' Set this property to `nil` to reset it to the default value.';
            }
        }
        if ('requires' in property) {
            doc += '\n\n' + propertyReqs(property, spec[`${kind}_${layerType}`], layerType);
        }
        if ('original' in property) {
            let anchor;
            switch (kind) {
                case 'layout':
                    anchor = `layout-${layerType}-${property.original}`;
                    break;
                case 'paint':
                    anchor = `paint-${property.original}`;
                    break;
            }
            doc += `\n\nThis attribute corresponds to the <a href="https://www.mapbox.com/mapbox-gl-style-spec/#${anchor}"><code>${property.original}</code></a> layout property in the Mapbox Style Specification.`;
        }
        doc += '\n\nYou can set this property to an expression containing any of the following:\n\n';
        doc += `* Constant ${describeType(property)} values`;
        if ('minimum' in property) {
            if ('maximum' in property) {
                doc += ` between ${formatNumber(property.minimum)} and ${formatNumber(property.maximum)} inclusive`;
            } else {
                doc += ` no less than ${formatNumber(property.minimum)}`;
            }
        } else if ('maximum' in property) {
            doc += ` no greater than ${formatNumber(property.maximum)}`;
        }
        doc += '\n';
        if (property.type === 'enum') {
            doc += '* Any of the following constant string values:\n';
            doc += Object.keys(property.values).map(value => '  * `' + value + '`: ' + property.values[value].doc).join('\n') + '\n';
        } else if (property.type === 'array' && property.value === 'enum') {
            doc += '* Constant array, in which each element is any of the following constant string values:\n';
            doc += Object.keys(property.values).map(value => '  * `' + value + '`: ' + property.values[value].doc).join('\n') + '\n';
        }
        if (property.type === 'formatted') {
            doc += '* Formatted expressions.\n';
        }
        doc += '* Predefined functions, including mathematical and string operators\n' +
            '* Conditional expressions\n' +
            '* Variable assignments and references to assigned variables\n';
        const inputVariable = property.expression && property['property-type'] === 'color-ramp' ?
            '$' + camelizeWithLeadingLowercase(property.expression.parameters[0]) : '$zoomLevel';
        if (isDataDriven(property)) {
            doc += `* Interpolation and step functions applied to the \`${inputVariable}\` variable and/or feature attributes\n`;
        } else if (property.expression && property.expression.interpolated) {
            doc += `* Interpolation and step functions applied to the \`${inputVariable}\` variable\n\n` +
                'This property does not support applying interpolation or step functions to feature attributes.';
        } else {
            doc += `* Step functions applied to the \`${inputVariable}\` variable\n\n` +
                `This property does not support applying interpolation functions to the \`${inputVariable}\` variable or applying interpolation or step functions to feature attributes.`;
        }
    }
    return doc;
};

global.propertyExample = function (property) {
    return property.examples;
};

global.isDataDriven = function (property) {
  return property['property-type'] === 'data-driven' || property['property-type'] === 'cross-faded-data-driven';
};

global.propertyReqs = function (property, propertiesByName, type) {
    return 'This property is only applied to the style if ' + property.requires.map(function (req) {
        if (typeof req === 'string') {
            return '`' + camelizeWithLeadingLowercase(req) + '` is non-`nil`';
        } else if ('!' in req) {
            return '`' + camelizeWithLeadingLowercase(req['!']) + '` is set to `nil`';
        } else {
            let name = Object.keys(req)[0];
            if (name === 'source')
                return 'the data source requirements are met';
            return '`' + camelizeWithLeadingLowercase(name) + '` is set to an expression that evaluates to ' + describeValue(req[name], propertiesByName[name], type);
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

global.describeType = function (property) {
    switch (property.type) {
        case 'boolean':
            return 'Boolean';
        case 'number':
            return 'numeric';
        case 'formatted':
        case 'string':
        case 'resolvedImage':
            return 'string';
        case 'enum':
            return '`MGL' + camelize(property.name) + '`';
        case 'color':
            return '`UIColor`';
        case 'array':
            switch (arrayType(property)) {
                case 'padding':
                    return '`UIEdgeInsets`';
                case 'offset':
                case 'translate':
                    return '`CGVector`';
                case 'position':
                    return '`MGLSphericalPosition`';
                case 'anchor':
                    return '`MGLTextAnchor` array';
                case 'mode':
                    return '`MGLTextWritingMode` array';
                default:
                    return 'array';
            }
            break;
        default:
            throw new Error(`unknown type for ${property.name}`);
    }
}

global.describeValue = function (value, property, layerType) {
    if (Array.isArray(value) && property.type !== 'array' && property.type !== 'enum') {
        switch (value[0]) {
            case 'interpolate': {
                let curveType = value[1][0];
                let minimum = describeValue(value[3 + value.length % 2], property, layerType);
                let maximum = describeValue(_.last(value), property, layerType);
                return `${curveType.match(/^[aeiou]/i) ? 'an' : 'a'} ${curveType} interpolation expression ranging from ${minimum} to ${maximum}`;
            }
            default:
                throw new Error(`No description available for ${value[0]} expression in ${property.name} of ${layerType}.`);
        }
    }

    switch (property.type) {
        case 'boolean':
            return value ? '`YES`' : '`NO`';
        case 'number':
            return 'the float ' + '`' + formatNumber(value) + '`';
        case 'formatted':
        case 'string':
        case 'resolvedImage':
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
                displayValue = `\`${value}\``;
            }
            return displayValue;
        case 'color':
            let color = parseColor(value);
            if (!color) {
                throw new Error(`unrecognized color format in default value of ${property.name}`);
            }
            if (color.r === 0 && color.g === 0 && color.b === 0 && color.a === 0) {
                return '`UIColor.clearColor`';
            }
            if (color.r === 0 && color.g === 0 && color.b === 0 && color.a === 1) {
                return '`UIColor.blackColor`';
            }
            if (color.r === 1 && color.g === 1 && color.b === 1 && color.a === 1) {
                return '`UIColor.whiteColor`';
            }
            return 'a `UIColor`' + ` object whose RGB value is ${formatNumber(color.r)}, ${formatNumber(color.g)}, ${formatNumber(color.b)} and whose alpha value is ${formatNumber(color.a)}`;
        case 'array':
            let units = property.units || '';
            if (units) {
                units = ` ${units}`.replace(/pixel/, 'point');
            }
            switch (arrayType(property)) {
                case 'padding':
                    if (value[0] === 0 && value[1] === 0 && value[2] === 0 && value[3] === 0) {
                        return 'an `NSValue` object containing `UIEdgeInsetsZero`';
                    }
                    return 'an `NSValue` object containing a `UIEdgeInsets` struct set to' + ` ${formatNumber(value[0])}${units} on the top, ${formatNumber(value[3])}${units} on the left, ${formatNumber(value[2])}${units} on the bottom, and ${formatNumber(value[1])}${units} on the right`;
                case 'offset':
                case 'translate':
                    return 'an `NSValue` object containing a `CGVector` struct set to' + ` ${formatNumber(value[0])}${units} rightward and ${formatNumber(value[1])}${units} downward`;
                case 'position':
                    return 'an `MGLSphericalPosition` struct set to' + ` ${formatNumber(value[0])} radial, ${formatNumber(value[1])} azimuthal and ${formatNumber(value[2])} polar`;
                default:
                    return 'the array `' + value.join('`, `') + '`';
            }
        default:
            throw new Error(`unknown type for ${property.name}`);
    }
};

global.formatNumber = function (num) {
    return num.toLocaleString().replace('-', '\u2212');
}

global.propertyDefault = function (property, layerType) {
    if (property.name === 'heatmap-color') {
        return 'an expression that evaluates to a rainbow color scale from blue to red';
    } else {
        return 'an expression that evaluates to ' + describeValue(property.default, property, layerType);
    }
};

global.originalPropertyName = function (property) {
    return property.original || property.name;
};

global.enumName = function (property) {
    return property.enumName || property.name;
};

global.propertyType = function (property) {
    switch (property.type) {
        case 'boolean':
            return 'NSNumber *';
        case 'number':
            return 'NSNumber *';
        case 'formatted':
        case 'string':
        case 'resolvedImage':
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
                case 'position':
                case 'offset':
                case 'translate':
                    return 'NSValue *';
                case 'anchor':
                case 'mode':
                    return 'NSArray<NSValue *> *';
                default:
                    throw new Error(`unknown array type for ${property.name}`);
            }
        default:
            throw new Error(`unknown type for ${property.name}`);
    }
};

global.isInterpolatable = function (property) {
    const type = property.type === 'array' ? property.value : property.type;
    return type !== 'boolean' &&
        type !== 'enum' &&
        type !== 'string' &&
        type !== 'resolvedImage' &&
        type !== 'formatted';
};

global.valueTransformerArguments = function (property) {
    let objCType = propertyType(property);
    switch (property.type) {
        case 'boolean':
            return ['bool', objCType];
        case 'number':
            return ['float', objCType];
        case 'formatted':
            return ['mbgl::style::expression::Formatted', objCType];
        case 'resolvedImage':
            return ['mbgl::style::expression::Image', objCType];
        case 'string':
            return ['std::string', objCType];
        case 'enum':
            return [mbglType(property), 'NSValue *', mbglType(property), `MGL${camelize(property.name)}`];
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
                case 'position':
                    return ['mbgl::style::Position', objCType];
                case 'offset':
                case 'translate':
                    return ['std::array<float, 2>', objCType];
                case 'anchor':
                    return ['std::vector<mbgl::style::SymbolAnchorType>', objCType, 'mbgl::style::SymbolAnchorType', 'MGLTextAnchor'];
                case 'mode':
                    return ['std::vector<mbgl::style::TextWritingModeType>', objCType, 'mbgl::style::TextWritingModeType', 'MGLTextWritingMode'];
                default:
                    throw new Error(`unknown array type for ${property.name}`);
            }
        default:
            throw new Error(`unknown type for ${property.name}`);
    }
};

global.mbglType = function(property) {
    switch (property.type) {
        case 'boolean':
            return 'bool';
        case 'number':
            return 'float';
        case 'formatted':
            return 'mbgl::style::expression::Formatted';
        case 'resolvedImage':
            return 'mbgl::style::expression::Image';
        case 'string':
            return 'std::string';
        case 'enum': {
            let type = camelize(originalPropertyName(property));
            if (property['light-property']) {
                return `mbgl::style::Light${type}Type`;
            }
            if (/-translate-anchor$/.test(originalPropertyName(property))) {
                type = 'TranslateAnchor';
            }
            if (/-(rotation|pitch)-alignment$/.test(originalPropertyName(property))) {
                type = 'Alignment';
            }
            if (/^(text|icon)-anchor$/.test(originalPropertyName(property))) {
                type = 'SymbolAnchor'
            }
            return `mbgl::style::${type}Type`;
        }
        case 'color':
            return 'mbgl::Color';
        case 'array':
            switch (arrayType(property)) {
                case 'dasharray':
                    return 'std::vector<float>';
                case 'font':
                    return 'std::vector<std::string>';
                case 'padding':
                    return 'std::array<float, 4>';
                case 'offset':
                case 'translate':
                    return 'std::array<float, 2>';
                case 'position':
                    return 'mbgl::style::Position';
                case 'anchor':
                    return 'std::vector<mbgl::style::SymbolAnchorType>';
                case 'mode':
                    return 'std::vector<mbgl::style::TextWritingModeType>';
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
};

global.setSourceLayer = function() {
    return `_layer->setSourceLayer(sourceLayer.UTF8String);`
};

const lightProperties = Object.keys(spec['light']).reduce((memo, name) => {
  var property = spec['light'][name];
  property.name = name;
  property['light-property'] = true;
  memo.push(property);
  return memo;
}, []);

const lightDoc = spec['light-cocoa-doc'];
const lightType = 'light';

const layerH = ejs.compile(fs.readFileSync('platform/darwin/src/MGLStyleLayer.h.ejs', 'utf8'), { strict: true });
const layerPrivateH = ejs.compile(fs.readFileSync('platform/darwin/src/MGLStyleLayer_Private.h.ejs', 'utf8'), { strict: true });
const layerM = ejs.compile(fs.readFileSync('platform/darwin/src/MGLStyleLayer.mm.ejs', 'utf8'), { strict: true});
const testLayers = ejs.compile(fs.readFileSync('platform/darwin/test/MGLStyleLayerTests.mm.ejs', 'utf8'), { strict: true});
const forStyleAuthorsMD = ejs.compile(fs.readFileSync('platform/darwin/docs/guides/For Style Authors.md.ejs', 'utf8'), { strict: true });
const ddsGuideMD = ejs.compile(fs.readFileSync('platform/darwin/docs/guides/Migrating to Expressions.md.ejs', 'utf8'), { strict: true });
const templatesMD = ejs.compile(fs.readFileSync('platform/darwin/docs/guides/Tile URL Templates.md.ejs', 'utf8'), { strict: true });

const lightH = ejs.compile(fs.readFileSync('platform/darwin/src/MGLLight.h.ejs', 'utf8'), {strict: true});
const lightM = ejs.compile(fs.readFileSync('platform/darwin/src/MGLLight.mm.ejs', 'utf8'), {strict: true});
const testLight = ejs.compile(fs.readFileSync('platform/darwin/test/MGLLightTest.mm.ejs', 'utf8'), { strict: true});
writeIfModified(`platform/darwin/src/MGLLight.h`, duplicatePlatformDecls(lightH({ properties: lightProperties, doc: lightDoc, type: lightType })));
writeIfModified(`platform/darwin/src/MGLLight.mm`, lightM({ properties: lightProperties, doc: lightDoc, type: lightType }));
writeIfModified(`platform/darwin/test/MGLLightTest.mm`, testLight({ properties: lightProperties, doc: lightDoc, type: lightType }));


const layers = _(spec.layer.type.values).map((value, layerType) => {
    const layoutProperties = Object.keys(spec[`layout_${layerType}`]).reduce((memo, name) => {
        if (name !== 'visibility') {
            spec[`layout_${layerType}`][name].name = name;
            memo.push(spec[`layout_${layerType}`][name]);
        }
        return memo;
    }, []);

    const paintProperties = Object.keys(spec[`paint_${layerType}`]).reduce((memo, name) => {
        spec[`paint_${layerType}`][name].name = name;
        memo.push(spec[`paint_${layerType}`][name]);
        return memo;
    }, []);

    return {
        doc: spec.layer.type.values[layerType].doc,
        examples: spec.layer.type.values[layerType].examples,
        type: layerType,
        layoutProperties: _.sortBy(layoutProperties, ['name']),
        paintProperties: _.sortBy(paintProperties, ['name']),
    };
}).sortBy(['type']).value();

function duplicatePlatformDecls(src) {
    // Look for a documentation comment that contains “MGLColor” or “UIColor”
    // and the subsequent function, method, or property declaration. Try not to
    // match greedily.
    return src.replace(/(\/\*\*(?:\*[^\/]|[^*])*?\b(?:MGL|NS|UI)Color\b[\s\S]*?\*\/)(\s*.+?;)/g,
                       (match, comment, decl) => {
        let macosComment = comment.replace(/\b(?:MGL|UI)Color\b/g, 'NSColor')
            // Use the correct indefinite article.
            .replace(/\ba(\s+`?NSColor)\b/gi, 'an$1');
        let iosDecl = decl.replace(/\bMGLColor\b/g, 'UIColor');
        let macosDecl = decl.replace(/\b(?:MGL|UI)Color\b/g, 'NSColor');
        return `\
#if TARGET_OS_IPHONE
${comment}${iosDecl}
#else
${macosComment}${macosDecl}
#endif`;
    })
        // Do the same for CGVector-typed properties.
        .replace(/(\/\*\*(?:\*[^\/]|[^*])*?\b(?:CGVector|UIEdgeInsets(?:Zero)?)\b[\s\S]*?\*\/)(\s*.+?;)/g,
                       (match, comment, decl) => {
        let macosComment = comment.replace(/\bdownward\b/g, 'upward')
            .replace(/\bUI(EdgeInsets(?:Zero)?)\b/g, 'NS$1');
        return `\
#if TARGET_OS_IPHONE
${comment}${decl}
#else
${macosComment}${decl}
#endif`;
    });
}

var renamedPropertiesByLayerType = {};

for (var layer of layers) {
    layer.properties = _.concat(layer.layoutProperties, layer.paintProperties);
    let enumProperties = _.filter(layer.properties, prop => definesEnum(prop, layer.properties));
    if (enumProperties.length) {
        layer.enumProperties = enumProperties;
    }

    let renamedProperties = {};
    _.assign(renamedProperties, _.filter(layer.properties, prop => 'original' in prop || 'getter' in prop));
    if (!_.isEmpty(renamedProperties)) {
        renamedPropertiesByLayerType[layer.type] = renamedProperties;
    }

    writeIfModified(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}.h`, duplicatePlatformDecls(layerH(layer)));
    writeIfModified(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}_Private.h`, duplicatePlatformDecls(layerPrivateH(layer)));
    writeIfModified(`platform/darwin/src/${prefix}${camelize(layer.type)}${suffix}.mm`, layerM(layer));
    writeIfModified(`platform/darwin/test/${prefix}${camelize(layer.type)}${suffix}Tests.mm`, testLayers(layer));
}

// Extract examples for guides from unit tests.
let examplesSrc = fs.readFileSync('platform/darwin/test/MGLDocumentationGuideTests.swift', 'utf8');
const exampleRegex = /func test([\w$]+)\s*\(\)\s*\{[^]*?\n([ \t]+)\/\/#-example-code\n([^]+?)\n\2\/\/#-end-example-code\n/gm;

let examples = {};
let match;
while ((match = exampleRegex.exec(examplesSrc)) !== null) {
    let testMethodName = match[1],
        indentation = match[2],
        exampleCode = match[3];

    // Trim leading whitespace from the example code.
    exampleCode = exampleCode.replace(new RegExp('^' + indentation, 'gm'), '');

    examples[testMethodName] = exampleCode;
}

global.guideExample = function (guide, exampleId, os) {
    // Get the contents of the test method whose name matches the symbol path.
    let testMethodName = `${guide}$${exampleId}`;
    let example = examples[testMethodName];
    if (!example) {
        console.error(`MGLDocumentationExampleTests.test${testMethodName}() not found.`);
        process.exit(1);
    }

    // Resolve conditional compilation blocks.
    example = example.replace(/^(\s*)#if\s+os\((iOS|macOS)\)\n([^]*?)(?:^\1#else\n([^]*?))?^\1#endif\b\n?/gm,
                              function (m, indentation, ifOs, ifCase, elseCase) {
      return (os === ifOs ? ifCase : elseCase).replace(new RegExp('^    ', 'gm'), '');
    }).replace(/\n$/, '');

    return '```swift\n' + example + '\n```';
};

writeIfModified(`platform/ios/docs/guides/For Style Authors.md`, forStyleAuthorsMD({
    os: 'iOS',
    renamedProperties: renamedPropertiesByLayerType,
    layers: layers,
}));
writeIfModified(`platform/macos/docs/guides/For Style Authors.md`, forStyleAuthorsMD({
    os: 'macOS',
    renamedProperties: renamedPropertiesByLayerType,
    layers: layers,
}));
writeIfModified(`platform/ios/docs/guides/Migrating to Expressions.md`, ddsGuideMD({
    os: 'iOS',
}));
writeIfModified(`platform/macos/docs/guides/Migrating to Expressions.md`, ddsGuideMD({
    os: 'macOS',
}));
writeIfModified(`platform/ios/docs/guides/Tile URL Templates.md`, templatesMD({
    os: 'iOS',
}));
writeIfModified(`platform/macos/docs/guides/Tile URL Templates.md`, templatesMD({
    os: 'macOS',
}));
