// Converts a PNG image to a custom "image format" that has a uint32_t width/height prefix and then
// raw RGBA data. We can't use the built-in PNG reading routines because they are reading
// premultiplied images by default.

var fs = require('fs');
var zlib = require('zlib');
var PNG = require('png-js');
var png = PNG.load('styles/sprites/bright.png');
png.decodePixels(function(data) {
    var result = new Buffer(8 + data.length);
    result.writeUInt32BE(png.width, 0);
    result.writeUInt32BE(png.height, 4);
    data.copy(result, 8);

    zlib.deflate(result, function(err, data) {
        if (err) throw err;
        fs.writeFileSync('test/fixtures/sprites/bright.bin', data);
    });
});
