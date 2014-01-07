CXXFLAGS = -Wall -Wextra -std=c++11 -stdlib=libc++ -fno-exceptions
CPPFLAGS = -O3 -DDEBUG
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

macosx emscripten: OBJS = $(patsubst %.mm,%.o,$(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(SRCS))))

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) -c -o $@ $^

%.o: %.mm
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) -c -o $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(INCLUDE) -c -o $@ $^


.SECONDEXPANSION:
macosx: $$(OBJS)
	$(CXX) $(OBJS) $(INCLUDE) -lglfw3 -framework OpenGL -framework Foundation -o macosx/main

emscripten: $$(OBJS)
	$(CXX) $(OBJS) $(INCLUDE) -o emscripten/main.js

clean:
	rm -rf src/*/*.o
	rm -rf macosx/main

.PHONY: macosx emscripten
