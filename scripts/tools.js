#!/usr/bin/env node

var path = require('path');
var fs = require('fs');

module.exports = {
    // http://stackoverflow.com/a/40686853/2810637
    mkdirRecursive: function(recursiveDir) {
        if (!fs.existsSync(recursiveDir)) {
            recursiveDir.split('/').forEach((dir, index, splits) => {
                const parentDir = splits.slice(0, index).join('/');
                const dirPath = path.resolve(parentDir, dir);
                if (!fs.existsSync(dirPath)) {
                    fs.mkdirSync(dirPath);
                }
            });
        }
    }
};
