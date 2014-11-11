'beekeep' is an editor and conversion tool for BEES patches, for use with the monome aleph device.

it can read and write both the native .scn binary filetype, and a more portable .json representation.

basic usage:

```
./beekeep-x.x.x [filename]
```

where [filename] can be .scn or .json. if an input file is ommitted, the editor starts with a blank scene.

most functions are pretty self-explanatory but we can add some wiki documentation to the monome.org docs if needed.

caveats:

at the moment, the .json parsing library is dynamically linked. so you will have to have libjansson installed (version 2.5 or greater.) it can be acquired here, and should be easily built and installed from any linux or os x system:
[ http://www.digip.org/jansson/ ]

after installing, you may have to update your LD_LIBRARY_PATH variable like so (add to .bashrc or equivalent for long-term use):
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```

just like BEES, beekeep relies on descriptor files for DSP module parameters. when you enter a module name (e.g. 'aleph-waves') in the editor, it will search the working directory for the corresponding .dsc file, and use it to load parameter scaling routines. the module .dsc files are distributed with aleph firmware releases, and can be built from source in `aleph/modules/foo`

for instructions on building beekeep from source, see build_osx.md and build_linux.md