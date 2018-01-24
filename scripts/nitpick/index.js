var failed = false;

process.on('exit', function() {
    if (failed) {
        process.exitCode = 1;
    }
});

module.exports = function(status, str, desc) {
    if (status) {
        console.warn(`\x1b[1m\x1b[32m✔︎ ${str}\x1b[0m`);
    } else {
        console.warn(`\x1b[1m\x1b[31m✘ ${str}\x1b[0m`);
        failed = true;
    }

    if (desc) {
        console.warn(`${desc.split('\n').map((line) => '|   ' + line).join('\n')}\n`);
    }
};

module.exports.ok = function(str, desc) {
    module.exports(true, str, desc);
};

module.exports.fail = function(str, desc) {
    module.exports(false, str, desc);
};
