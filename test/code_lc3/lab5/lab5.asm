.ORIG x3000
                AND         R1, R1, #0      ; the counter
                AND         R2, R2, #0      ; the state of FSM
                ADD         R2, R2, #1
                LD          R3, NegASCII0
                LD          R4, NegASCIIy
                
                LEA         R0, PromptMsg
                PUTS

; check the Sequence when a new character is inputed
LOOP            GETC
                OUT
                ; halt when 'y' is inputed
                ADD         R5, R0, R4
                BRz         DONE
                ; check the state
                AND         R5, R2, #1
                BRp         STATE0
                AND         R5, R2, #2
                BRp         STATE1
                AND         R5, R2, #4
                BRp         STATE2
                AND         R5, R2, #8
                BRp         STATE3
                BRnzp       LOOP

; check and update the state
STATE0          ADD         R5, R0, R3
                BRz         LOOP
                ADD         R2, R2, R2
                BRnzp       LOOP

STATE1          ADD         R5, R0, R3
                BRnp        LOOP
                ADD         R2, R2, R2
                BRnzp       LOOP

STATE2          ADD         R5, R0, R3
                BRz         SetState0
                ADD         R2, R2, R2
                BRnzp       LOOP

STATE3          ADD         R5, R0, R3
                BRnp        SetState1
                ADD         R1, R1, #1      ; 1010 is detected so the counter is increased
                BRnzp       SetState2

SetState0       AND         R2, R2, #0
                ADD         R2, R2, #1
                BRnzp       LOOP

SetState1       AND         R2, R2, #0
                ADD         R2, R2, #2
                BRnzp       LOOP

SetState2       AND         R2, R2, #0
                ADD         R2, R2, #4
                BRnzp       LOOP

; output
DONE            AND         R0, R0, #0      ; start a new line
                ADD         R0, R0, #10
                OUT
                LEA         R0, DoneMsg1
                PUTS
                JSR         BinaryToASCII
                PUTS
                LEA         R0, DoneMsg2
                PUTS
                HALT

BinaryToASCII   ST          R7, SAVE7
                LEA         R0, ASCIIBUFF
                NOT         R3, R3
                ADD         R3, R3, #1
Loop10          ADD         R1, R1, #-10
                BRn         End10
                ADD         R3, R3, #1
                BRnzp       Loop10
End10           STR         R3, R0, #0
                ADD         R1, R1, #10
                ADD         R1, R1, #15
                ADD         R1, R1, #15
                ADD         R1, R1, #15
                ADD         R1, R1, #3
                STR         R1, R0, #1
                LEA         R0, ASCIIBUFF
                LD          R7, SAVE7
                RET

PromptMsg       .STRINGZ    "SD is ready! Please input your number: "
DoneMsg1        .STRINGZ    "There are "
DoneMsg2        .STRINGZ    " 1010 in the sequence!"
NegASCII0       .FILL       xFFD0
NegASCIIy       .FILL       xFF87
SAVE7           .BLKW       1
ASCIIBUFF       .BLKW       2
                .FILL       x0000
.END