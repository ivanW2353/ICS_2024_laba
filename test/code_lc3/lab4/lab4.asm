            .ORIG x3000
            LDI R0, INPUT
            LD R6, STACK
            JSR CALC
            LDR R0, R6, #2
            STI R0, RESULT
            HALT
    
CALC
            ; PUSH R7
            ADD R6, R6, #-1
            STR R7, R6, #0
            
            ; JUDGE N ?= 1
            ADD R1, R0, #-1
            BRn INIT
            
            ; CALL CALC(N - 1)
            ADD R0, R0, #-1
            JSR CALC
            
            ; POP EARN(N - 1)
            LDR R0, R6, #0
            ADD R6, R6, #1
            
            ; POP SPEND(N - 1)
            LDR R1, R6, #0
            ADD R6, R6, #1
            
            ; POP SAVE(N - 1)
            LDR R2, R6, #0
            ADD R6, R6, #1
            
            ; POP R7
            LDR R7, R6, #0
            ADD R6, R6, #1
            
            ; EARN(N) = 2 * EARN(N - 1)
            ADD R3, R0, R0
            
            ; R4 = -SPEND(N - 1)
            NOT R4, R1
            ADD R4, R4, #1
            
            ; SAVE(N) = SAVE(N - 1) + EARN(N - 1) - SPEND(N - 1)
            ADD R2, R2, R0
            ADD R2, R2, R4
            
            ; CALC SPEND(N)
            ADD R1, R1, R1
            ADD R1, R1, R1
            ADD R5, R0, R4
            BRzp EARNMORE
            AND R1, R1, #0
            ADD R1, R1, #2
            
EARNMORE
            ; PUSH SAVE(N)
            ADD R6, R6, #-1
            STR R2, R6, #0
            
            ; PUSH SPEND(N)
            ADD R6, R6, #-1
            STR R1, R6, #0
            
            ; PUSH EARN(N)
            ADD R6, R6, #-1
            STR R3, R6, #0
            RET
    
INIT
            ; POP R7
            LDR R7, R6, #0
            ADD R6, R6, #1
            
            ; PUSH SAVE(0)
            AND R1, R1, #0
            ADD R1, R1, #10
            ADD R6, R6, #-1
            STR R1, R6, #0
            
            ; PUSH SPEND(0)
            ADD R1, R1, #-8
            ADD R6, R6, #-1
            STR R1, R6, #0
            
            ; PUSH EARN(0)
            ADD R1, R1, #4
            ADD R6, R6, #-1
            STR R1, R6, #0
            RET
    
STACK   .FILL x6000
INPUT   .FILL x3100
RESULT  .FILL x3200
.END
