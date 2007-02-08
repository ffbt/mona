(assert-check-true "procedure +"
                   (= 3 (+ 1 2))
                   (= 2 (+ 4 -2))
                   (= 11 (+ 4 2 5)))

(assert-check-true "procedure -"
                   (= 2 (- 3 1))
                   (= 0 (- 3 1 2))
                   (= -3 (- 3 1 2 3))
                   (= -3 (- 3)))

(assert-check-true "procedure /"
                   (= 2 (/ 4 2))
                   (= 2 (/ 8 2 2))
                   (= 1 (/ 54 2 9 3)))

(assert-check-true "procedure *"
                   (= 2 (* 1 2))
                   (= -8 (* 4 -2))
                   (= 40 (* 4 2 5)))

(assert-check-true "number? should be #t"
                   (number? 3)
                   (number? (+ 1 2)))

(assert-check-false "number? should be #f"
                    (number? "string")
                    (number? (cons 1 2)))


(assert-check-true "="
                   (= 1 1 1 1)
                   (= 1 (+ 0 1) 1)
                   (= 1 1))

(assert-check-false "="
                   (= 1 2 1 1)
                   (= 1 "1" 1))

(assert-check-true ">"
                   (> 2 1)
                   (> 5 4 3)
                   (> 5 4 3 2 1))

(assert-check-false ">"
                   (> 1 2)
                   (> 5 3 4)
                   (> 5 4 4)
                   (> "4" 3 2 1))

(assert-check-true ">="
                   (>= 2 1)
                   (>= 5 4 4)
                   (>= 5 4 3 2 1))

(assert-check-false ">="
                   (>= 1 2)
                   (>= 5 3 4)
                   (>= "4" 3 2 1))

(assert-check-false "< #f"
                   (< 2 1)
                   (< 5 4 3)
                   (< 5 4 3 2 1))

(assert-check-true "<"
                   (< 1 2)
                   (< 5 6 7)
                   (< 1 2 3 4))

(assert-check-false "<= #f"
                   (<= 2 1)
                   (<= 5 4 4)
                   (<= 5 4 3 2 1))

(assert-check-true "<="
                   (<= 1 2)
                   (<= 3 4 4))

(assert-check-true "number->string"
                   (string=? (number->string 1234) "1234"))

(assert-check-true "positive?/negative?"
                   (positive? 1)
                   (negative? -2))

(assert-check-false "positive?/negative?"
                   (positive? 0)
                   (positive? -1)
                   (negative? 0)
                   (negative? 1))

(assert-check-true "abs"
                   (= (abs 3) 3)
                   (= (abs 0) 0)
                   (= (abs -1) 1))

(assert-check-true "not"
                   (if (not #f) #t #f)
                   (if (not 3)  #f #t))

(assert-check-true "odd?/even?"
                   (odd? 1)
                   (odd? 3)
                   (odd? 5)
                   (odd? 7)
                   (even? 0)
                   (even? 2)
                   (even? 4)
                   (even? 6))




