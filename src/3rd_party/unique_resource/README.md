unique\_resource, The General RAII Wrapper for Exclusive Ownership Resource Management.
===

This is an unique\_resource implementation with Boost Software License 1.0.  
This implementation is based on example implementation in the C++ Standards Committee Paper [N4189](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4189.pdf).

What is unique\_resource
---

unique\_resource is a generalization of std::unique\_ptr.
std::unique\_ptr can be instantiated only by pointer like types.
unique\_resource does not have such limitation and, normally, works with a user's customized deleter.

Installation
---

Put unique\_resource.hpp into your including directory.

Sample Usage
---

```c++
#include <iostream>
#include "unique_resource.hpp"

// some dummy resource functions
using handle_t = int;
decltype(auto) GenHandle(handle_t i) { return i; }
decltype(auto) ReleaseHandle(handle_t handle) {
	std::cout << "release handle " << handle << std::endl;
}

decltype(auto) make_unique_handle(handle_t i) {
	return std_experimental::make_unique_resource(
		GenHandle(i), &ReleaseHandle);
}
int main() {
	auto handle1 = make_unique_handle(1);
	{
		auto handle2 = make_unique_handle(2);
		std::cout << handle2.get() << std::endl; // raw handle access by get()
		// here, handle2 is released.
	}
	auto handle3 = make_unique_handle(3);
	handle3.reset(); // release handle explicitly
	{
		auto handle4 = make_unique_handle(4);
		auto raw_handle4 = handle4.release(); // strip RAII wrapper
		ReleaseHandle(raw_handle4); // now, we should release handle by hand.
	}
	{
		auto handle5 = make_unique_handle(5);
		auto handle6 = std::move(handle5); // move construct
		handle6.reset(GenHandle(6)); // reset raw handle
	}

	std::cout << "Bye!" << std::endl;
}
```

output

```
2
release handle 2
release handle 3
release handle 4
release handle 5
release handle 6
Bye!
release handle 1
```

The detailed documentation is on 7.4.1 of [N4189](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4189).

What is different from the example implementation in N4189
---

- License  
The original codes have no license to use. This implementation has Boost Software License 1.0 (thanks Sandval and Sommerlad for the consent).

- Namespace  
The original codes are included in namespace "std::experimental", but adding any user codes into namespace "std" or nested namespaces of it cause undefined behavior. So in this implementation, the namespace is changed from "std::experimental" to "std\_experimental".

- Some codes for C++11  
Using features of C++14, the original codes do not work under C++11. This implementation works under C++11 (and, of course, under C++14 or later).

- Workaround for GCC  
Because of the [bug](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52869) of GCC, the original codes cause compilation error with GCC. This implementation contains the workaround about the bug and works fine with GCC.

- Test codes  
The original implementation has test codes but they uses external libraries except standard libraries. This implementation also has test codes but they do not use such libraries except standard ones.
