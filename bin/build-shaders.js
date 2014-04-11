#!/usr/bin/env node
'use strict';

var fs = require('fs');
var path = require('path');
var lazy_update = require('./lazy-update');

try { var glsl = require('glsl-optimizer'); } catch(err) {}

module.exports = function(shader_type, prefix, suffix) {
    var name;
    var shaders = {};

    var shaderFiles = fs.readdirSync('src/shader');

    // Load shaders
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


    // Optimize shader
    if (glsl) {
        var preamble = '';
        var target = glsl.TARGET_OPENGL;
        if (shader_type == 'gles2') {
            target = glsl.TARGET_OPENGLES20;
            preamble = 'precision highp float;';
        } else if (shader_type == 'gles3') {
            target = glsl.TARGET_OPENGLES30;
            preamble = 'precision highp float;';
        } else {
            preamble = '#version 120';
        }

        var compiler = new glsl.Compiler(target);
        for (name in shaders) {
            var vertex_shader = new glsl.Shader(compiler, glsl.VERTEX_SHADER, preamble + '\n' + shaders[name].vertex);
            if (vertex_shader.compiled()) {
                shaders[name].vertex = vertex_shader.output();
            } else {
                console.warn('failed to optimize %s vertex shader', name);
                process.exit(1);
            }
            vertex_shader.dispose();

            var fragment_shader = new glsl.Shader(compiler, glsl.FRAGMENT_SHADER, preamble + '\n' + shaders[name].fragment);
            if (fragment_shader.compiled()) {
                shaders[name].fragment = fragment_shader.output();
            } else {
                console.warn('failed to optimize %s fragment shader', name);
                process.exit(1);
            }
            fragment_shader.dispose();
        }
        compiler.dispose();
    } else {
        console.warn('Not optimizing shaders');
    }


    // Save to file
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


    var code = '// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.\n';
    code += '#include <llmr/platform/gl.hpp>\n';
    code += prefix + '\n';
    code += '#include <llmr/shader/shaders.hpp>\n';
    code += '\n';
    code += 'using namespace llmr;\n';
    code += '\n';
    code += 'const shader_source llmr::shaders[SHADER_COUNT] = {\n';
    code += lines.join(',\n');
    code += '\n};\n';
    code += suffix + '\n';


    lazy_update('include/llmr/shader/shaders.hpp', header);
    lazy_update('src/shader/shaders_' + shader_type + '.cpp', code);
};

module.exports('gl', '#ifndef GL_ES_VERSION_2_0', '#endif');
module.exports('gles2', '#ifdef GL_ES_VERSION_2_0', '#endif');
