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

content = """// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#ifndef MBGL_SHADER_{NAME}_{TYPE}
#define MBGL_SHADER_{NAME}_{TYPE}

#include <mbgl/platform/gl.hpp>

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

#endif
""".format(
     name = shader_name,
     NAME = shader_name.upper(),
     type = shader_type,
     TYPE = shader_type.upper(),
     data = data,
)

mkdir_p(os.path.dirname(output_file))
with open(output_file, 'w') as f: f.write(content)
