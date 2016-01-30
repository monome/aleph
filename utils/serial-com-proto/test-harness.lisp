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

(defun send-serial-command (stream msg-type params)
  (prog1 (send-framed-message (cons (foreign-enum-value
				     'serial-msg-types msg-type)
				    params)
			      stream)
    (force-output stream)))

(defvar *s16-max* #xFFFF)
(defvar *char-max* #xFF)

(defun s16-chars (s16)
  (assert (< s16 *s16-max*))
  (list (ash s16 -8)
	(logand s16
		#x00FF)))

(defun chars-s16 (hi lo)
  (assert (<  hi *char-max*))
  (assert (<  lo *char-max*))
  (+ lo (ash hi 8)))

(defun serial-debug (stream control-string &rest format-arguments)
  (send-serial-command stream
		       :eSerialMsg_debug
		       (coerce (string-to-octets
				(apply #'format nil control-string
				       format-arguments)
				:external-format :ascii)
			       'list)))

(defun serial-dumpIns (stream)
  (send-serial-command stream
		       :eSerialMsg_dumpIns
		       '()))

(defun serial-dumpParams (stream)
  (send-serial-command stream
		       :eSerialMsg_dumpParams
		       '()))

(defun serial-trigger-param (stream addr val)
  (send-serial-command stream
		       :eSerialMsg_triggerParam
		       (append (s16-chars addr)
			       (s16-chars val))))

(defun serial-trigger-in (stream addr val)
  (send-serial-command stream
		       :eSerialMsg_triggerIn
		       (append (s16-chars addr)
			       (s16-chars val))))

(defun serial-query-in (stream addr)
  (send-serial-command stream
		       :eSerialMsg_queryIn
		       (s16-chars addr)))

(defun serial-query-param (stream addr)
  (send-serial-command stream
		       :eSerialMsg_queryParam
		       (s16-chars addr)))

(defun serial-recv-msg (stream)
  (let ((state :waiting)
	(bytes nil))
    (iterate (until (eq state :done))
	     (if (> (length bytes)
		    4096)
		 (error "why the long message?"))
	     (let ((new-byte (read-byte stream)))
	       (match state
		 (:waiting (if (= new-byte *start-flag*)
			       (setf state :receiving)))
		 (:receiving (cond
			       ((= new-byte *end-flag*) (setf state :done))
			       ((= new-byte *dle*) (setf state :escaping))
			       (t (push new-byte bytes))))
		 (:escaping (push new-byte bytes)
			    (setf state :receiving))
		 (otherwise (error "indeterminate state")))))
    (reverse bytes)))

#+nil
(with-open-file (stream "/home/rick/foo"
			:direction :output;:io
			:if-exists :overwrite
			:external-format :ascii)
  (serial-debug stream "noshrats ~A" 'woohoo))

#+nil
(with-open-file (stream "/home/rick/foo"
			:direction :output;:io
			:if-exists :overwrite
			:element-type '(unsigned-byte 8))
  (serial-debug stream "noshrats ~A" 'woohoo)
  (serial-dumpIns stream)
  (serial-dumpParams stream)
  (serial-trigger-param stream 3 3)
  (serial-trigger-in stream 4 4)
  (serial-query-in stream 4)
  (serial-query-param stream 5))

#+nil
(bt:make-thread
 (lambda ()
   (with-open-file (stream "/home/rick/foo"
			   :direction :output;:io
			   :if-exists :overwrite
			   :element-type '(unsigned-byte 8))

     (loop (sleep 1)
	(serial-debug stream "noshrats ~A" 'woohoo)
	(serial-dumpIns stream)
	(serial-dumpParams stream)
	(serial-trigger-param stream 3 3)
	(serial-trigger-in stream 4 4)
	(serial-query-in stream 4)
	(serial-query-param stream 5)))))

#+nil
(with-open-file (stream "/home/rick/bar"
			:direction :input
			:element-type '(unsigned-byte 8))
  (loop (print (serial-recv-msg stream))))
