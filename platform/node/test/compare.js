var spawn = require('child_process').spawn;

module.exports = function compare(actual, expected, diff, t, callback) {
    var compare = spawn('compare', ['-metric', 'MAE', actual, expected, diff]);
    var error = '';

    compare.stderr.on('data', function(data) {
        error += data.toString();
    });

    compare.on('error', function(err) {
        t.error(err);
    });

    compare.on('exit', function(code) {
        // The compare program returns 2 on error otherwise 0 if the images are similar or 1 if they are dissimilar.
        if (code === 2) {
            callback(error.trim(), Infinity);
        } else {
            var match = error.match(/^\d+(?:\.\d+)?\s+\(([^\)]+)\)\s*$/);
            var difference = match ? parseFloat(match[1]) : Infinity;
            callback(match ? '' : error, difference);
        }
    });

    compare.stdin.end();
};
