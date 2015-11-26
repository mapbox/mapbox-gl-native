# Troubleshooting

To trigger a complete rebuild, run `make clean`.

If you are having trouble getting the dependencies right, you can blow away the `mason_packages` directory, or run `make distclean`. This means the `Makefile` and configure script will automatically install the dependencies again on the next try.
