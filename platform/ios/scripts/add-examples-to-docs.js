'use strict';

const fs = require('fs');

const examples = fs.readFileSync(process.env.SCRIPT_INPUT_FILE_0, 'utf8');

// Regex extracts the following block
// /** Front matter to describe the example. **/
// func testMGLClass$method() {
//     ...
//     //#-example-code
//     let sampleCode: String?
//     //#-end-example-code
//     ...
// }
//
// into the following regex groups:
// 1 (front matter): "Front matter to describe the example."
// 2 (class name): "MGLClass"
// 3 (method name): "method"
// 4 (indentation): "    "
// 5 (sample code): "let sampleCode: String?"
const exampleRegex = /(?:\/\*\*\s*((?:[^*]|\*(?=\/))+?)\s*\*\/\s*)?func test(\w+)(?:\$(\w+))?\s*\(\)\s*\{[^]*?\n([ \t]+)\/\/#-example-code\n([^]+?)\n\4\/\/#-end-example-code\n/gm;

let path = `${process.env.TARGET_BUILD_DIR}/${process.env.PUBLIC_HEADERS_FOLDER_PATH}`;

console.log("Installing examples...");

let match;
while ((match = exampleRegex.exec(examples)) !== null) {
  let frontmatter = match[1],
      className = match[2],
      token = match[3] ? `${className}.${match[3]}` : className,
      indentation = match[4],
      exampleCode = match[5];

  // Remove leading whitespace from front matter and example code.
  frontmatter = frontmatter && frontmatter.replace(/^\s+/gm, '');
  exampleCode = exampleCode.replace(new RegExp('^' + indentation, 'gm'), '');

  // Generate example text
  var exampleText = "### Example\n\n";
  if (frontmatter) {
    exampleText += `${frontmatter}\n\n`;
  }
  exampleText += '```swift\n' + exampleCode + '\n```';

  const placeholderRegex = new RegExp(`^([ \\t]*)<!--EXAMPLE: ${token.replace(/\./g, '\\.')}-->`, 'm');

  // check if file exists at path
  const filename = `${path}/${className}.h`;

  if (fs.existsSync(filename)) {
    const file = fs.readFileSync(filename, 'utf8');
    // Check for example placeholder in file & update file if found
    if (placeholderRegex.test(file)) {
      console.log("Updating example:", filename);
      fs.writeFileSync(filename, file.replace(placeholderRegex, function (m, indentation) {
        return exampleText.replace(/^/gm, indentation);
      }));
    } else if (file.replace(/^\s+/gm, '').indexOf(exampleText.replace(/^\s+/gm, '')) === -1) {
      console.log(`error: Placeholder "${token}" missing:`, filename);
    } else {
      console.log(`Example "${token}" already replaced.`);
    }
  } else if (token !== "ExampleToken") {
    console.log("error: Error file doesn't exist:", filename);
  }
}
