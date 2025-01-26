.ORIG x3000

; Assign the correct secret here
; Shifts may be necessary
AND R2, R2, x0
ADD R2, R2, xa
ADD R2, R2, R2
ADD R2, R2, R2
ADD R2, R2, R2
ADD R2, R2, R2
ADD R2, R2, xa

; XOR Impl
NOT R4, R0
NOT R5, R2
AND R4, R4, R2
AND R5, R5, R0
NOT R4, R4
NOT R5, R5
AND R4, R4, R5
NOT R4, R4

; Output
AND R3, R3, x0
ADD R3, R3, R4

HALT
.END