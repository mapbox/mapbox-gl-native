#!/usr/bin/env node
/* jshint node: true */
'use strict';

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


var cacheCounter = 0;
app.get('/cache', function(req, res) {
    res.setHeader('Cache-Control', 'max-age=30'); // Allow caching for 30 seconds
    res.send('Response ' + (++cacheCounter));
});

app.get('/revalidate-same', function(req, res) {
    if (req.headers['if-none-match'] == 'snowfall') {
        // Second request can be cached for 30 seconds.
        res.setHeader('Cache-Control', 'max-age=30');
        res.status(304).end();
    } else {
        // First request must always be revalidated.
        res.setHeader('ETag', 'snowfall');
        res.setHeader('Cache-Control', 'must-revalidate');
        res.status(200).send('Response');
    }
});


app.get('/revalidate-modified', function(req, res) {
    var jan1 = new Date('jan 1 2015 utc');

    if (req.headers['if-modified-since']) {
        var modified_since = new Date(req.headers['if-modified-since']);
        if (modified_since >= jan1) {
            res.setHeader('Cache-Control', 'max-age=30');
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


var temporaryErrorCounter = 0;
app.get('/temporary-error', function(req, res) {
    if (temporaryErrorCounter === 0) {
        res.status(500).end();
    } else {
        res.status(200).send('Hello World!');
    }

    temporaryErrorCounter++;
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
    var host = server.address().address;
    var port = server.address().port;
    console.warn('Storage test server listening at http://%s:%s', host, port);

    if (process.argv[2]) {
        // Allow the test to continue running.
        process.stdin.write("Go!\n");
    }
});
