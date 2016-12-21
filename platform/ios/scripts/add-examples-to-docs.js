'use strict';

const fs = require('fs');

const examples = fs.readFileSync(`${__dirname}/../test/MGLDocumentationExampleTests.swift`, 'utf8');

// Regex extracts the following block
// /*---BEGIN EXAMPLE: MGLStyleSource---*/
// /* Frontmatter to describe the example */
// let sampleCode: String?
// /*---END EXAMPLE---*/
//
// into the following regex groups:
// 1 (token): " MGLStyleSource"
// 2 (frontmatter): "/* Frontmatter to describe the example */"
// 3 (sample code): "let sampleCode: String?"
const exampleRegex = /\/\*---BEGIN EXAMPLE:(.*)---\*\/\s*(\/\*+[\s\S]*?\*+\/)?([\s\S]*?)\/\*---END EXAMPLE---\*\//gm;

var path = `${process.env.TARGET_BUILD_DIR}/${process.env.PUBLIC_HEADERS_FOLDER_PATH}`;

console.log("Installing examples...");

var match;
while ((match = exampleRegex.exec(examples)) !== null) {
  const token = match[1].trim();
  const className = token.split('.')[0];

  const frontmatter = (match[2] || '')
    .replace(/\/\*+/g, '') // Remove block comment /**
    .replace(/\*+\//g, '') // Remove block comment end */
    .trim()
    .replace(/\n {8,9}/g, '\n'); // Remove leading whitespace (8-9 spaces incl block comment)

  const exampleCode = match[3]
    .trim()
    .replace(/\n {8}/g, '\n'); // Remove leading whitespace (8 spaces)

  // Generate example text
  var exampleText = "### Example\n\n";
  if (frontmatter.length > 0) {
    exampleText += `${frontmatter}\n\n`;
  }
  exampleText += "```swift\n" + exampleCode + "\n```";
  exampleText = exampleText.replace(/\n/g, '\n ');

  const placeholderRegex = new RegExp(`<!--EXAMPLE: ${token}-->`);

  // check if file exists at path
  const filename = `${path}/${className}.h`;

  if (fs.existsSync(filename)) {
    const file = fs.readFileSync(filename, 'utf8');
    // Check for example placeholder in file & update file if found
    if (placeholderRegex.test(file)) {
      console.log("Updating example:", filename);
      fs.writeFileSync(filename, file.replace(placeholderRegex, exampleText));
    } else if (file.indexOf(exampleText) === -1) {
      console.log(`Placeholder "${token}" missing:`, filename);
    } else {
      console.log(`Example "${token}" already replaced.`);
    }
  } else if (token !== "ExampleToken") {
    console.log("Error file doesn't exist:", filename);
  }
}
