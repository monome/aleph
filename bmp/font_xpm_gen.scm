; a scheme program for generating bitmapped fonts from the GIMP.
; stage 1 of the aleph font process.
; produces a pile of images in .xpm format.					

(define (font_xpm_gen
	 outpath 	; target location
	 font		; font name
	 pre		; prefix for output .xpm names
	 font-size	; font size
;	 img-w		; w and height of img
;	 img-h		; 
	 crop           ; autocrop flag
	 all		; if set, generate all basic ascii characters, otherwise just numerals
;	 img-w		; initial (max) image width
;	 img-h		; initial (max) image height
;	 crop-w	; fonts are dumb... its best to arbitrarily crop the output.
;	 crop-h	; 
;	 crop-offx	; 
;	 crop-offy	; 
	 ; now using autocrop
	 )


  (define (draw_text img drawable text font size)
					; start undo group
    (gimp-undo-push-group-start img)
    
    (let * (
	    (text-float (car (gimp-text-fontname
			      img 		; image
			      drawable	; drawable
			      0		; x 
			      0		; y
			      text		; text string
			      0		; size of border
			      1		; antialiasing
			      size		; size in pixels
			      0		; unit (pixels)
			      font		; font name string
			      )))
	    )

      (display #\newline)
					; anchor selection
      (gimp-floating-sel-anchor text-float)
      
					; finish undo group
      (gimp-undo-push-group-end img)

					; flush output
      (gimp-displays-flush)
      )
    )

  (define (make_char_img char font fontsize w h)
    (let* (
	   (img (car (gimp-image-new w h RGB)))
	   (layer (car (gimp-layer-new img w h RGB "layer 1" 100 NORMAL)))
;	   (charstring (if (string=? char ".")
;			   "dot"
;			   (if (string=? char "/")
;			       "slash"
;			       char
;			       )
			   ; ... other special chars?
;			   )
	   ; use code point of character for filename
;	   (charstring ( (number->string (char->integer (string-ref char 0))) ))
;	   (charstring ( (char->name (string-ref char 0) ) ) )
	   (charstring (number->string (char->integer (string-ref char 0))))
	   (outstring 
		(string-append pre "_ascii_" charstring ".xpm")
		)
	    )

      (display " rendering glyph at path: ")
      (display (string-append outpath outstring) )
      (display #\newline)

					; add a layer
      (gimp-image-add-layer img layer 0)

					; paint background
      (gimp-palette-set-background '(255 255 255))
      (gimp-edit-fill layer BG-IMAGE-FILL)

					; draw the text
					; in fonts, the size in px represents the maximum baseline between one line and the next.
					; it does NOT represent the actual height of each character.
					; so, this needs to be fudged with arbitrarily different arguments
					; for px height of the output image, and px size of the font
					; ( could modify xpm_convert tool to discard empty columns / rows ? )
      (draw_text img layer char font fontsize)

					; convert to 4-bit indexed
      (gimp-image-convert-indexed img
				  0	; dither:  no dither
				  4 ; custom ; 0	; palette type: generate
				  16; 16	; number of colors
				  0	; alpha dither: no
				  1	; ignore unused (n/a)
				  "4bitgrey" ; "none"	; palette name (n/a)
				  )

      ; crop the image ... this is because fonts usually include some padding
;      (gimp-image-crop
;       img
;       crop-w 		; new width
;       crop-h 		; new height
;       crop-offx  		; off-x
;       crop-offy 		; off-y
;      )
      ; autocrop
      (if (> crop 0)
	  (plug-in-autocrop RUN-NONINTERACTIVE img layer)
      )
					; display
      (gimp-display-new img)

					; save
      (gimp-file-save RUN-NONINTERACTIVE
		      img
		      layer
		      (string-append outpath outstring)
		      outstring)
      ))

					; test
					; (make_char_img 14 24 "3" "Arial")

					; long string of all basic ascii characters starting from 0x20 [space]
					; ( let* ( (str "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}"))
					; just numbers
  (let* ( (str (if (= all 1)
		   "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}"
		   "./0123456789"
		   ))
	  (maxw 0)
	  (maxh 0)
	  )

; loop through string to find max w/h
    (define (str_scan n) 
      (if (= n 0)
	  n
	  (let*(
		(char (substring str (- n 1) n ))
		(w 0)
		(h 0)
		(ext '(0 0 0 0))
		)

	    (set! ext (gimp-text-get-extents-fontname char font-size 0 font))


	    (display #\newline)
	    (display char)
	    (display " ")
	    (display "font extents: ")
	    (display ext)
	    
	    (set! w (car ext))
	    (set! h (car (cdr ext)))
	    
	    (display " ( ")
	    (display w)
	    (display " , ")
	    (display h)
	    (display " ) ")

	    (if (> w maxw)
		(set! maxw w)
		)
	    (if (> h maxh)
		(set! maxh h)
		)
	    (str_scan (- n 1))
	    )    
	  )
      )
; loop through string to render
    (define (str_rend n) 
      (if (= n 0)
	  n
	  (let* 
	      ( (char (substring str (- n 1) n ) ) )
	    (display char)
	    (display " ")
	    (make_char_img char font font-size maxw maxh)
	    (str_rend (- n 1))
	    )    
	  )
      )
    (display str)
    (display " [ size: ")
    (display (string-length str))
    (display " ] ")
    (display #\newline)

    (str_scan (string-length str))
    (str_rend (string-length str))
    )
)
