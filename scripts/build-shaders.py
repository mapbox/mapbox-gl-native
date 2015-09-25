#!/usr/bin/env python

import sys, re, os, errno


output_dir = sys.argv[1]
file_names = sys.argv[2:]


def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

# Load all shaders
shaders = {}
for file_name in file_names:
    parts = re.search('/(\w+)\.(vertex|fragment)\.glsl$', file_name)
    if parts:
        shader_name = parts.group(1)
        shader_type = parts.group(2)
        if not shader_name in shaders:
            shaders[shader_name] = {}
        with open(file_name, "r") as f:
            shaders[shader_name][shader_type] = f.read()


def write_header():
    header = """// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.

#ifndef MBGL_SHADER_SHADERS
#define MBGL_SHADER_SHADERS

#include <mbgl/platform/gl.hpp>

namespace mbgl {

struct shader_source {
    const GLchar *vertex;
    const GLchar *fragment;
};

enum {
%s
    SHADER_COUNT
};

extern const shader_source shaders[SHADER_COUNT];

}

#endif
""" % '\n'.join(['    %s_SHADER,' % name.upper() for name in shaders.keys()])
    header_path = os.path.join(output_dir, 'include/mbgl/shader/shaders.hpp')
    mkdir_p(os.path.dirname(header_path))
    with open(header_path, 'w') as f: f.write(header)


def write_source(shader_platform, prefix, suffix):
    if shader_platform == 'gles2' or shader_platform == 'gles3':
        # OpenGL ES
        preamble = 'precision highp float;';
    else:
        # Desktop OpenGL
        preamble = '#version 120';

    shader_lines = [("""    {{
        R"{name}_vert({preamble}\n{vertex}){name}_vert",
        R"{name}_frag({preamble}\n{fragment}){name}_frag",
    }},
""").format(
    name = shader,
    preamble = preamble,
    vertex = shaders[shader]['vertex'],
    fragment = shaders[shader]['fragment']
) for shader in shaders]

    source = """// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
#include <mbgl/platform/gl.hpp>
{prefix}
#include <mbgl/shader/shaders.hpp>

namespace mbgl {{

const shader_source shaders[SHADER_COUNT] = {{
{shaders}
}};

}}
{suffix}
""".format(
    prefix = prefix,
    suffix = suffix,
    shaders = ''.join(shader_lines)
)

    source_path = os.path.join(output_dir, 'src/shader/shaders_' + shader_platform + '.cpp')
    mkdir_p(os.path.dirname(source_path))
    with open(source_path, 'w') as f: f.write(source)

write_header()
write_source('gl', '#ifndef GL_ES_VERSION_2_0', '#endif')
write_source('gles2', '#ifdef GL_ES_VERSION_2_0', '#endif')
