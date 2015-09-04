'use strict';

/* jshint node: true */

var test = require('tape').test;
var mbgl = require('../../../..');
var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');
var http = require('http');
var request = require('request');
var st = require('st');
var style = require('../fixtures/style.json');
var PNG = require('pngjs').PNG;
var compare = require('../compare.js');

var server = http.createServer(st({ path: path.join(__dirname, '..') }));
server.listen(0);

function filePath(name) {
    return ['expected', 'actual', 'diff'].reduce(function(prev, key) {
        var dir = path.join(__dirname, '..', key, 'gzip');
        mkdirp.sync(dir);
        prev[key] = path.join(dir, name);
        return prev;
    }, {});
}

function setup(options, callback) {
    callback(new mbgl.Map(options));
}

function getOptions(gzip, t) {
    return {
        request: function(req) {
            var parts = req.url.split('.');
            var filetype = parts[parts.length - 1];

            request({
                url: 'http://localhost:' + server.address().port + path.join('/', req.url),
                encoding: null,
                gzip: filetype === 'pbf' ? gzip : true,
                headers: {
                    'Accept-Encoding': 'gzip'
                }
            }, function (err, res, body) {
                t.error(err);
                var response = {};
                response.data = res.body;
                req.respond(null, response);
            });
        },
        ratio: 1.0
    };
}

test('gzip', function(t) {
    t.test('success', function(t) {
        mbgl.on('message', function(msg) {
            if (msg.severity == 'ERROR') t.error(msg);
        });

        setup(getOptions(true, t), function(map) {
            var options = { height: 512, width: 512 };
            map.load(style);
            map.render(options, function(err, pixels) {
                mbgl.removeAllListeners('message');
                map.release();

                t.error(err);

                var filename = filePath('success.png');

                var png = new PNG(options);

                png.data = pixels;

                if (process.env.UPDATE) {
                    png.pack()
                        .pipe(fs.createWriteStream(filename.expected))
                        .on('finish', t.end);
                } else {
                    png.pack()
                        .pipe(fs.createWriteStream(filename.actual))
                        .on('finish', function() {
                            compare(filename.actual, filename.expected, filename.diff, t, function(err, diff) {
                                t.error(err);
                                t.ok(diff <= 0.01, 'actual matches expected');
                                t.end();
                            });
                        });
                }
            });
        });
    });

    t.test('unhandled', function(t) {
        mbgl.once('message', function(msg) {
            if (msg.severity == 'ERROR') {
                t.ok(msg, 'emits error');
                t.equal(msg.class, 'Style');
                t.equal(msg.severity, 'ERROR');
                t.ok(msg.text.match(/pbf unknown field type exception/), 'error text matches');
            }
        });

        setup(getOptions(false, t), function(map) {
            map.load(style);
            map.render({}, function(err, data) {
                map.release();

                t.ok(err, 'returns error');
                t.ok(err.message.match(/Failed to parse/), 'error text matches');

                t.end();
            });
        });
    });

    t.test('teardown', function(t) {
        server.close(t.end);
    });
});
