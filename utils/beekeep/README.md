'beekeep' is an editor and conversion tool for BEES patches, for use with the monome aleph device.

it can read and write both the native .scn binary filetype, and a more portable .json representation.

basic usage:

```
./beekeep-x.x.x [filename]
```

where [filename] can be .scn or .json. if an input file is ommitted, the editor starts with a blank scene.




caveats:

just like BEES, beekeep relies on descriptor files for DSP module parameters. when you enter a module name (e.g. 'aleph-waves') in the editor, it will search the working directory for the corresponding .dsc file, and use it to load parameter scaling routines. the module .dsc files are distributed with aleph firmware releases, and can be built from source in `aleph/modules/foo`

for instructions on building beekeep from source, see build_osx.md and build_linux.md