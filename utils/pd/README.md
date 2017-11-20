# PD external for Bees

## Building

1. Find m_pd.h (`locate m_pd.h` if your platform supports)
1. Install liblo (`brew install liblo` on the macOS platform)
1. Install jansson (`brew install jansson` on the macOS platform)
1. Build with `make PDINCLUDEDIR=/path/to/include`
1. Copy `bees_op.pd_darwin` (on the macOS platform) to ~/Library/Pd/bees_op/
1. open one of the example patches like `bees_op_example.pd`

## Use

All operators are mapped to objects named [BEES_$OPNAME] where $OPPNAME is the name of the operator. For example, [BEES_MUL] is a multiplication operator.

## Importing scene graph from Aleph

1. Plug SD card into computer
1. Convert scene file into JSON
1. ???

## Exporting PD patch to Aleph

TBD
