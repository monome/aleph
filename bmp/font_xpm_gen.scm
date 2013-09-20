; a scheme program for generating bitmapped fonts from the GIMP.
; stage 1 of the aleph font process.
; produces a pile of images in .xpm format.					

(define (draw_text img drawable text font size)
; start undo gropu
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
    ; anchor selection
    (gimp-floating-sel-anchor text-float)
    
    ; finish undo group
    (gimp-undo-push-group-end img)

    ; flush output
    (gimp-displays-flush)
  )
)

(define (make_char_img w h char font fontsize)
  (let* (
	 (img (car (gimp-image-new w h RGB)))
	 (layer (car (gimp-layer-new img w h
				     RGB "layer 1" 100 NORMAL)))
	 (outpath "/home/emb/Desktop/aleph_fonts/")
	 (outstring (string-append font "_" (number->string w) "_" (number->string h) "_" (number->string fontsize) "-" char ".xpm"))
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
    ; can modify xpm_convert tool to discard empty columns / rows ?
    (draw_text img layer char font fontsize)

    ; convert to 4-bit indexed
    (gimp-image-convert-indexed img
				0	; dither:  no dither
				0	; palette type: generate
				16	; number of colors
				0	; alpha dither: no
				0	; ignore unused (n/a)
				"none"	; palette name (n/a)
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
(let* ( (str "0123456789.") )
  ; loop through the string
  (define (strdo n) 
    ; tail-recurse
    (if (= n 0)
	n
	(let* 
	    ( (char (substring str (- n 1) n ) ) )
	  (display char)
	  (display " ")
	  (make_char_img 16 24 char "Courier" 28)
	  (strdo (- n 1))
	  )    
	)
    )
  (display str)
  (display " [ size: ")
  (display (string-length str))
  (display " ] ")
  (strdo (string-length str))
)