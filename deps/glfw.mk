
glfw: local/lib/libglfw3.a

local/lib/libglfw3.a: deps/glfw/build/Makefile
	make -C deps/glfw/build install

deps/glfw/build/Makefile:
	mkdir -p deps/glfw/build
	cd deps/glfw/build && cmake -DCMAKE_INSTALL_PREFIX=../../../local -DGLFW_BUILD_EXAMPLES=off -DGLFW_BUILD_TESTS=off -DGLFW_BUILD_DOCS=off ..
