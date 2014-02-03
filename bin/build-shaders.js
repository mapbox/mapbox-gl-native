#!/usr/bin/env node
'use strict';

var fs = require('fs');
var path = require('path');

module.exports = function() {
    var name;
    var shaders = {};

    var shaderFiles = fs.readdirSync('src/shader');

    for (var i = 0; i < shaderFiles.length; i++) {
        var parts = shaderFiles[i].match(/^(.+)\.(vertex|fragment)\.glsl$/);
        if (parts) {
            name = parts[1];
            var type = parts[2];
            if (!(name in shaders)) {
                shaders[name] = {};
            }
            shaders[name][type] = fs.readFileSync(path.join('src/shader', shaderFiles[i]), 'utf8');
        }
    }

    var lines = [];
    var consts = [];
    for (var name in shaders) {
        consts.push(name.toUpperCase() + '_SHADER');

        var line = '';
        line += '   {\n';
        line += '       ' + JSON.stringify(shaders[name].vertex) + ',\n';
        line += '       ' + JSON.stringify(shaders[name].fragment) + ',\n';
        line += '   }';
        lines.push(line);
    }

    var header = '// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.\n\n';
    header += '#ifndef LLMR_SHADER_SHADERS\n';
    header += '#define LLMR_SHADER_SHADERS\n';
    header += '\n';
    header += 'namespace llmr {\n';
    header += '\n';
    header += 'struct shader_source {\n';
    header += '    const char *vertex;\n';
    header += '    const char *fragment;\n';
    header += '};\n';
    header += '\n';
    header += 'enum {\n';
    consts.push('SHADER_COUNT');
    header += '    ' + consts.join(',\n    ') + '\n';
    header += '};\n';
    header += '\n';
    header += 'extern const shader_source shaders[SHADER_COUNT];\n';
    header += '\n';
    header += '}\n';
    header += '\n';
    header += '#endif\n';
    fs.writeFileSync('include/llmr/shader/shaders.hpp', header);


    var code = '// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.\n';
    code += '#include <llmr/shader/shaders.hpp>\n';
    code += '\n';
    code += 'using namespace llmr;\n';
    code += '\n';
    code += 'const shader_source llmr::shaders[SHADER_COUNT] = {\n';
    code += lines.join(',\n');
    code += '\n};\n';

    fs.writeFileSync('src/shader/shaders.cpp', code);
};

module.exports();
