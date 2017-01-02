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
// 2 (member name): "member" or "initWithArg_anotherArg_"
// 3 (indentation): "    "
// 4 (sample code): "let sampleCode: String?"
const exampleRegex = /func test(\w+)(?:\$(\w+))?\s*\(\)\s*\{[^]*?\n([ \t]+)\/\/#-example-code\n([^]+?)\n\3\/\/#-end-example-code\n/gm;

console.log("Installing examples...");

let sysroot = execFileSync('xcrun', ['--show-sdk-path', '--sdk', 'iphonesimulator']).toString().trim();
let docStr = execFileSync('sourcekitten', ['doc', '--objc', 'platform/ios/src/Mapbox.h', '--', '-x', 'objective-c', '-I', 'platform/darwin/src/', '-isysroot', sysroot]).toString().trim();
let docJson = JSON.parse(docStr);

/**
 * Returns the one-based line number of a symbol in the file at the given path.
 */
function getLineForSymbol(path, className, memberName) {
  let fileStructure = _.find(docJson, fileStructure => path in fileStructure)[path];
  let substructure = fileStructure['key.substructure'];
  substructure = _.find(substructure, decl => decl['key.name'] === className);
  if (memberName) {
    substructure = _.find(substructure['key.substructure'] || [], decl => decl['key.name'].match(memberName));
  }
  if (!substructure) {
    console.log(`error: ${className} not found in ${path}`);
    process.exit(1);
  }
  return substructure['key.parsed_scope.start'];
}

/**
 * Returns the given source with example code inserted into the documentation
 * comment for the symbol at the given line.
 */
function completeSymbolInSource(src, line, exampleCode) {
  // Split the file contents right before the symbol declaration (but after its documentation comment).
  let srcUpToSymbol = src.split('\n', line - 1).join('\n');
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
  return srcUpToSymbol + srcFromSymbol;
}

/**
 * Edits the file at the given path to include the given example code.
 */
function completeSymbolInFile(path, className, memberName, exampleCode) {
  // Find the class or class member named by the test method.
  let startLine = getLineForSymbol(path, className, memberName);
  
  // Update the documentation comment for the class or class member.
  let src = fs.readFileSync(path, 'utf8');
  let newSrc = completeSymbolInSource(src, startLine, exampleCode);
  
  // Write out the modified file contents.
  if (src === newSrc) {
    console.log('Skipping', path);
  } else {
    console.log("Updating", path);
    fs.writeFileSync(path, newSrc);
  }
}

let match;
while ((match = exampleRegex.exec(examples)) !== null) {
  let className = match[1],
      memberName = match[2],
      indentation = match[3],
      exampleCode = match[4];

  // Trim leading whitespace from the example code.
  exampleCode = exampleCode.replace(new RegExp('^' + indentation, 'gm'), '');
  
  if (memberName) {
    memberName = memberName.replace(/(.)_/g, '$1:');
    memberName = new RegExp(`^[-+]?${memberName}$`);
  }

  // check if file exists at path
  let path = `platform/darwin/src/${className}.h`;
  if (fs.existsSync(path)) {
    completeSymbolInFile(path, className, memberName, exampleCode);
    continue;
  }
  
  path = `platform/ios/src/${className}.h`;
  if (fs.existsSync(path)) {
    completeSymbolInFile(path, className, memberName, exampleCode);
    continue;
  }
  
  console.log("error: File doesn't exist:", path);
  process.exit(1);
}
