var fs = require('fs');

module.exports = function(file, content) {
    try {
        var existing = fs.readFileSync(file, 'utf8');
        if (existing != content) {
            fs.writeFileSync(file, content);
        }
    } catch(err) {
        fs.writeFileSync(file, content);
    }
};
