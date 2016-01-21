# node-mapbox-gl-native

[![NPM](https://nodei.co/npm/mapbox-gl-native.png)](https://npmjs.org/package/mapbox-gl-native)  
[![Travis](https://travis-ci.org/mapbox/mapbox-gl-native.svg?branch=master)](https://travis-ci.org/mapbox/mapbox-gl-native/builds)

## Installing

Requires a modern C++ runtime that supports C++14.

By default, installs binaries. On these platforms no additional dependencies are needed.

- 64 bit OS X or 64 bit Linux
- Node.js v4+

Just run:

```
npm install mapbox-gl-native
```

Other platforms will fall back to a source compile with `make node`. To compile this module, make sure all submodules are initialized with `git submodule update --init` and install the [external dependencies required to build from source](https://github.com/mapbox/mapbox-gl-native/blob/node-v2.1.0/INSTALL.md#2-installing-dependencies).

## Testing

```
npm test
```

## Rendering a map tile

```js
var map = new mbgl.Map({ request: function() {} });
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
    center: [{longitude}, {latitude}], // array of numbers (coordinates), defaults to [0,0]
    bearing: {bearing}, // number (in degrees, counter-clockwise from north), defaults to 0
    classes: {classes} // array of strings
}
```

When you are finished using a map object, you can call `map.release()` to dispose the internal map resources manually. This is not necessary, but can be helpful to optimize resource usage (memory, file sockets) on a more granualar level than v8's garbage collector.

## Implementing a file source

When creating a `Map`, you must pass an options object (with a required `request` method and optional 'ratio' number) as the first parameter.

```js
var map = new mbgl.Map({
    request: function(req) {
        // TODO
    },
    ratio: 2.0
});
```

The `request()` method starts a new request to a file. The `ratio` sets the scale at which the map will render tiles, such as `2.0` for rendering images for high pixel density displays. The `req` parameter has two properties:

```json
{
    "url": "http://example.com",
    "kind": 1
}
```

The `kind` is an enum and defined in [`mbgl.Resource`](https://github.com/mapbox/mapbox-gl-native/blob/node/include/mbgl/storage/resource.hpp):

```json
{
    "Unknown": 0,
    "Style": 1,
    "Source": 2,
    "Tile": 3,
    "Glyphs": 4,
    "SpriteImage": 5,
    "SpriteJSON": 6
}
```

It has no significance for anything but serves as a hint to your implemention as to what sort of resource to expect. E.g., your implementation could choose caching strategies based on the expected file type.

The `request` implementation should pass uncompressed data to `callback`. If you are downloading assets from a source that applies gzip transport encoding, the implementation must decompress the results before passing them on.

A sample implementation that reads files from disk would look like the following:

```js
var map = new mbgl.Map({
    request: function(req, callback) {
        fs.readFile(path.join('base/path', req.url), function(err, data) {
            callback(err, { data: data });
        });
    }
});
```

This is a very barebones implementation and you'll probably want a better implementation. E.g. it passes the url verbatim to the file system, but you'd want add some logic that normalizes `http` URLs. You'll notice that once your implementation has obtained the requested file, you have to deliver it to the requestee by calling `callback()`, which takes either an error object or `null` and an object with several settings:

```js
{
    modified: new Date(),
    expires: new Date(),
    etag: "string",
    data: new Buffer()
};
```

A sample implementation that uses [`request`](https://github.com/request/request) to query data from HTTP:

```js
var mbgl = require('mapbox-gl-native');
var request = require('request');

var map = new mbgl.Map({
    request: function(req, callback) {
        request({
            url: req.url,
            encoding: null,
            gzip: true
        }, function (err, res, body) {
            if (err) {
                callback(err);
            } else if (res.statusCode == 200) {
                var response = {};

                if (res.headers.modified) { response.modified = new Date(res.headers.modified); }
                if (res.headers.expires) { response.expires = new Date(res.headers.expires); }
                if (res.headers.etag) { response.etag = res.headers.etag; }
                
                response.data = body;
                
                callback(null, response);
            } else {
                callback(new Error(JSON.parse(body).message));
            }
        });
    }
});
```

Mapbox GL uses two types of protocols: `asset://` for files that should be loaded from some local static system, and `http://` (and `https://`), which should be loaded from the internet. However, stylesheets are free to use other protocols too, if your implementation of `request` supports these; e.g. you could use `s3://` to indicate that files are supposed to be loaded from S3.

## Listening for log events

The module imported with `require('mapbox-gl-native')` inherits from [`EventEmitter`](https://nodejs.org/api/events.html), and the `NodeLogObserver` will push log events to this. Log messages can have [`class`](https://github.com/mapbox/mapbox-gl-native/blob/node-v2.1.0/include/mbgl/platform/event.hpp#L43-L60), [`severity`](https://github.com/mapbox/mapbox-gl-native/blob/node-v2.1.0/include/mbgl/platform/event.hpp#L17-L23), `code` ([HTTP status codes](http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html)), and `text` parameters.

```
MBGL_DEFINE_ENUM_CLASS(EventClass, Event, {
    { Event::General, "General" },
    { Event::Setup, "Setup" },
    { Event::Shader, "Shader" },
    { Event::ParseStyle, "ParseStyle" },
    { Event::ParseTile, "ParseTile" },
    { Event::Render, "Render" },
    { Event::Style, "Style" },
    { Event::Database, "Database" },
    { Event::HttpRequest, "HttpRequest" },
    { Event::Sprite, "Sprite" },
    { Event::Image, "Image" },
    { Event::OpenGL, "OpenGL" },
    { Event::JNI, "JNI" },
    { Event::Android, "Android" },
    { Event::Crash, "Crash" },
    { Event(-1), "Unknown" },
});
```

```
MBGL_DEFINE_ENUM_CLASS(EventSeverityClass, EventSeverity, {
    { EventSeverity::Debug, "DEBUG" },
    { EventSeverity::Info, "INFO" },
    { EventSeverity::Warning, "WARNING" },
    { EventSeverity::Error, "ERROR" },
    { EventSeverity(-1), "UNKNOWN" },
});
```

```js
var mbgl = require('mapbox-gl-native');
mbgl.on('message', function(msg) {
    t.ok(msg, 'emits error');
    t.equal(msg.class, 'Style');
    t.equal(msg.severity, 'ERROR');
    t.ok(msg.text.match(/Failed to load/), 'error text matches');
});
```

## Mapbox API Access tokens

To use styles that rely on Mapbox vector tiles, you must pass an [API access token](https://www.mapbox.com/developers/api/#access-tokens) in your `request` implementation with requests to `mapbox://` protocols.

```js
var mbgl = require('mapbox-gl-native');
var request = require('request');
var url = require('url');

var map = new mbgl.Map({
    request: function(req, callback) {
        var opts = {
            url: req.url,
            encoding: null,
            gzip: true
        };

        if (url.parse(req.url).protocol === 'mapbox:') {
            opts.qs = { access_token: process.env.MAPBOX_ACCESS_TOKEN};
        }

        request(opts, function (err, res, body) {
            if (err) {
                callback(err);
            } else if (res.statusCode == 200) {
                var response = {};

                if (res.headers.modified) { response.modified = new Date(res.headers.modified); }
                if (res.headers.expires) { response.expires = new Date(res.headers.expires); }
                if (res.headers.etag) { response.etag = res.headers.etag; }
            
                response.data = body;
            
                callback(null, response);
            } else {
                callback(new Error(JSON.parse(body).message));
            }
        });
    }
});

// includes a datasource with a reference to something like `mapbox://mapbox.mapbox-streets-v6`
var style = mapboxStyle;

map.load(style);
map.render({}, function(err, image) {
    if (err) throw err;
    fs.writeFileSync('image.png', image);
});

```

## Contributing

See [DEVELOPING.md](DEVELOPING.md) for instructions on building this module for development.
