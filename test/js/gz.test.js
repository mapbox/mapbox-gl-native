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

mkdirp.sync('test/results');

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

    mbgl.on('message', function(msg) {
        t.error(msg);
    });

    setup(getFileSource(true, t), function(map) {
        map.load(style);
        map.render({}, function(err, image) {
            mbgl.compressPNG(image, function(err, data) {
                fs.writeFileSync('test/results/image-gz-success.png', data);
                mbgl.removeAllListeners('message');
                t.end();
            });
        });
    });
});

test('unhandled gzip', function(t) {

    mbgl.once('message', function(msg) {
        t.ok(msg, 'emits error');
        t.equal(msg.class, 'ParseTile');
        t.equal(msg.severity, 'ERROR');
        t.equal(msg.text, 'Parsing [0/0/0] failed: pbf unknown field type exception');
    });

    setup(getFileSource(false, t), function(map) {
        map.load(style);
        map.render({}, function(err, image) {
            mbgl.compressPNG(image, function(err, data) {
                fs.writeFileSync('test/results/image-gz-fail.png', data);
                t.ok(fs.statSync('test/results/image-gz-success.png')['size'] > fs.statSync('test/results/image-gz-fail.png')['size'], 'render is smaller than unpacked render');
                server.close(t.end);
            });
        });
    });

});
