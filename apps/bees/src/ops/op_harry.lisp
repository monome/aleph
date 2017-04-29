(in-package :cl-user)

(defun nondividing-harmonics (limit octave)
  (loop for i from 2 to limit
     when (not (integerp (/ i octave)))
     collect i))

(defun reduce-to-octave (n octave)
  (if (>= n octave)
      (reduce-to-octave (/ n octave) octave)
      n))

(defun increase-to-octave (n octave)
  (if (<= (* n 2) octave)
      (increase-to-octave (* n octave) octave)
      n))

(defun build-utonalities (limit octave)
  (cons 1
	(sort (mapcar (lambda (n)
			(increase-to-octave n octave))
		      (mapcar #'/
			      (nondividing-harmonics limit octave)))
	      #'>)))

(defun build-otonalities (limit octave)
  (cons 1
	(sort (mapcar (lambda (n)
			(reduce-to-octave n octave))
		      (nondividing-harmonics limit octave))
	      #'<)))

(defun build-tonality-diamond (limit &optional (octave 2))
  (mapcar (lambda (ut)
	    (mapcar (lambda (ot)
		      (reduce-to-octave (* ut ot)
					octave))
		    (build-otonalities limit octave)))
	  (build-utonalities limit octave)))

(defun ratio-to-equal-temperament (ratio)
  (* 12 (log ratio 2)))

(defun ratio-to-equal-temperament-fixed-point (ratio radix)
  (round (* (expt 2 radix)
	    (ratio-to-equal-temperament ratio))))

(defun print-tonality-diamond (diamond key name &optional (s *standard-output*))
  (format s "~a[~a][~a] = {~{{~{~a~^,~^ ~}}~^,~^~%~}};~%~%"
	  name
	  (length diamond)
	  (length (car diamond))
	  (mapcar (lambda (row)
		    (mapcar key row))
		  diamond)))

(defun aleph-print-tonality-diamond (limit)
  (print-tonality-diamond (build-tonality-diamond limit)
			  (lambda (ratio)
			    (ratio-to-equal-temperament-fixed-point ratio 8))
			  (format nil "static const io_t limit_~a" limit)))

(defun aleph-print-tonality-diamonds ()
  (let ((limits '(1 3 5 7 9 11 13 15)))
    (format t "#define NUM_DIAMONDS ~a~%" (length limits))
    (format t "static const u8 diamond_sizes[NUM_DIAMONDS] = {~{~a~^,~^ ~}};~%~%"
	    (mapcar (lambda(l)
		      (length (build-tonality-diamond l)))
		    limits))
    (mapcar #'aleph-print-tonality-diamond
	    limits)
    (format t "static const io_t *diamonds[NUM_DIAMONDS] = {~{~a~^,~^ ~}};~%"
	    (mapcar (lambda (l)
		      (format nil "&limit_~a" l))
		    limits))))
