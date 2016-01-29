(ql:quickload :cffi)
(in-package :cl-user)
(use-package :cffi)

(defparameter *start-flag* #x12)
(defparameter *end-flag* #x13)
(defparameter *dle* #x7D)

(cffi:defcenum serial-msg-types
  :eSerialMsg_debug
  :eSerialMsg_dumpIns
  :eSerialMsg_insDump
  :eSerialMsg_dumpParams
  :eSerialMsg_paramsDump
  :eSerialMsg_triggerParam
  :eSerialMsg_triggerIn
  :eSerialMsg_queryIn
  :eSerialMsg_inVal
  :eSerialMsg_queryParam
  :eSerialMsg_paramVal
  :eSerialMsg_outVal
  :eSerialMsg_numParams)


(defun send-framed-message (bytes stream)
  (write-byte *start-flag* stream)
  (loop for byte in bytes
     do (if (or (= byte *start-flag*)
		(= byte *end-flag*)
		(= byte *dle*))
	    (progn (write-byte *dle* stream)
		   (write-byte byte stream))
	    (write-byte byte stream)))
  (write-byte *end-flag* stream))

(defun debug-test (stream msg-type params)
  (prog1 (send-framed-message (cons (foreign-enum-value
				     'serial-msg-types msg-type)
				    params)
			      stream)
    (force-output stream)))
#+nil
(with-open-file (stream "/home/rick/foo"
			:direction :output;:io
			:if-exists :overwrite
			:external-format :ascii)
  (serial-debug stream))
