.ORIG x3000

    LDI R0, LENGTH
    BRz isPALINDROME
    
    LD R1, STRING       ; R1 is the pointer of the string
    ADD R2, R0, R1
    ADD R2, R2, #-1     ; R2 is the pointer of the last character of the string
    
LOOP
    LDR R3, R1, #0      ; Load character from the start
    LDR R4, R2, #0      ; Load character from the end
    NOT R4, R4          
    ADD R4, R4, #1      
    ADD R3, R3, R4      
    BRnp notPALINDROME ; If not equal, string is not a palindrome

    ; Increment start pointer and decrement end pointer
    ADD R1, R1, #1
    ADD R2, R2, #-1
    ADD R0, R0, #-2
    BRp LOOP            ; Continue if pointers have not crossed

isPALINDROME
    AND R0, R0, #0      ; Clear R0
    ADD R0, R0, #1      ; Set R0 to 1
    STI R0, RESULT       ; Store result at x3200
    HALT
    
notPALINDROME
    AND R0, R0, #0      ; Clear R0 (set to 0)
    ST R0, RESULT       ; Store result at x3200
    HALT

LENGTH
    .FILL x3100
STRING
    .FILL x3101
RESULT
    .FILL x3200
.END

