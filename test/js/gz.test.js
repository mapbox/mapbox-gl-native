'use strict';

/* jshint node: true */

var test = require('tape');
var mbgl = require('../..');
var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');
var http = require('http');
var request = require('request');
var st = require('st');
var style = require('../../test/fixtures/style.json');
var compare = require('../compare.js');
var actualDir = 'test/actual';
var expectedDir = 'test/expected';

mkdirp.sync(actualDir);
mkdirp.sync(expectedDir);

var server = http.createServer(st({path: __dirname + '/..'}));
server.listen(2900);

function setup(fileSource, callback) {
    callback(new mbgl.Map(fileSource));
}

function getFileSource(gzip, t) {
    var fileSource = new mbgl.FileSource();

    fileSource.request = function(req) {
        var parts = req.url.split('.');
        var filetype = parts[parts.length - 1];

        request({
            url: 'http://localhost:2900' + path.join('/', req.url),
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

test('before tests', function(t) {
    server.listen(2900, t.end);
});

test('unpacked gzip', function(t) {

    mbgl.once('message', function(msg) {
        t.error(msg);
    });

    setup(getFileSource(true, t), function(map) {
        map.load(style);
        map.render({}, function(err, image) {
            mbgl.compressPNG(image, function(err, image) {
                t.error(err);

                var expected = path.join(expectedDir, 'gz-success.png');
                var actual = path.join(actualDir, 'gz-success.png');
                var diff = path.join(actualDir, 'gz-success-diff.png');

                if (process.env.UPDATE) {
                    fs.writeFile(expected, image, function(err) {
                        t.error(err);
                        mbgl.removeAllListeners('message');
                        t.end();
                    });
                } else {
                    fs.writeFile(actual, image, function(err) {
                        t.error(err);
                        compare(actual, expected, diff, t, function(error, difference) {
                            t.ok(difference <= 0.001, 'actual matches expected');
                            mbgl.removeAllListeners('message');
                            t.end();
                        });
                    });
                }
            });
        });
    });
});

test('unhandled gzip', function(t) {

    mbgl.once('message', function(msg) {
        t.ok(msg, 'emits error');
        t.equal(msg.class, 'ParseTile');
        t.equal(msg.severity, 'ERROR');
        t.equal(msg.text.indexOf('failed: pbf unknown field type exception') !== -1, true);
    });

    setup(getFileSource(false, t), function(map) {
        map.load(style);
        map.render({}, function(err, image) {
            mbgl.compressPNG(image, function(err, image) {
                t.error(err);

                var expected = path.join(expectedDir, 'gz-fail-expected.png');
                var actual = path.join(actualDir, 'gz-fail-actual.png');
                var diff = path.join(actualDir, 'gz-fail-diff.png');

                if (process.env.UPDATE) {
                    fs.writeFile(expected, image, function(err) {
                        t.error(err);
                        server.close(t.end);
                    });
                } else {
                    fs.writeFile(actual, image, function(err) {
                        t.error(err);
                        compare(actual, expected, diff, t, function(error, difference) {
                            t.ok(difference <= 0.001, 'actual matches expected');
                            mbgl.removeAllListeners('message');
                            server.close(t.end);
                        });
                    });
                }
            });
        });
    });

});




