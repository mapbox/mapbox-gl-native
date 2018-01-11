// Global functions //

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

global.unhyphenate = function (str) {
 return str.replace(/-/g, " ");
};

global.writeIfModified = function(filename, newContent) {
  try {
    const oldContent = fs.readFileSync(filename, 'utf8');
    if (oldContent == newContent) {
      console.warn(`* Skipping file '${filename}' because it is up-to-date`);
      return;
    }
  } catch(err) {
  }
  if (['0', 'false'].indexOf(process.env.DRY_RUN || '0') !== -1) {
    fs.writeFileSync(filename, newContent);
  }
  console.warn(`* Updating outdated file '${filename}'`);
};
