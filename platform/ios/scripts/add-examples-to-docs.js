'use strict';

const fs = require('fs');
const execFileSync = require('child_process').execFileSync;
const _ = require('lodash');

const examples = fs.readFileSync('platform/ios/test/MGLDocumentationExampleTests.swift', 'utf8');

// Regex extracts the following block
// /** Front matter to describe the example. **/
// func testMGLClass$member() {
//     ...
//     //#-example-code
//     let sampleCode: String?
//     //#-end-example-code
//     ...
// }
//
// into the following regex groups:
// 1 (class name): "MGLClass"
// 2 (member name): "member"
// 3 (indentation): "    "
// 4 (sample code): "let sampleCode: String?"
const exampleRegex = /func test(\w+)(?:\$(\w+))?\s*\(\)\s*\{[^]*?\n([ \t]+)\/\/#-example-code\n([^]+?)\n\3\/\/#-end-example-code\n/gm;

console.log("Installing examples...");

let match;
while ((match = exampleRegex.exec(examples)) !== null) {
  let className = match[1],
      memberName = match[2],
      indentation = match[3],
      exampleCode = match[4];

  // Trim leading whitespace from the example code.
  exampleCode = exampleCode.replace(new RegExp('^' + indentation, 'gm'), '');

  // check if file exists at path
  let path = `platform/darwin/src/${className}.h`;
  if (!fs.existsSync(path)) {
    path = `platform/ios/src/${className}.h`;
  }

  if (fs.existsSync(path)) {
    let src = fs.readFileSync(path, 'utf8');
    
    // Use SourceKitten to find the class or class member named by the test method.
    let docStr = execFileSync('sourcekitten', ['doc', '--objc', path, '--', '-x', 'objective-c']).toString().trim();
    let docJson = JSON.parse(docStr);
    let fileStructure = _.find(docJson, fileStructure => path in fileStructure)[path];
    let substructure = fileStructure['key.substructure'];
    substructure = _.find(substructure, decl => decl['key.name'] === className);
    if (memberName) {
      substructure = _.find(substructure['key.substructure'], decl => decl['key.name'] === memberName);
    }
    
    // Split the file contents right before the symbol declaration (but after its documentation comment).
    let srcUpToSymbol = src.split('\n', substructure['key.parsed_scope.start'] - 1).join('\n');
    let srcFromSymbol = src.substr(srcUpToSymbol.length);
    
    // Match the documentation comment block that is not followed by the beginning or end of a declaration.
    let commentMatch = srcUpToSymbol.match(/\/\*\*\s*(?:[^*]|\*(?!\/))+?\s*\*\/[^;{}]*?$/);
    
    // Replace the Swift code block with the test method’s contents.
    let completedComment = commentMatch[0].replace(/^([ \t]*)```swift\n[^]*?```/m, function (m, indentation) {
      // Apply the original indentation to each line.
      return ('```swift\n' + exampleCode + '\n```').replace(/^/gm, indentation);
    });
    
    // Splice the modified comment into the overall file contents.
    srcUpToSymbol = (srcUpToSymbol.substr(0, commentMatch.index) + completedComment +
                     srcUpToSymbol.substr(commentMatch.index + commentMatch[0].length));
    let newSrc = srcUpToSymbol + srcFromSymbol;
    
    // Write out the modified file contents.
    if (src === newSrc) {
      console.log('Skipping', path);
    } else {
      console.log("Updating", path);
      fs.writeFileSync(path, newSrc);
    }
  } else {
    console.log("error: File doesn't exist:", path);
  }
}
