#pragma once

#include <QtGlobal>

// Qt4
#if QT_VERSION < 0x050000
    #if MBGL_USE_GLES2
        #define GL_GLEXT_PROTOTYPES
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>
    #else
        #define GL_GLEXT_PROTOTYPES
        #include <GL/gl.h>
        #include <GL/glext.h>
    #endif

// Qt5
#else
    #define GL_GLEXT_PROTOTYPES
    #include <QtGui/qopengl.h>
#endif
