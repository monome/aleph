ok so to be a little clearer,
here are the steps you would take to generate an ant-aliased font,
or any other collection of bitmaps.

i'm looking at xpm_convert.c and remembering that it does many files at once, 
and that it was most convenient to simply hardcode these since they are C sources.

so, in xpm_convert.c, change the #includes near the top to include the digit .xpms/.

also, add their names to the static array on line 28.
these are the variable names that will be created in the output.

then:

gcc xpm_convert -o xpm-convert

./xpm-convert

and all the requested bitmap variables should show up in aleph_bmp_region.c


to create the images in GIMP:

- canvas size should be the same as the desired output size.

- flatten the image: ( Image/Flatten Image )

- change the color mode to Indexed with 16 optimized colors:
( Image/Mode -> Indexed... Optimized Pallette + Maximum Colors:16 )

- save as .xpm format.

the colors selected should be [ 0x000000, 0x111111, 0x222222... 0xffffff ],
which the converter will right-shift to 4 bits.
