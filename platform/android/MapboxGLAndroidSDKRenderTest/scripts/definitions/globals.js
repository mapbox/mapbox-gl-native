import fs from 'fs';
import path from 'path';

// Write out a list of files that this script is modifying so that we can check
var files = [];
process.on('exit', function() {
    const list = path.join(path.dirname(process.argv[1]), path.basename(process.argv[1], '.js') + '.list');
    fs.writeFileSync(list, files.join('\n'));
});

export function writeIfModified(filename, newContent) {
    ensureDirectoryExistence(filename);
    files.push(filename);
    try {
        const oldContent = fs.readFileSync(filename, 'utf8');
        if (oldContent == newContent) {
            console.warn(`* Skipping file '${filename}' because it is up-to-date`);
            return;
        }
    } catch(err) {
        console.log(err);
    }
    
    if (['0', 'false'].indexOf(process.env.DRY_RUN || '0') !== -1) {
        fs.writeFileSync(filename, newContent);
    }
    console.warn(`* Updating outdated file '${filename}'`);
}

function ensureDirectoryExistence(filePath) {
  var dirname = path.dirname(filePath);
  if (fs.existsSync(dirname)) {
    return true;
  }
  ensureDirectoryExistence(dirname);
  fs.mkdirSync(dirname);
}