// (c) Dean McNamee <dean@gmail.com>, 2012.
// C++ port by Konstantin Käfer <mail@kkaefer.com>, 2014.
//
// https://github.com/deanm/css-color-parser-js
// https://github.com/kkaefer/css-color-parser-cpp
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef CSS_COLOR_PARSER_CPP
#define CSS_COLOR_PARSER_CPP

#include <string>

namespace CSSColorParser {

struct Color {
    inline Color() {}
    inline Color(unsigned char r_, unsigned char g_, unsigned char b_, float a_)
        : r(r_), g(g_), b(b_), a(a_) {}
    unsigned char r = 0, g = 0, b = 0;
    float a = 1.0f;
};

Color parse(const std::string& css_str);

}

#endif
