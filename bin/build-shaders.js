#!/usr/bin/env node
'use strict';

var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');

try { var glsl = require('mapbox-glsl-optimizer'); } catch(err) {}

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

    var preamble = '';
    if (shader_type == 'gles2' || shader_type == 'gles3') {
        preamble = 'precision highp float;';
    } else {
        preamble = '#version 120';
    }

    for (var name in shaders) {
        shaders[name].vertex = preamble + '\n' + shaders[name].vertex;
        shaders[name].fragment = preamble + '\n' + shaders[name].fragment;
    }

    // Optimize shader
    if (glsl) {
        var target = shader_type == 'gles2' ? glsl.TARGET_OPENGLES20 : (shader_type == 'gles3' ? glsl.TARGET_OPENGLES30 : glsl.TARGET_OPENGL);

        var compiler = new glsl.Compiler(target);
        for (name in shaders) {
            var vertex_shader = new glsl.Shader(compiler, glsl.VERTEX_SHADER, shaders[name].vertex);
            if (vertex_shader.compiled()) {
                shaders[name].vertex = vertex_shader.output();
            } else {
                console.warn('failed to optimize %s vertex shader', name);
                process.exit(1);
            }
            vertex_shader.dispose();

            var fragment_shader = new glsl.Shader(compiler, glsl.FRAGMENT_SHADER, shaders[name].fragment);
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
    header += '#ifndef MBGL_SHADER_SHADERS\n';
    header += '#define MBGL_SHADER_SHADERS\n';
    header += '\n';
    header += 'namespace mbgl {\n';
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
    code += '#include <mbgl/platform/gl.hpp>\n';
    code += prefix + '\n';
    code += '#include <mbgl/shader/shaders.hpp>\n';
    code += '\n';
    code += 'namespace mbgl {\n';
    code += '\n';
    code += 'const shader_source shaders[SHADER_COUNT] = {\n';
    code += lines.join(',\n');
    code += '\n};\n';
    code += '\n}\n';
    code += suffix + '\n';

    var header_path = path.join(process.argv[2], 'include/mbgl/shader/shaders.hpp');
    mkdirp.sync(path.dirname(header_path));
    fs.writeFileSync(header_path, header);
    console.warn('wrote file ' + header_path);
    var file_path = path.join(process.argv[2], 'src/shader/shaders_' + shader_type + '.cpp');
    mkdirp.sync(path.dirname(file_path));
    fs.writeFileSync(file_path, code);
    console.warn('wrote file ' + file_path);

};

module.exports('gl', '#ifndef GL_ES_VERSION_2_0', '#endif');
module.exports('gles2', '#ifdef GL_ES_VERSION_2_0', '#endif');
