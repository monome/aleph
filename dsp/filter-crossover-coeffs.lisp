(in-package :cl-alsaseq)
#+nil
(loop for i from 10 to 16
   as freq = (expt 10000 (/ i 16))
   do (format t "set_crossover_lpf_coeffs_from_floats(&(xo->xs[~a]),~{~f~^,~});~%" (- i 10) (make-biquad-lpf-coefficients freq 0.7071))
     (format t "set_crossover_hpf_coeffs_from_floats(&(xo->xs[~a]),~{~f~^,~});~%" (- i 10) (make-biquad-hpf-coefficients freq 0.7071))
     collect freq)
