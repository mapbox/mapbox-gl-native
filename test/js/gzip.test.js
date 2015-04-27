'use strict';

/* jshint node: true */

var test = require('tape').test;
var mbgl = require('../..');
var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');
var http = require('http');
var request = require('request');
var st = require('st');
var style = require('../../test/fixtures/style.json');
var compare = require('../compare.js');

var dirPath = path.join(path.dirname(require.resolve('../../package.json')), 'test');
var server = http.createServer(st({ path: dirPath }));
server.listen(0);

function filePath(name) {
    return ['expected', 'actual', 'diff'].reduce(function(prev, key) {
        var dir = path.join('test', key, 'gzip');
        mkdirp.sync(dir);
        prev[key] = path.join(dir, name);
        return prev;
    }, {});
}

function setup(fileSource, callback) {
    callback(new mbgl.Map(fileSource));
}

function getFileSource(gzip, t) {
    var fileSource = new mbgl.FileSource();

    fileSource.request = function(req) {
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
    };

    fileSource.cancel = function(req) {};

    return fileSource;
}

test('gzip', function(t) {
    t.test('success', function(t) {
        mbgl.on('message', function(msg) {
            if (msg.severity == 'ERROR') t.error(msg);
        });

        setup(getFileSource(true, t), function(map) {
            map.load(style);
            map.render({}, function(err, image) {
                mbgl.removeAllListeners('message');
                mbgl.compressPNG(image, function(err, image) {
                    t.error(err);

                    var filename = filePath('success.png');

                    if (process.env.UPDATE) {
                        fs.writeFile(filename.expected, image, function(err) {
                            t.error(err);
                            t.end();
                        });
                    } else {
                        fs.writeFile(filename.actual, image, function(err) {
                            t.error(err);
                            compare(filename.actual, filename.expected, filename.diff, t, function(error, difference) {
                                t.ok(difference <= 0.01, 'actual matches expected');
                                t.end();
                            });
                        });
                    }
                });
            });
        });
    });

    t.test('unhandled', function(t) {
        var errorEmitted = false;

        mbgl.on('message', function(msg) {
            if (msg.severity == 'ERROR') {
                errorEmitted = true;
                t.ok(msg, 'emits error');
                t.equal(msg.class, 'ParseTile');
                t.equal(msg.severity, 'ERROR');
                t.ok(msg.text.match(/failed: pbf unknown field type exception/), 'pbf unknown field type exception');
            }
        });

        setup(getFileSource(false, t), function(map) {
            map.load(style);
            map.render({}, function(err, image) {
                if (!errorEmitted) t.fail('no error emitted');
                mbgl.removeAllListeners('message');
                mbgl.compressPNG(image, function(err, image) {
                    t.error(err);

                    var filename = filePath('unhandled.png');

                    if (process.env.UPDATE) {
                        fs.writeFile(filename.expected, image, function(err) {
                            t.error(err);
                            t.end();
                        });
                    } else {
                        fs.writeFile(filename.actual, image, function(err) {
                            t.error(err);
                            compare(filename.actual, filename.expected, filename.diff, t, function(error, difference) {
                                t.ok(difference <= 0.001, 'actual matches expected');
                                t.end();
                            });
                        });
                    }
                });
            });
        });
    });

    t.test('teardown', function(t) {
        server.close(t.end);
    });
});
