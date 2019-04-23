// TODO: support multiple test cases
var testCase = 'background-color/function'

var fs = require('fs'),
    PNG = require('pngjs').PNG,
    pixelmatch = require('pixelmatch');

var img1 = fs.createReadStream('build/outputs/' + testCase + '/expected.png').pipe(new PNG()).on('parsed', doneReading),
    img2 = fs.createReadStream('build/outputs/' + testCase + '/actual.png').pipe(new PNG()).on('parsed', doneReading),
    filesRead = 0;

function doneReading() {
    if (++filesRead < 2) return;
    var diff = new PNG({width: img1.width, height: img1.height});
    var numberMismatchPixels = pixelmatch(img1.data, img2.data, diff.data, img1.width, img1.height, {threshold: 0.1});
    console.log("PixelMatching: " + testCase + ": " + numberMismatchPixels + " mismatched pixels.");
    diff.pack().pipe(fs.createWriteStream('build/outputs/' + testCase + '/diff.png'));
}