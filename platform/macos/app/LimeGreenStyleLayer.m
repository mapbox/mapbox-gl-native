#import "LimeGreenStyleLayer.h"

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

@implementation LimeGreenStyleLayer {
    GLuint _program;
    GLuint _vertexShader;
    GLuint _fragmentShader;
    GLuint _buffer;
    GLuint _aPos;
}

- (void)didMoveToMapView:(MGLMapView *)mapView {
    static const GLchar *vertexShaderSource = "attribute vec2 a_pos; void main() { gl_Position = vec4(a_pos, 0, 1); }";
    static const GLchar *fragmentShaderSource = "void main() { gl_FragColor = vec4(0, 1, 0, 1); }";

    _program = glCreateProgram();
    _vertexShader = glCreateShader(GL_VERTEX_SHADER);
    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(_vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(_vertexShader);
    glAttachShader(_program, _vertexShader);
    glShaderSource(_fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(_fragmentShader);
    glAttachShader(_program, _fragmentShader);
    glLinkProgram(_program);
    _aPos = glGetAttribLocation(_program, "a_pos");

    GLfloat background[] = { -1,-1, 1,-1, -1,1, 1,1 };
    glGenBuffers(1, &_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), background, GL_STATIC_DRAW);
}

- (void)drawInMapView:(MGLMapView *)mapView withContext:(MGLStyleLayerDrawingContext)context {
    glUseProgram(_program);
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glEnableVertexAttribArray(_aPos);
    glVertexAttribPointer(_aPos, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

- (void)willMoveFromMapView:(MGLMapView *)mapView {
    if (!_program) {
        return;
    }

    glDeleteBuffers(1, &_buffer);
    glDetachShader(_program, _vertexShader);
    glDetachShader(_program, _fragmentShader);
    glDeleteShader(_vertexShader);
    glDeleteShader(_fragmentShader);
    glDeleteProgram(_program);
}

@end
