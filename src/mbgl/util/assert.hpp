#pragma once

#include <cassert>

// Based on FreeBSD's src/include/assert.h
// Licensed under the original BSD license
#define assert_always(expr)                                                                        \
    ((void)((expr) ? ((void)0) : ((void)fprintf(stderr, "%s:%u: failed assertion `%s'\n",          \
                                                __FILE__, __LINE__, #expr),                        \
                                  abort())))
