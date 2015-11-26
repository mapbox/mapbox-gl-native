var readJSON = require('read-package-json');

readJSON('package.json', function(err, data) {
    if (err) process.stderr.write(err);
    process.stdout.write(data.version);
});
