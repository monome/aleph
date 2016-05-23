(eval-when (:compile-toplevel :load-toplevel)
  (ql:quickload '(:optima :cffi :iterate :external-program))
  (in-package :cl-user)
  (use-package '(:optima :cffi :iterate)))
(in-package :cl-user)
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

  :eSerialMsg_dumpOutputs
  :eSerialMsg_outputsDump
  :eSerialMsg_dumpConnections
  :eSerialMsg_connectionsDump
  :eSerialMsg_connect
  :eSerialMsg_disconnect
  :eSerialMsg_dumpOps
  :eSerialMsg_opsDump
  :eSerialMsg_dumpOpDescriptions
  :eSerialMsg_opDescriptionsDump
  :eSerialMsg_newOp
  :eSerialMsg_deleteOp
  :eSerialMsg_storePreset
  :eSerialMsg_recallPreset
  :eSerialMsg_bfinProgStart
  :eSerialMsg_bfinHexChunk
  :eSerialMsg_bfinDscChunk
  :eSerialMsg_bfinProgEnd
  :eSerialMsg_bfinProgEcho;;DEBUG - can we round-trip the hex?

  :eSerialMsg_numParams
  )


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

(defparameter *s16-max* #x7FFF)
(defparameter *s16-min* (- #x8000))
(defvar *char-max* #xFF)

(defun u16-s16 (unsigned)
  (assert (<= unsigned  #xFFFF))
  (assert (>= unsigned 0))
  (if (> unsigned #x7FFF)
      (+ (- #x10000)
	 unsigned)
      unsigned))

(defun s16-u16 (signed)
  (assert (<= signed  #x7FFF))
  (assert (>= signed (- #x8000)))
  (if (< signed 0)
      (+ #x10000
	 signed)
      signed))

(defun s16-chars (s16)
  (assert (<= s16 *s16-max*))
  (assert (>= s16 *s16-min*))
  (let ((u16 (s16-u16 s16)))
    (list (ash u16 -8)
	  (logand u16
		  #x00FF))))

(defun chars-s16 (hi lo)
  (assert (<=  hi *char-max*))
  (assert (<=  lo *char-max*))
  (u16-s16 (+ lo (ash hi 8))))

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
(defun serial-dumpOutputs (stream)
  (send-serial-command stream
		       :eSerialMsg_dumpOutputs
		       '()))
(defun serial-dumpConnections (stream)
  (send-serial-command stream
		       :eSerialMsg_dumpConnections
		       '()))
(defun serial-dumpOps (stream)
  (send-serial-command stream
		       :eSerialMsg_dumpOps
		       '()))
(defun serial-dumpOpDescriptions (stream)
  (send-serial-command stream
		       :eSerialMsg_dumpOpDescriptions
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
(defun serial-connect (stream out in)
  (send-serial-command stream
		       :eSerialMsg_connect
		       (append (s16-chars out)
			       (s16-chars in))))
(defun serial-disconnect (stream out)
  (send-serial-command stream
		       :eSerialMsg_disconnect
		       (s16-chars out)))
(defun serial-newOp (stream type)
  (send-serial-command stream
		       :eSerialMsg_newOp
		       (s16-chars type)))
(defun serial-deleteOp (stream op)
  (send-serial-command stream
		       :eSerialMsg_deleteOp
		       (s16-chars op)))
(defun serial-storePreset (stream preset)
  (send-serial-command stream
		       :eSerialMsg_storePreset
		       (s16-chars preset)))
(defun serial-recallPreset (stream preset)
  (send-serial-command stream
		       :eSerialMsg_recallPreset
		       (s16-chars preset)))

(defun serial-bfinProgStart (stream)
  (send-serial-command stream
		       :eSerialMsg_bfinProgStart
		       '()))
(defun serial-bfinHexChunk (stream bytes)
  (send-serial-command stream
		       :eSerialMsg_bfinHexChunk
		       bytes))
(defun serial-bfinDscChunk (stream bytes)
  (send-serial-command stream
		       :eSerialMsg_bfinDscChunk
		       bytes))
(defun serial-bfinProgEnd (stream)
  (send-serial-command stream
		       :eSerialMsg_bfinProgEnd
		       '()))

(defun serial-send-aleph-module (stream module-path dsc-path
				 &optional (chunk-pause 0.01))
  (serial-bfinProgStart stream)
  (let ((buf (make-array 64
			 :element-type '(unsigned-byte 8))))
    (with-open-file (module-stream module-path
				   :direction :input
				   :element-type '(unsigned-byte 8))
      (loop for read = (read-sequence buf module-stream)
	 while (plusp read)
	 do (sleep chunk-pause)
	   (serial-bfinHexChunk stream (subseq (coerce buf 'list)
					       0 read))))
    (with-open-file (dsc-stream dsc-path
				:direction :input
				:element-type '(unsigned-byte 8))
      (loop for read = (read-sequence buf dsc-stream)
	 while (plusp read)
	 do (sleep chunk-pause)
	   (serial-bfinDscChunk stream (subseq (coerce buf 'list)
					       0 read)))))
  (sleep chunk-pause)
  (serial-bfinProgEnd stream))

(defun eat-leading-string (chars)
  (iterate (for remaining on chars)
	   (until (= 0 (car remaining)))
	   (collect (car remaining) into octets)
	   (finally (return (cons (octets-to-string (coerce octets
							    '(vector (unsigned-byte 8))))
				  (cdr remaining))))))

(defun unpack-string-s16-s16-xN (connections-dump)
  (destructuring-bind (string . rest) (eat-leading-string connections-dump)
    (when (and string
	       (car rest)
	       (cadr rest)
	       (caddr rest)
	       (cadddr rest))
      (cons (list string
		  (chars-s16 (car rest)
			     (cadr rest))
		  (chars-s16 (caddr rest)
			     (cadddr rest)))
	    (unpack-string-s16-s16-xN (cddddr rest))))))
#+nil
(unpack-string-s16-s16-xN '(44 45 46 0 5 6 7 8 44 45 46 0 5 6 7 8))

(defun unpack-string-s16-xN (connections-dump)
  (destructuring-bind (string . rest) (eat-leading-string connections-dump)
    (when (and string
	       (car rest)
	       (cadr rest))
      (cons (list string
		  (chars-s16 (car rest)
			     (cadr rest)))
	    (unpack-string-s16-xN (cddr rest))))))
#+nil
(unpack-string-s16-xN '(44 45 46 0 5 6 44 45 46 0 5 6))

(defun unpack-s16-s16-xN (dump)
  (when (and (car dump)
	     (cadr dump)
	     (caddr dump)
	     (cadddr dump))
  (cons (list (chars-s16 (car dump)
			 (cadr dump))
	      (chars-s16 (caddr dump)
			 (cadddr dump)))
	(unpack-s16-s16-xN (cddddr dump)))))

(defun unpack-string-xN (octets)
  (when octets
    (destructuring-bind (str . rest) (eat-leading-string octets)
      (cons str (unpack-string-xN rest)))))
#+nil
(unpack-s16-s16-xN '(5 6 7 8 5 6 7 8))

(defun serial-recv-msg (stream)
  (let ((state :waiting)
	(bytes nil))
    (iterate (until (eq state :done))
	     ;; (if (> (length bytes)
	     ;; 	    4096)
	     ;; 	 (break "why the long message?"))
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

(defvar *echo-bfin-prog* nil)

(defun serial-unpack-message (bytes)
  (match bytes
    ((cons #.(foreign-enum-value 'serial-msg-types :eSerialMsg_debug)
	   octets)
     (list :debug
	   (octets-to-string (coerce octets
				     '(vector (unsigned-byte 8))))))
    ((cons #.(foreign-enum-value 'serial-msg-types :eSerialMsg_insDump)
	   octets)
     (cons :ins-dump
	   (unpack-string-xN octets)))
    ((cons #.(foreign-enum-value 'serial-msg-types :eSerialMsg_paramsDump)
	   octets)
     (cons :paramsDump
	   (unpack-string-xN octets)))
    ((list* #.(foreign-enum-value 'serial-msg-types :eSerialMsg_inVal)
	    addr-hi addr-lo val-hi val-lo _)
     (list :in-val (chars-s16 addr-hi addr-lo) (chars-s16 val-hi val-lo)))
    ((list* #.(foreign-enum-value 'serial-msg-types :eSerialMsg_paramVal)
	    addr-hi addr-lo val-hi val-lo _)
     (list :param-val (chars-s16 addr-hi addr-lo) (chars-s16 val-hi val-lo)))
    ((list* #.(foreign-enum-value 'serial-msg-types :eSerialMsg_outVal)
	    addr-hi addr-lo val-hi val-lo _)
     (list :out-val (chars-s16 addr-hi addr-lo) (chars-s16 val-hi val-lo)))
    ((list* #.(foreign-enum-value 'serial-msg-types :eSerialMsg_opDescriptionsDump)
	    op-descriptions)
     (cons :op-descriptions (unpack-string-xN op-descriptions)))
    ((list* #.(foreign-enum-value 'serial-msg-types :eSerialMsg_opsDump)
	    ops-dump)
     (cons :ops-dump (unpack-string-s16-s16-xN ops-dump)))
    ((list* #.(foreign-enum-value 'serial-msg-types :eSerialMsg_connectionsDump)
	    connections-dump)
     (cons :connections-dump (unpack-s16-s16-xN connections-dump)))
    ((list* #.(foreign-enum-value 'serial-msg-types :eSerialMsg_outputsDump)
	    outputs-dump)
     (cons :outputs-dump (unpack-string-xN outputs-dump)))
    ((list* #.(foreign-enum-value 'serial-msg-types :eSerialMsg_bfinProgEcho)
	    outputs-dump)
     (setf *echo-bfin-prog* outputs-dump)
     (list :bfin-echo-received))
    (otherwise (break "unknown message: ~A" bytes))))


;;;Some stinky debug stuff follows...
(defun start-debug-listener ()
  (list (multiple-value-list
	 (external-program:run "stty" '("-F" "/dev/ttyACM0" "115200" "raw")))
	(bt:make-thread
	 (lambda ()
	   (with-open-file (stream "/dev/ttyACM0"
				   :direction :io
				   :if-exists :overwrite
				   :element-type '(unsigned-byte 8))
	     (loop (print (serial-unpack-message (serial-recv-msg stream)))))))))

(defun test-all-commands ()
  (with-open-file (stream "/dev/ttyACM0"
			  :direction :output
			  :if-exists :overwrite
			  :element-type '(unsigned-byte 8))
    (serial-dumpIns stream)
    (serial-dumpParams stream)
    (serial-trigger-param stream 3 3)
    (serial-trigger-in stream 4 4)
    (serial-query-in stream 4)
    (serial-query-param stream 5)(serial-dumpoutputs stream)
    (serial-dumpconnections stream)
    (serial-dumpops stream)(sleep 1)
    (serial-dumpopdescriptions stream)
    (serial-connect stream 1 1)
    (serial-disconnect stream 1)
    (serial-newOp stream 1)
    (serial-deleteop stream 1)
    (serial-recallPreset stream 1)
    (serial-storePreset stream 1)
    ))

(defun test-bfin-module-load ()
  (with-open-file (stream "/dev/ttyACM0"
			  :direction :output
			  :if-exists :overwrite
			  :element-type '(unsigned-byte 8))
    (serial-send-aleph-module stream
			      "/home/rick/git_checkouts/aleph/modules/grains/grains.ldr"
			      "/home/rick/git_checkouts/aleph/modules/grains/grains.dsc" 0.001)))

(defvar *local-hex-buf* nil)

(defun hex-loopback-test ()
  (let ((buf (make-array (* 1024 256) :initial-element nil
			 :fill-pointer (* 1024 256))))
    (with-open-file (module-stream "/home/rick/git_checkouts/aleph/modules/analyser/analyser.ldr"
				   :direction :input
				   :element-type '(unsigned-byte 8))
      (setf (fill-pointer buf)
	    (read-sequence buf module-stream))
      (setf *local-hex-buf* (coerce buf 'list))
      (let ((i 0))
	(loop for c1 in *local-hex-buf*
	   for c2 in *echo-bfin-prog*
	   do (unless (= c1 c2)
		(break "~Ath char doesn't match" i))
	     (incf i))))))

(defun hexdump (li &optional)
  (loop for el in li
     collect (format nil "~X" el)))

;;Some even stinkier debug stuff used to query fifo ~/foo
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

