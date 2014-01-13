CXXFLAGS = -O2 -Wall -Wextra -std=c++11 -stdlib=libc++ -fno-exceptions
CPPFLAGS = -DDEBUG
INCLUDE  = -Iinclude



SRCS    += src/map/map.cpp
SRCS    += src/map/tile.cpp
SRCS    += src/map/transform.cpp
SRCS    += src/geometry/linevertexbuffer.cpp
SRCS    += src/renderer/painter.cpp
SRCS    += src/renderer/shader.cpp
SRCS    += src/renderer/shader-fill.cpp
SRCS    += src/renderer/shader-line.cpp
SRCS    += src/shader/shaders.c
SRCS    += src/util/mat4.c

# OBJS     = $(patsubst %.mm,%.o,$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(SRCS))))

main: macosx

macosx: SRCS += macosx/main.mm
emscripten: SRCS += emscripten/main.cpp
emscripten3: SRCS += emscripten/main.cpp

macosx emscripten emscripten3: OBJS = $(patsubst %.mm,%.o,$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(SRCS))))

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) -c -o $@ $^

%.o: %.mm
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) -c -o $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $^


.SECONDEXPANSION:
macosx: $$(OBJS)
	$(CXX) -O3 $(OBJS) $(INCLUDE) -lglfw3 -framework OpenGL -framework Foundation -o macosx/main

emscripten: $$(OBJS)
	$(CXX) $(OBJS) $(INCLUDE) -o emscripten/main.js

emscripten3: $$(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) $(INCLUDE) -s DOUBLE_MODE=0 -s FORCE_ALIGNED_MEMORY=1 -s PRECISE_I64_MATH=0 --closure 1 -o emscripten/main.js
	uglifyjs emscripten/main.js -mc > emscripten/main.dist.js
	gzip -c emscripten/main.dist.js > emscripten/main.dist.js.gz

clean:
	rm -rf */*.o */*/*.o
	rm -rf macosx/main emscripten/main.js emscripten/main.js.map

.PHONY: macosx emscripten
