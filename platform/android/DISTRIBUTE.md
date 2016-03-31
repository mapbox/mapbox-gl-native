# Distributing Mapbox GL Native for Android

Depending on your use-case, you might want to support a subset of ABI's.
This can be achieved using the different `Makefile` targets we expose.

##### Build JNI libraries for all supported ABI's

```
make apackage
```

This will build libraries to support following ABI:
 - arm-v5
 - arm-v7
 - arm-v8
 - x86
 - x86-64
 - mips


##### Build JNI libraries for a specific ABI

```
make android-lib-%
```

In the command above you need to replace `%` with an ABI mentioned above. Eg.

```
make android-lib-x86
```

This will build the JNI library for the x86 architecture.