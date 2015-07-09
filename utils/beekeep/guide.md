# beekeep 

graphical scene editor for aleph:BEES

currently linux-ony, mac build is coming.

## launching:

run by double-clicking the linux executable or launch from commandline fith filename argument. i recommend launching from a terminal so you can see any error messages that might occur.

## GUI layout

### top row:

scene name entry: edit the scene name

write buttons: export the scene as .scn or .json

module entry: enter the name of a DSP module (no extension.) beekeep searches for the corresponding descriptor (.dsc) file in the current working directory. if found, it populates the PARAMETERS column appropriately. if not found, it prints an error to standard output.

## columns:

five scrolling columns take up the majoriry of the screen. each column is populated with a number of selectable items.

- OPS: a list of operators. selection currently has no effect.

- OUTPUTS: a list of outputs.
  - selecting an output also selects the parameter or input item that corresponds to its target, if any. 
  - after the output name are two toggle-buttons labelled "I" and "S." clicking on these include and store, respectively, the current routing for this output in the selected preset. 
  - following the I/S buttons is a label showing the current target for the output, if any.

- INPUTS: a list of operator input nodes.
  - selecting an input has no immediate effect.
  - a spinbox allows you to set the current value of the input. just as in BEES, setting a value may cause the operator to produce output, and possibly propagate further changes.
  - the I(nclude)/S(tore) buttons work as for outputs.

- PARAMETERS: a list of DSP parameters
  - selecting a parameter has no immediate effect.
  - a spinbox for setting the value is included just as for inputs. beekeep will attempt to apply the correct parameter scaling and show the scaled value in an additional label field.
  - the I(nclude)/S(tore) buttons work as for outputs and inputs.

- PRESETS:
  - selecting a preset causes its stored routings and input/param values to be immediately reacalled. (at the moment this takes a moment because all relevant widgets are rebuilt.)
  - each preset item has a text entry field for editing the preset name.

### bottom row

below the "OPS" column there are several buttons:

- a popup menu which selects a new operator class for creation.
- a CREATE button which appends a new instance of the selected class to the operators list.
- a DELETE button, which removes the last created operator; *not* the selected operator (this might change.)

below the INPUTS and PARAMETERS columns are toggle-buttons labelled CONNECT. as expected, these toggle the connections of, respectively, the selected input and the selected DSP parameter, with the selected output.