(define (pascal-triangle-list total-layers)
  (define (get-next-layer above-layer)
    (if (null? (cdr above-layer))
        '(1)
        (cons (+ (car above-layer) (cadr above-layer))
              (get-next-layer (cdr above-layer)))))
  (define (generate-deep-layers n above-layer)
    (if (zero? n)
        '()
        (let ((this-layer (cons 1 (get-next-layer above-layer))))
             (cons this-layer (generate-deep-layers (- n 1) this-layer)))))
  (cond ((= total-layers 0) '())
        ((= total-layers 1) '((1)))
        ((= total-layers 2) '((1) (1 1)))
        (else (append '((1) (1 1)) (generate-deep-layers (- total-layers 2) '(1 1))))))

(define (print-pascal-triangle total-layers)
  (define (print-triangle pascal-list)
    (if (null? pascal-list)
      (newline)
      (let ()
           (pprint (car pascal-list))
           (newline)
           (print-triangle (cdr pascal-list)))))
  (print-triangle (pascal-triangle-list total-layers)))


(print-pascal-triangle 17) ; Prints `n` layers of Pascal's triangle