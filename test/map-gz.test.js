'use strict';

/* jshint node: true */

var test = require('tape');
var mbgl = require('../');
var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');
var http = require('http');
var request = require('request');
var st = require('st');

/*
FIX: In all cases style-gz fails, and in all cases, style.json passes,
regardless of how gzip is set in either st or request.
*/
var style = require('../test/fixtures/style.json');
// var style = require('../test/fixtures/style-gz.json');

mkdirp.sync('results');

var server = http.createServer(st({path: __dirname}));
server.listen(2900);

function setup(fileSource, callback) {
    callback(new mbgl.Map(fileSource));
}

function getFileSource(gzip, t) {
    var fileSource = new mbgl.FileSource();

    fileSource.request = function(req) {
        request({
            url: 'http://localhost:2900' + path.join('/', req.url),
            encoding: null,
            gzip: gzip
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

test('gzip passes', function(t) {

    /*
    FIX: I think need to turn OFF this event listener before this test
    is done so next test doesn't accidentally use it.
    */
    mbgl.on('message', function(msg) {
        t.error(msg);
    });

    setup(getFileSource(true, t), function(map) {
        map.load(style);
        map.render({}, function(err, image) {
            mbgl.compressPNG(image, function(err, data) {
                fs.writeFileSync('results/image-pgf-gz-false.png', data);
                t.end();
            });
        });
    });
});

test('gzip fails', function(t) {

    /*
    FIX: No message will be emmitted if gzip doesn't fail,
    so this test is pointless
    */
    mbgl.on('message', function(msg) {
        t.equal(msg === null, 'gzipped pbf fails when gzip:false');
    });

    setup(getFileSource(false, t), function(map) {
        map.load(style);
        map.render({}, function(err, image) {
            mbgl.compressPNG(image, function(err, data) {
                fs.writeFileSync('results/image-pgf-gz-true.png', data);
                server.close(t.end);
            });
        });
    });

});
