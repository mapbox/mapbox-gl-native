#!/usr/bin/env node
'use strict';

const fs = require('fs');
const execFileSync = require('child_process').execFileSync;
const _ = require('lodash');

require('../../../scripts/style-code');

const examplesSrc = fs.readFileSync('platform/darwin/test/MGLDocumentationExampleTests.swift', 'utf8');

// Regex extracts the following block
// /** Front matter to describe the example. **/
// func testMGLClass$member() {
//     ...
//     // #-example-code
//     let sampleCode: String?
//     // #-end-example-code
//     ...
// }
//
// into the following regex groups:
// 1 (test method name): "MGLClass" or "MGLClass$member" or "MGLClass$initWithArg_anotherArg_"
// 2 (indentation): "    "
// 3 (sample code): "let sampleCode: String?"
const exampleRegex = /func test([\w$]+)\s*\(\)\s*\{[^]*?\n([ \t]+)\/\/#-example-code\n([^]+?)\n\2\/\/#-end-example-code\n/gm;

/**
 * Returns the given source with example code inserted into the documentation
 * comment for the symbol at the given one-based line.
 *
 * @param {String} src Source code to insert the example code into.
 * @param {Number} line One-based line number of the symbol being documented.
 * @param {String} exampleCode Example code to insert.
 * @returns {String} `src` with `exampleCode` inserted just above `line`.
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

let examples = {};
let match;
while ((match = exampleRegex.exec(examplesSrc)) !== null) {
  let testMethodName = match[1],
      indentation = match[2],
      exampleCode = match[3];

  // Trim leading whitespace from the example code.
  exampleCode = exampleCode.replace(new RegExp('^' + indentation, 'gm'), '');

  examples[testMethodName] = exampleCode;
}

function completeExamples(os) {
  console.log(`Installing ${os} SDK examples…`);

  let sdk = os === 'iOS' ? 'iphonesimulator' : 'macosx';
  let sysroot = execFileSync('xcrun', ['--show-sdk-path', '--sdk', sdk]).toString().trim();
  let umbrellaPath = `platform/${os.toLowerCase()}/src/Mapbox.h`;
  let docArgs = ['doc', '--objc', umbrellaPath, '--',
                 '-x', 'objective-c', '-I', 'platform/darwin/src/', '-isysroot', sysroot];
  let docStr = execFileSync('sourcekitten', docArgs).toString().trim();
  let docJson = JSON.parse(docStr);
  _.forEach(docJson, function (result) {
    _.forEach(result, function (structure, path) {
      let src;
      let newSrc;
      // Recursively search for code blocks in documentation comments and populate
      // them with example code from the test methods. Find and replace the code
      // blocks in reverse to keep the SourceKitten line numbers accurate.
      _.forEachRight(structure['key.substructure'], function completeSubstructure(substructure, idx, substructures, symbolPath) {
        if (!symbolPath) {
          symbolPath = [substructure['key.name']];
        }
        _.forEachRight(substructure['key.substructure'], function (substructure, idx, substructures) {
          completeSubstructure(substructure, idx, substructures, _.concat(symbolPath, substructure['key.name']));
        });

        let comment = substructure['key.doc.comment'];
        if (!comment || !comment.match(/^(?:\s*)```swift\n/m)) {
          return;
        }

        // Lazily read in the existing file.
        if (!src) {
          newSrc = src = fs.readFileSync(path, 'utf8');
        }

        // Get the contents of the test method whose name matches the symbol path.
        let testMethodName = symbolPath.join('$').replace(/\$[+-]/, '$').replace(/:/g, '_');
        let example = examples[testMethodName];
        if (!example) {
          console.error(`MGLDocumentationExampleTests.test${testMethodName}() not found.`);
          process.exit(1);
        }

        // Resolve conditional compilation blocks.
        example = example.replace(/^(\s*)#if\s+os\((iOS|macOS)\)\n([^]*?)(?:^\1#else\n([^]*?))?^\1#endif\b\n?/gm,
                                  function (m, indentation, ifOs, ifCase, elseCase) {
          return (os === ifOs ? ifCase : elseCase).replace(new RegExp('^    ', 'gm'), '');
        }).replace(/\n$/, '');

        // Insert the test method contents into the documentation comment just
        // above the substructure.
        let startLine = substructure['key.parsed_scope.start'];
        newSrc = completeSymbolInSource(newSrc, startLine, example);
      });

      if (!src) {
        return;
      }

      // Write out the modified file contents.
      writeIfModified(path, newSrc)
    });
  });
}

function ensureSourceKittenIsInstalled() {
  try {
    execFileSync('which', ['sourcekitten']);
  } catch (e) {
    console.log(`Installing SourceKitten via Homebrew…`);
    execFileSync('brew', ['install', 'sourcekitten']);
  }
}

ensureSourceKittenIsInstalled();

// Where a particular comment is part of both SDKs, prefer the iOS example.
completeExamples('macOS');
completeExamples('iOS');
