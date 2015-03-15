# node-mapbox-gl-native

Renders map tiles with [Mapbox GL](https://github.com/mapbox/mapbox-gl-native).

[![Build Status](https://travis-ci.org/mapbox/node-mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/node-mapbox-gl-native)

## Installing

By default, installs binaries. On these platforms no external dependencies are needed.

- 64 bit OS X or 64 bit Linux
- Node v0.10.x

Just run:

```
npm install mapbox-gl-native
```

However, other platforms will fall back to a source compile with `make build`. To compile this module, make sure all submodules are initialized.

## Rendering a map tile

```js
var map = new mbgl.Map(fileSource);
map.load(require('./test/fixtures/style.json'));
map.render({}, function(err, image) {
    if (err) throw err;
    fs.writeFileSync('image.png', image);
});
```

The first argument passed to `map.render` is an options object, all keys are optional:

```js
{
    zoom: {zoom}, // number, defaults to 0
    width: {width}, // number (px), defaults to 512
    height: {height}, // number (px), defaults to 512
    center: [{latitude}, {longitude}], // array of numbers (coordinates), defaults to [0,0]
    bearing: {bearing}, // number (in degrees, counter-clockwise from north), defaults to 0
    ratio: {ratio} // number (scale factor), defaults to 1.0,
    classes: {classes} // array of strings, optional
}
```

_More about classes: https://github.com/mapbox/mapbox-gl-js/blob/master/API.md#working-with-style-classes_

## Testing

```
npm test
```

## Implementing a file source

When creating a `Map` object, you need to pass a `FileSource` object as the first parameter

```js
var request = require('request');

var fileSource = new mbgl.FileSource();
fileSource.request = function(req) {
    // TODO
};

fileSource.cancel = function(req) {
    // TODO
};
```

The `request()` method starts a new request to a file, while `cancel()` tells your FileSource implementation to cancel the request (if possible). The `req` parameter has two properties:

```json
{
    "url": "http://example.com",
    "kind": 1
}
```

The `kind` is an enum and defined in `mbgl.Resource`:

```json
{
    "Unknown": 0,
    "Tile": 1,
    "Glyphs": 2,
    "Image": 3,
    "JSON": 4
}
```

It has no significance for anything but serves as a hint to your implemention as to what sort of resource to expect. E.g., your implementation could choose caching strategies based on the expected file type.

A sample implementation that reads files from disk would look like the following:

```js
fileSource.request = function(req) {
    fs.readFile(path.join('base/path', req.url), function(err, data) {
        req.respond(err, { data: data });
    });
};
fileSource.cancel = function() {};
```

This is a very barebones implementation and you'll probably want a better implementation. E.g. it passes the url verbatim to the file system, but you'd want add some logic that normalizes `http` URLs. You'll notice that once your implementation has obtained the requested file, you have to deliver it to the requestee by calling `req.respond()`, which takes either an error object or `null` and an object with several settings:

```js
{
    modified: new Date(),
    expires: new Date(),
    etag: "string",
    data: new Buffer(),
};
```

A sample implementation that uses [`request`](https://github.com/request/request) to query data from HTTP:

```js
var mbgl = require('mapbox-gl-native');
var request = require('request');

var fileSource = new mbgl.FileSource();
fileSource.request = function(req) {
    request({
        url: req.url,
        encoding: null,
        gzip: true
    }, function (err, res, body) {
        if (req.canceled) {
            return;
        }

        if (err) {
            req.respond(err);
        } else if (res.statusCode == 200) {
            var response = {};

            if (res.headers.modified) { response.modified = new Date(res.headers.modified); }
            if (res.headers.expires) { response.expires = new Date(res.headers.expires); }
            if (res.headers.etag) { response.etag = res.headers.etag; }
            response.data = body;
            req.respond(null, response);
        } else {
            console.warn(new Date(res.headers.expires));
        }
    });
};

fileSource.cancel = function(req) {
    req.canceled = true;
};
```

Note that in reality, Mapbox GL uses two types of protocols: `asset://` for files that should be loaded from some local static system, and `http://` (and `https://`), which should be loaded from the internet. However, stylesheets are free to use other protocols too, if your implementation of FileSource supports these; e.g. you could use `s3://` to indicate that files are supposed to be loaded from S3.

## Mapbox API Access tokens

To use styles that rely on Mapbox vector tiles, you must register a [API access token](https://www.mapbox.com/developers/api/#access-tokens) on the map. Use the `setAccessToken` method before load:

```js
var map = mbgl.Map(fileSource);
var style = mapboxStyle; // includes a datasource with a reference to something like `mapbox://mapbox.mapbox-streets-v6`
map.setAccessToken(process.env.MAPBOX_ACCESS_TOKEN);
map.load(style);
map.render({}, function(err, image) {
    if (err) throw err;
    fs.writeFileSync('image.png', image);
});

```

