#!/usr/bin/env python

import sys, re, os, errno

input_file = sys.argv[1]
output_file = sys.argv[2]

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

shader_name, shader_type, extension = os.path.basename(input_file).split('.')

with open(input_file, "r") as f:
    data = f.read()

# Replace uniform pragmas

pragma_mapbox_regex = re.compile("(\s*)\#pragma \mapbox\: (initialize|define) (.*) (lowp|mediump|highp)")
color_regex = re.compile("(.*)color")

def replace_uniform_pragmas(line):
    # FIXME We should obtain these from the source code.
    if pragma_mapbox_regex.match(line):
        params = line.split()
        u_method = params[2]
        u_name = "color" if color_regex.match(params[3]) else params[3]
        u_precision = params[4]
        u_type = "vec4" if color_regex.match(u_name) else "float"
        if u_method == "define":
            return """uniform {precision} {type_} u_{name};""".format(
                    precision = u_precision,
                    type_ = u_type,
                    name = u_name)
        else:
            return """    {precision} {type_} {glsl_name} = u_{name};""".format(
                    precision = u_precision,
                    type_ = u_type,
                    glsl_name = params[3],
                    name = u_name)
    else:
        return line

data = "\n".join([replace_uniform_pragmas(line) for line in data.split("\n")])

content = """#pragma once

// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/gl/gl.hpp>

namespace mbgl {{
namespace shaders {{
namespace {name} {{

#ifdef GL_ES_VERSION_2_0
constexpr const char* {type} = R"MBGL_SHADER(precision highp float;\n{data})MBGL_SHADER";
#else
constexpr const char* {type} = R"MBGL_SHADER(#version 120\n{data})MBGL_SHADER";
#endif

}} // namespace {name}
}} // namespace shaders
}} // namespace mbgl
""".format(
     name = shader_name,
     NAME = shader_name.upper(),
     type = shader_type,
     TYPE = shader_type.upper(),
     data = data,
)

mkdir_p(os.path.dirname(output_file))
with open(output_file, 'w') as f: f.write(content)
