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

pragma_mapbox_regex = re.compile("(\s*)\#pragma \mapbox\: (define|initialize) (low|medium|high)p (float|vec(2|3|4)) (.*)")

def replace_uniform_pragmas(line):
    if pragma_mapbox_regex.match(line):
        params = line.split()
        if params[2] == "define":
            return """uniform {u_precision} {u_type} u_{u_name};""".format(
                    u_precision = params[3],
                    u_type = params[4],
                    u_name = params[5])
        else:
            return """    {u_precision} {u_type} {u_name} = u_{u_name};""".format(
                    u_precision = params[3],
                    u_type = params[4],
                    u_name = params[5])
    else:
        return line

data = "\n".join([replace_uniform_pragmas(line) for line in data.split("\n")])

content = """#pragma once

// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#include <mbgl/gl/gl.hpp>

namespace mbgl {{
namespace shaders {{
namespace {name} {{

#ifndef MBGL_SHADER_NAME_{NAME}
#define MBGL_SHADER_NAME_{NAME}
constexpr const char* name = "{name}";
#endif // MBGL_SHADER_NAME_{NAME}

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
