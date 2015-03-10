'use strict';

/* jshint node:true */

var test = require('tape');
var mbgl = require('..');
var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');
var compare = require('./compare.js');
var suitePath = path.dirname(require.resolve('mapbox-gl-test-suite/package.json'));
var actualDir = 'test/actual';
var expectedDir = 'test/expected';

function template(name) {
    return fs.readFileSync(require.resolve('mapbox-gl-test-suite/templates/' + name + '.html.tmpl')).toString();
}

var results = '';
var resultTemplate = template('result');

function format(tmpl, kwargs) {
    return tmpl.replace(/\{\{|\}\}|\{([^}]+)\}/g, function(match, key) {
        if (match === '{{') return '{';
        if (match === '}}') return '}';
        return kwargs[key];
    });
}

function renderTest(style, info, base, key) {
    return function(t) {
        var watchdog = setTimeout(function() {
            t.fail('timed out after 20 seconds');
        }, 20000);

        t.once('end', function() {
            clearTimeout(watchdog);
        });

        var fileSource = new mbgl.FileSource();
        fileSource.request = function(req) {
            fs.readFile(path.join(suitePath, decodeURIComponent(req.url)), function(err, data) {
                req.respond(err, { data: data });
                t.error(err);
            });
        };
        fileSource.cancel = function() {};

        var map = new mbgl.Map(fileSource);
        map.load(style);

        map.render(info[key], function(err, image) {
            t.error(err);
            mbgl.compressPNG(image, function(err, image) {
                t.error(err);
                mkdirp.sync(actualDir);
                mkdirp.sync(expectedDir);

                var expected = path.join(expectedDir, 'expected.png');
                var actual = path.join(actualDir, 'actual.png');
                var diff = path.join(actualDir, 'diff.png');

                if (process.env.UPDATE) {
                    fs.writeFile(expected, image, function(err) {
                        t.error(err);
                        t.end();
                    });
                } else {
                    fs.writeFile(actual, image, function(err) {
                        t.error(err);
                        compare(actual, expected, diff, t, function(error, difference) {
                            var allowedDifference = ('diff' in info) ? info.diff : 0.001;
                            var color = difference <= allowedDifference ? 'green' : 'red';

                            results += format(resultTemplate, {
                                name: base,
                                key: key,
                                color: color,
                                error: error ? '<p>' + error + '</p>' : '',
                                difference: difference,
                                zoom: info.zoom || 0,
                                center: info.center || [0, 0],
                                bearing: info.bearing || 0,
                                width: info.width || 512,
                                height: info.height || 512
                            });

                            t.ok(difference <= allowedDifference, 'actual matches expected');
                            t.end();
                        });
                    });
                }
            });
        });
    };
}

function rewriteLocalSchema(uri) {
    return uri.replace(/^local:\/\//, '');
}

var tests;

if (process.argv[1] === __filename) {
    tests = process.argv.slice(2);
}

test('Render', function(t) {
    fs.readdirSync(path.join(suitePath, 'tests')).forEach(function(dir) {
        if (dir === 'index.html' || dir[0] === '.') return;
        if (tests && tests.length && tests.indexOf(dir) < 0) return;

        var style = require(path.join(suitePath, 'tests', dir, 'style.json')),
            info  = require(path.join(suitePath, 'tests', dir, 'info.json'));

        for (var k in style.sources) {
            var source = style.sources[k];

            if (source.tiles) {
                source.tiles = source.tiles.map(rewriteLocalSchema);
            }

            if (source.url) {
                source.url = rewriteLocalSchema(source.url);
            }
        }

        if (style.sprite) style.sprite = rewriteLocalSchema(style.sprite);
        if (style.glyphs) style.glyphs = rewriteLocalSchema(style.glyphs);

        style = JSON.stringify(style);

        for (k in info) {
            (info[k].native === false ? t.skip : t.test)(dir + ' ' + k, renderTest(style, info, dir, k));
        }
    });

    t.test('results', function(t) {
        var p = path.join(suitePath, 'tests', 'index.html');
        fs.writeFileSync(p, format(template('results'), {results: results}));
        console.warn('Results at: ' + p);
        t.end();
    });
});
