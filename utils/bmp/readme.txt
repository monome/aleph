aleph/bmp

ok this has evolved into a little ecosystem of scripts.

the goal is to create arbitrary fonts for the aleph with 4-bit greyscale.

right now, there are 3 steps to this process:

1)  run the scheme script font_xpm_gen.scm
    this calls the GIMP graphics editor to render a list of ascii chars.
    run it one of two ways:
        a)  copy it to ~/.gimp-2.6/scripts,
            then type: gimp -i -b '(font_xpm_gen)
        b)  run it from emacs-gimp or from the script-fu console in the GIMP gui.  
        * note
        
    either way, it should create a lot of .xpm files at the indicated path.    

2)  compile and run xpm_read.c
    this scans the output directory of step 1) 
    and produces 3 include files for use in step 3)
    
3)  compile and run xpm_convert.c
    using the data from step 1) and metadata from step 2), 
    this produces a single source file with all requested glyphs,
    in ready-to-use format for the avr32 application build.


* note:
    there are a lot of arguments to this script.
    here's what is happening:
    most fonts include some arbitrary spacing on the characters
    within the bounds specified by the font size.
    we don't want any space on the margins.
    we could crop empty rows/columns automatically during conversion,
    but perhaps its better to have each glyph the same size.
    so instead there are arguments to arbitrarily crop each image after rendering.
    the crop arguments will have different ideal values for different fonts,
    best determined experimentally i'm afraid.
    
    example invocations:
    
    (fonts_xpm_gen "/home/emb/Desktop/aleph_fonts" "Courier" 0 18 28 32 15 22 2 6)
    (font_xpm_gen "/home/emb/Desktop/aleph_fonts/" "Monospace" 0 18 28 28 16 24 1 4)
    
