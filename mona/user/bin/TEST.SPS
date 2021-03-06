(import (rnrs)
        (srfi :8)
        (mosh)
        (mosh test)
	(only (mosh ffi) string->utf8z)
        (mosh control)
        (mosh process)
        (monapi))

;; hold the stream not to GC-ed.
(define out (monapi-make-stream))

(define handle (monapi-stream-handle out))

(define (ls dir)
  (let1 shell (monapi-name-whereis "/servers/shell")
  (receive (from header . rest) (monapi-message-send-receive shell MSG_TEXT handle 0 0 (string->utf8z (string-append "ls " dir)))
    (let1 ret (utf8->string (monapi-stream-read out 64000))
      ret))))
         
(define (ps)
  (let1 shell (monapi-name-whereis "/servers/shell")
  (receive (from header . rest) (monapi-message-send-receive shell MSG_TEXT handle 0 0 (string->utf8z "ps"))
    (let1 ret (utf8->string (monapi-stream-read out 64000))
      ret))))
(start-process "/APPS/HELLO.EX5" out)

(let loop ()
	(display (utf8->string (monapi-stream-read out 64000)))
	(loop))

(test-true (#/GUI\.DL./ (ls "/LIBS/")))
(test-true (#/\d+ GUI.EX5\n/ (ps)))

(test-results)
