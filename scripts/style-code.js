// Global functions //

'use strict';

const fs = require('fs');

global.iff = function (condition, val) {
  return condition() ? val : "";
};

global.camelize = function (str) {
  return str.replace(/(?:^|-)(.)/g, function (_, x) {
    return x.toUpperCase();
  });
};

global.camelizeWithLeadingLowercase = function (str) {
  return str.replace(/-(.)/g, function (_, x) {
    return x.toUpperCase();
  });
};

global.snakeCaseUpper = function snakeCaseUpper(str) {
  return str.replace(/-/g, "_").toUpperCase();
};

global.writeIfModified = function(filename, newContent) {
  try {
    const oldContent = fs.readFileSync(filename, 'utf8');
    if (oldContent == newContent) {
      console.warn(`* Skipping current file '${filename}'`);
      return;
    }
  } catch(err) {
  }
  fs.writeFileSync(filename, newContent);
  console.warn(`* Updating outdated file '${filename}'`);
};

String.prototype.wrap = function (cols, indent, prefix) {
  if (!prefix) prefix = '';
  let wrapRe = new RegExp(`(.{1,${cols - indent}})(?: +|\n|$)`, "gm");

  return this.replace(/\{@link /g, "{@link:")
             .replace(wrapRe, prefix + "$1\n")
             .replace(/\{@link:/g, "{@link ")
             .replace(/\s+$/, "")
             .indent(indent);
};

String.prototype.indent = function (cols) {
  return this.replace(/^|\n/g, "$&" + " ".repeat(cols)).replace(/(^|\n)\s+(?=\n)/g, "$1");
};
