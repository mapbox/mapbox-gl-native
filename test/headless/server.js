#!/usr/bin/env node
/* jshint node: true */
'use strict';

var express = require('express');
var app = express();

app.use(express.static('test/suite'));

var server = app.listen(2900, function () {
    var host = server.address().address;
    var port = server.address().port;
    console.log('Test server listening at http://%s:%s', host, port);

    if (process.argv[2]) {
        // Allow the test to continue running.
        process.stdin.write("Go!\n");
    }
});
