.ORIG x3000
    LDI R0, A0
    AND R1, R1, #0

LOOP
    ADD R2, R0, #-1
    BRz END

    AND R2, R0, #1
    BRz EVEN
    
    ADD R2, R0, #0
    ADD R2, R2, R0
    ADD R2, R2, R0
    ADD R0, R2, #1
    ADD R1, R1, #1
    BRnzp LOOP
    
EVEN
    AND R2, R2, #0
    ADD R2, R2, #2
    
    AND R3, R3, #0
    ADD R3, R3, #1
    
    AND R5, R5, #0
    
AGAIN
    AND R4, R0, R2
    BRz #1
    ADD R5, R5, R3
    ADD R3, R3, R3
    ADD R2, R2, R2
    BRnp AGAIN
    ADD R0, R5, #0
    ADD R1, R1, #1
    BRnzp LOOP
    
END
    STI R1, A1
    HALT
    
A0
    .FILL x3100
A1
    .FILL x3101
.END