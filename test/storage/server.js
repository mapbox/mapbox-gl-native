#!/usr/bin/env node
/* jshint node: true */
'use strict';

// This needs to be here to make sure the pipe stays open.
// We're waiting until the stdin pipe gets closed (e.g. because the parent
// process dies)
process.stdin.on('readable', function() {});
process.stdin.on('end', function() { process.exit(0); });


var fs = require('fs');
var express = require('express');
var app = express();

// We're manually setting Etag headers.
app.disable('etag');

app.get('/test', function (req, res) {
    if (req.query.modified) {
        res.setHeader('Last-Modified', (new Date(req.query.modified * 1000)).toUTCString());
    }
    if (req.query.expires) {
        res.setHeader('Expires', (new Date(req.query.expires * 1000)).toUTCString());
    }
    if (req.query.etag) {
        res.setHeader('ETag', req.query.etag);
    }
    if (req.query.cachecontrol) {
        res.setHeader('Cache-Control', req.query.cachecontrol);
    }
    res.send('Hello World!');
});

app.get('/stale/*', function() {
    // Never respond.
});

var cacheCounter = 0;
app.get('/cache', function(req, res) {
    res.setHeader('Cache-Control', 'max-age=30'); // Allow caching for 30 seconds
    res.send('Response ' + (++cacheCounter));
});

app.get('/revalidate-same', function(req, res) {
    if (req.headers['if-none-match'] == 'snowfall') {
        // Second request can be cached for 1 second.
        res.setHeader('Cache-Control', 'max-age=1, must-revalidate');
        res.status(304).end();
    } else {
        // First request must always be revalidated.
        res.setHeader('ETag', 'snowfall');
        res.setHeader('Cache-Control', 'must-revalidate');
        res.status(200).send('Response');
    }
});

var expiresCounter = 0;
app.get('/clockskew', function (req, res) {
    res.setHeader('Expires', (new Date(2010, 1, 1, 10, ++expiresCounter, 0)).toUTCString());
    res.status(200).send('Response');
});

app.get('/revalidate-modified', function(req, res) {
    var jan1 = new Date('jan 1 2015 utc');

    if (req.headers['if-modified-since']) {
        var modified_since = new Date(req.headers['if-modified-since']);
        if (modified_since >= jan1) {
            res.setHeader('Cache-Control', 'max-age=1, must-revalidate');
            res.status(304).end();
            return;
        }
    }

    // First request must always be revalidated.
    res.setHeader('Last-Modified', jan1.toUTCString());
    res.setHeader('Cache-Control', 'must-revalidate');
    res.status(200).send('Response');
});


var revalidateEtagCounter = 1;
app.get('/revalidate-etag', function(req, res) {
    res.setHeader('ETag', 'response-' + revalidateEtagCounter);
    res.setHeader('Cache-Control', 'must-revalidate');

    res.status(200).send('Response ' + revalidateEtagCounter);
    revalidateEtagCounter++;
});

app.get('/empty-data', function(req, res) {
    res.status(200).send();
});

app.get('/no-content', function(req, res) {
    res.status(204).send();
});

app.get('/not-found', function(req, res) {
    res.status(404).send('Not Found!');
});

app.get('/permanent-error', function(req, res) {
    res.status(500).send('Server Error!');
});

var temporaryErrorCounter = 0;
app.get('/temporary-error', function(req, res) {
    if (temporaryErrorCounter === 0) {
        res.status(500).end();
    } else {
        res.status(200).send('Hello World!');
    }

    temporaryErrorCounter++;
});

app.get('/rate-limit', function(req, res) {

    if (req.query.std) {
        res.setHeader('Retry-After', 1);
    } else if (req.query.mbx) {
        res.setHeader('x-rate-limit-reset', Math.round(Date.now() / 1000) + 1);
    }

    res.status(429).end();
});

var styleFailOnce500 = true;
app.get('/style-fail-once-500', function (req, res) {
    if (styleFailOnce500) {
        res.status(500).send('Server Error!');
        styleFailOnce500 = false;
    } else {
        res.status(200).send('{ "version": 8, "name": "Teste Style" }');
    }
});

var styleFailOnce404 = true;
app.get('/style-fail-once-404', function (req, res) {
    if (styleFailOnce404) {
        res.status(404).send('Not found!');
        styleFailOnce404 = false;
    } else {
        res.status(200).send('{ "version": 8, "name": "Teste Style" }');
    }
});

var styleFailOnce404Cache = true;
app.get('/style-fail-once-404-cache', function (req, res) {
    if (styleFailOnce404Cache) {
        res.setHeader('Cache-Control', 'max-age=30');
        res.status(404).send('Not found!');
        styleFailOnce404Cache = false;
    } else {
        res.status(200).send('{ "version": 8, "name": "Teste Style" }');
    }
});

app.get('/delayed', function(req, res) {
    setTimeout(function() {
        res.status(200).send('Response');
    }, 200);
});


app.get('/load/:number(\\d+)', function(req, res) {
    res.send('Request ' + req.params.number);
});

var server = app.listen(3000, function () {
    // Tell parent that we're now listening.
    process.stdout.write("OK");
});
