'use strict';

var suite = require('../../../mapbox-gl-js/test/integration').expression;
var mbgl = require('../index');
var ignores = require('./ignores.json');

var tests;

if (process.argv[1] === __filename && process.argv.length > 2) {
    tests = process.argv.slice(2);
}

function getExpectedType(spec) {
    if (spec.type === 'array') {
        const itemType = getExpectedType({ type: spec.value });
        const array = {
            kind: 'array',
            itemType: itemType || { kind: 'value' },
        };
        if (typeof spec.length === 'number') {
            array.N = spec.length;
        }
        return array;
    }

    if (spec.type === 'enum') {
        return { kind: 'string' };
    }

    return typeof spec.type === 'string' ? {kind: spec.type} : null;
}

suite.run('native', {ignores: ignores, tests: tests}, (fixture) => {
    const compiled = {};
    const recompiled = {};
    const result = {
        compiled,
        recompiled
    };

    const spec = fixture.propertySpec || {};
    const expression = mbgl.Expression.parse(fixture.expression, getExpectedType(spec));

    const evaluateExpression = (expression, compilationResult) => {
        if (expression instanceof mbgl.Expression) {
            compilationResult.result = 'success';
            compilationResult.isFeatureConstant = expression.isFeatureConstant();
            compilationResult.isZoomConstant = expression.isZoomConstant();
            compilationResult.type = expression.getType();

            const evaluate = fixture.inputs || [];
            const evaluateResults = [];
            for (const input of evaluate) {
                const feature = Object.assign({
                    type: 'Feature',
                    properties: {},
                    geometry: { type: 'Point', coordinates: [0, 0] }
                }, input[1])

                const output = expression.evaluate(input[0], feature);
                evaluateResults.push(output);
            }

            if (fixture.inputs) {
                return evaluateResults;
            }
        } else {
            compilationResult.result = 'error';
            compilationResult.errors = expression;
        }
    }

    result.outputs = evaluateExpression(expression, compiled);
    if (expression instanceof mbgl.Expression) {
        result.serialized = expression.serialize();
        const recompiledExpression = mbgl.Expression.parse(result.serialized, getExpectedType(spec));
        result.roundTripOutputs = evaluateExpression(recompiledExpression, recompiled);
        // Type is allowed to change through serialization
        // (eg "array" -> "array<number, 3>")
        // Override the round-tripped type here so that the equality check passes
        recompiled.type = compiled.type;
    }

    return result;
});
