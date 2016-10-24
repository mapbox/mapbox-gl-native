'use strict';

var test = require('tape');
var mbgl = require('../../index');
var fs = require('fs');
var path = require('path');
var PNG = require('pngjs').PNG;
var pixelmatch = require('pixelmatch');

function filename(name) {
    return ['expected', 'actual'].reduce(function(prev, val) {
        prev[val] = path.join(__dirname, '..', val, '/', name + '.png');
        return prev;
    }, {});
}

test('fuzzy', function(t) {
    var options = {
        request: function(req, callback) {
            fs.readFile(path.join(__dirname, '..', req.url), function(err, data) {
                callback(err, { data: data });
            });
        }
    };

    var renderOptions = {
        zoom: 15,
        center: [-87.6211, 41.8820]
    };

    // Loads 256px zoom 16 satellite tiles mosaic
    t.test('tileSize: 256', function(t) {
        var map = new mbgl.Map(options);
        map.load(require('../fixtures/satellite.json'));
        map.render(renderOptions, function(err, pixels) {
            t.error(err);
            map.release();

            var png = new PNG({
                width: 512,
                height: 512
            });

            png.data = pixels;

            var name = filename('satellite');
            png.pack()
                .pipe(fs.createWriteStream(process.env.UPDATE ? name.expected : name.actual))
                .on('finish', function(err) {
                    t.error(err);

                    fs.createReadStream(name.expected)
                        .pipe(new PNG())
                        .on('parsed', function() {
                            var diff = pixelmatch(pixels, this.data, null, 512, 512, { threshold: 0.1 });
                            t.equal(diff, 0);

                            t.end();
                        });
                });
        })
    });

    // Loads 256px zoom 15 satellite tiles and rescales to 512px
    t.test('missing tileSize', function(t) {
        var map = new mbgl.Map(options);
        map.load(require('../fixtures/satellite-fuzzy.json'));
        map.render(renderOptions, function(err, pixels) {
            t.error(err);
            map.release();

            var png = new PNG({
                width: 512,
                height: 512
            });

            png.data = pixels;

            var name = filename('satellite-fuzzy');
            png.pack()
                .pipe(fs.createWriteStream(process.env.UPDATE ? name.expected : name.actual))
                .on('finish', function(err) {
                    t.error(err);

                    fs.createReadStream(name.expected)
                        .pipe(new PNG())
                        .on('parsed', function() {
                            var diff = pixelmatch(pixels, this.data, null, 512, 512, { threshold: 0.1 });
                            t.equal(diff, 0);

                            t.end();
                        });
                });
        });
    });
});
