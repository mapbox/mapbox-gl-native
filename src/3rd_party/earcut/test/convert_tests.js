'use strict';
/* jshint node: true */

var fs = require('fs');
var path = require('path');

var hpp = '// This file is auto-generated, manual changes will be lost if the code is regenerated.\n\n';
hpp += '#pragma once\n\n';
hpp += '#include <vector>\n';
hpp += '#include <map>\n';
hpp += '#include <string>\n';
hpp += '#include <utility>\n\n';

hpp += '#ifdef _MSC_VER // msvc has trouble optimizing large inline std::vectors, so we disable the optimization\n';
hpp += '# define MAPBOX_MSVC_DISABLE_OPTIMIZATION() __pragma(optimize("", off))\n';
hpp += '# define MAPBOX_MSVC_ENABLE_OPTIMIZATION() __pragma(optimize("", on))\n';
hpp += '#else\n';
hpp += '# define MAPBOX_MSVC_DISABLE_OPTIMIZATION()\n';
hpp += '# define MAPBOX_MSVC_ENABLE_OPTIMIZATION()\n';
hpp += '#endif\n\n';

hpp += 'namespace mapbox {\n';
hpp += 'namespace fixtures {\n\n';

hpp += 'template <typename T> using Polygon = std::vector<std::vector<T>>;\n';
hpp += 'template <typename T> using Triangles = std::vector<T>;\n\n';

hpp += 'using ShortPoint = std::pair<short, short>;\n';
hpp += 'using ShortTriangles = Triangles<ShortPoint>;\n';
hpp += 'using ShortPolygon = Polygon<ShortPoint>;\n\n';

hpp += 'using IntegerPoint = std::pair<int, int>;\n';
hpp += 'using IntegerTriangles = Triangles<IntegerPoint>;\n';
hpp += 'using IntegerPolygon = Polygon<IntegerPoint>;\n\n';

hpp += 'using DoublePoint = std::pair<double, double>;\n';
hpp += 'using DoubleTriangles = Triangles<DoublePoint>;\n';
hpp += 'using DoublePolygon = Polygon<DoublePoint>;\n\n\n';

hpp += 'extern const ShortPolygon park;\n';

var integerPolygons = '';
var doublePolygons = '';

var base = '../earcut/test/fixtures';
fs.readdirSync(base).filter(function (name) {
    return path.extname(name) === '.json';
}).forEach(function (name) {
    var json = JSON.parse(fs.readFileSync(path.join(base, name), 'utf-8'));

    var id = path.basename(name, path.extname(name)).replace(/[^a-z0-9]+/g, '_');

    var integer = true;

    function processPoint(p) {
        if (integer && (p[0] % 1 !== 0 || p[1] % 1 !== 0)) {
            integer = false;
        }
        return p.join(',');
    }

    var geometry = '';
    for (var i = 0; i < json.length; i++) {
        geometry += '    {{' + (json[i].map(processPoint).join('},{')) + '}},\n';
    }

    var className = integer ? 'IntegerPolygon' : 'DoublePolygon';

    var cpp = '// This file is auto-generated, manual changes will be lost if the code is regenerated.\n\n';
    cpp += '#include "geometries.hpp"\n\n';
    cpp += 'MAPBOX_MSVC_DISABLE_OPTIMIZATION()\n';
    cpp += 'namespace mapbox {\n';
    cpp += 'namespace fixtures {\n\n';
    cpp += 'const ' + className + ' ' + id + ' = {\n';
    cpp += geometry;
    cpp += '};\n\n';
    cpp += '}\n';
    cpp += '}\n';
    cpp += 'MAPBOX_MSVC_ENABLE_OPTIMIZATION()\n';

    hpp += 'extern const ' + className + ' ' + id + ';\n';

    fs.writeFileSync('test/fixtures/' + id + '.cpp', cpp);
});

hpp += '\n';
hpp += '}\n';
hpp += '}\n';


fs.writeFileSync('test/fixtures/geometries.hpp', hpp);
