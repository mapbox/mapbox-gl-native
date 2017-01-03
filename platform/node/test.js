var fs = require('fs');
var path = require('path');
var mbgl = require('./index.js');
var sharp = require('sharp');

var options = {
  request: function(req, callback) {
    fs.readFile(path.join(__dirname, 'test', req.url), function(err, data) {
      callback(err, { data: data });
    });
  },
  ratio: 1
};

var map = new mbgl.Map(options);

map.load(require('./test/fixtures/style.json'));

map.render({
  zoom: 0,
  bearing: 20,
  annotations: [{
    latitude: 0,
    longitude: 0,
    imageBuffer: new Buffer()
  }]
}, function(err, buffer) {
    if (err) throw err;

    map.release();

    var image = sharp(buffer, {
        raw: {
            width: 512,
            height: 512,
            channels: 4
        }
    });

    // Convert raw image buffer to PNG
    image.toFile('image.png', function(err) {
        if (err) throw err;
    });
});
