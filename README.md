# Equalate
Use `make && make clean` to create the binary

## Modules
The modules directory can be moved anywhere, as long as you pass it correcty to the binary
Use `make MODULE=modulename` to create the module
You can pass `ELDFLAGS=` to pass more flags to the linker, e.g `ELDFLAGS=-lcurl`
