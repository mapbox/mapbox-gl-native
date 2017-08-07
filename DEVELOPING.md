# Modern C++ support

Mapbox GL Native supports the C++14 standard, and encourages contributions to
the source code using modern C++ idioms like return type deductions, generic
lambdas, `std::optional` and alike. However, we do not support all the features
from the final draft of the C++14 standard - we had to sacrifice support for
these features in order to support GCC from version 4.9 onwards.

The following C++14 features are **not supported** in Mapbox GL Native:

## [C++14 variable templates](https://isocpp.org/wiki/faq/cpp14-language#variable-templates)

Constructs like the example below are not supported:

```C++
template<typename T> constexpr T pi = T(3.14);
```

### Workarounds:

- If the variable is an alias, use the call the alias points to: [example](https://github.com/mapbox/mapbox-gl-native/commit/f1ac757bd28351fd57113a1e16f6c2e00ab193c1#diff-711ce10b54a522c948efc9030ffab4fcL269)
```C++
// auto foo = pi<double>;
auto foo = double(3.14);
```

- Replace variable templates with either functions or structs: [example 1](https://github.com/mapbox/mapbox-gl-native/commit/f1ac757bd28351fd57113a1e16f6c2e00ab193c1#diff-ffbe6cdfd30513aaa4749b4d959a5da6L58), [example 2](https://github.com/mapbox/mapbox-gl-native/commit/f1ac757bd28351fd57113a1e16f6c2e00ab193c1#diff-04af54dc8685cdc382ebe24466dc1d00L98)

## [C++14 aggregates with non-static data member initializers](http://en.cppreference.com/w/cpp/language/aggregate_initialization)

Constructs like the example below are not supported:

```C++
struct Foo {
    int x = { 0 };
};

// error: no matching function for call to 'Foo::Foo(<brace-enclosed initializer list>)'
int main() {
    Foo foo { 0 };
    return 0;
}
```

### Workarounds
- Replace data member initializers with default parameter values in default constructors:

```C++
struct Foo {
    Foo(int x_ = 0) : x(x_) {}
    int x;
};

int main() {
    Foo foo { 0 }; // works with default constructor
    return 0;
}
```

- Replace bracket initialization with regular round brackets or none:

```C++
struct Foo {
    Foo(int x_ = 0) : x(x_) {}
    int x;
};

int main() {
    Foo foo(); // works
    Foo bar; // also works
    return 0;
}
```

## [Extended `constexpr` support](https://isocpp.org/wiki/faq/cpp14-language#extended-constexpr)

GCC 4.9 strictly forbids `constexpr` usage in the following scenarios:
- No local variable declarations (not `static` or `thread_local`, and no uninitialized variables)
- Cannot mutate objects whose lifetime began with the constant expression evaluation
- Disable usage of if, switch, for, while, do-while (not goto) inside constexpr expressions
- Enforces that constexpr member functions are implicitly const

```C++
// sorry, unimplemented: use of the value of the object being constructed
// in a constant expression
struct Foo {
    int x, y;
    constexpr Foo(int i) : x(i), y(x) {}
};

// error: body of constexpr function 'constexpr int test1(int)' not a
// return-statement
constexpr int test1(int i) {
    int j = i;
    return j;
}

// error: body of constexpr function 'constexpr bool test2(int)' not a
// return-statement
constexpr bool test2(int i) {
    if (i > 0) {
        return true;
    } else {
        return false;
    }
}
```

### Workarounds

- Either remove `constexpr` specifier or replace it with `inline` in case of
  functions


## [Polymorphic lambdas](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68278)

GCC 5.2.0 crashes with polymorphic lambdas and this version of the compiler
is currently used in Qt Automotive. Luckily polymorphic lambdas are rarely
used/needed but we had one incident fixed by #9665.

### Workarounds

- Copy & Paste™ the code.
