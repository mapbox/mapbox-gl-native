'use strict';

/* jshint node:true */

var test = require('tape');
var mbgl = require('..');
var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');
var PNG = require('pngjs').PNG;
var compare = require('./compare.js');
var suitePath = path.dirname(require.resolve('mapbox-gl-test-suite/package.json'));

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
    var dir = path.join(suitePath, 'tests', base, key);
    mkdirp.sync(dir);

    return function(t) {
        var watchdog = setTimeout(function() {
            t.fail('timed out after 20 seconds');
        }, 20000);

        t.once('end', function() {
            clearTimeout(watchdog);
        });

        var options = {};
        options.request = function(req) {
            fs.readFile(path.join(suitePath, decodeURIComponent(req.url)), function(err, data) {
                req.respond(err, { data: data });
                t.error(err);
            });
        };
        options.cancel = function() {};
        options.ratio = 1.0;

        var map = new mbgl.Map(options);
        map.load(style);

        map.render(info[key], function(err, data) {
            t.error(err);

            map.release();

            var expected = path.join(dir, 'expected.png');
            var actual = path.join(dir, 'actual.png');
            var diff = path.join(dir, 'diff.png');

            var png = new PNG({
                width: data.width,
                height: data.height
            });

            png.data = data.pixels;

            if (process.env.UPDATE) {
                png.pack()
                    .pipe(fs.createWriteStream(expected))
                    .on('finish', t.end);
            } else {
                png.pack()
                    .pipe(fs.createWriteStream(actual))
                    .on('finish', function() {
                        compare(actual, expected, diff, t, function(err, diff) {
                            t.error(err);

                            var allowed = ('diff' in info) ? info.diff : 0.001;
                            var color = diff <= allowed ? 'green' : 'red';

                            results += format(resultTemplate, {
                                name: base,
                                key: key,
                                color: color,
                                error: err ? '<p>' + err + '</p>' : '',
                                difference: diff,
                                zoom: info.zoom || 0,
                                center: info.center || [0, 0],
                                bearing: info.bearing || 0,
                                width: info.width || 512,
                                height: info.height || 512
                            });

                            t.ok(diff <= allowed, 'actual matches expected');
                            t.end();
                        });
                });
            }
        });
    };
}

function rewriteLocalSchema(url) {
    var regex = /^local:\/\//;
    if (url instanceof Array) {
        return url.map(function(str) {
            return str.replace(regex, '');
        });
    } else if (typeof url === "string") {
        return url.replace(regex, '');
    }
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
