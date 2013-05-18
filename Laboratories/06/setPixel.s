0x0100:
    load #0 R0
    load #0 R1
    load #0 R2
    load #0 R3
    load #0 R4
    load #0 R5
    load #0 R6
    load #0 R7    

start:

readX:
    load #10 R4  ; since it is base 10
    load #48 R5  ; 0 has ascii 48

    ; load one char and calcualte its numeric value
    ; and multply constant 10 to it
    ; and add to the register R3 for accumulation
    load 0xFFF0 R2
    sub R2 R5 R2
    mult R2 R4 R3

    ; load one char and calcualte its numeric value
    ; and multply constant 10 to it
    ; and add to the register R3 for accumulation
    load 0xFFF0 R2
    sub R2 R5 R2
    add R2 R3 R3
    mult R3 R4 R3

    ; load one char and calcualte its numeric value
    ; and add  to the register 10
    load 0xFFF0 R2
    sub R2 R5 R2
    add R2 R3 R3

    ; copy the width to register R6 for storage
    move R3 R6 

readY:
    load #10 R4
    load #48 R5

    load 0xFFF0 R2
    sub R2 R5 R2
    mult R2 R4 R3
    

    load 0xFFF0 R2
    sub R2 R5 R2
    add R2 R3 R3
    mult R3 R4 R3

    load 0xFFF0 R2
    sub R2 R5 R2
    add R2 R3 R3

    ; copy the height to register R6 for storage
    move R3 R7

main:
    push R7 ; y
    push R6 ; x
    call getAddress
    pop R0 ; this is useless
    pop R0 ; this is the resulted address

    load R0 R3
    push R6
    push R3
    call getValue
    pop R3
    pop R3
    store R3 R0
    halt

getAddress:
    load #0x7C40 R0
    load SP #-1 R3
    load #32 R5
    div R3 R5 R3 ; x / 32
    add R0 R3 R0 ; 7c40 + x/32
    load SP #-2 R4
    load #6 R5
    mult R4 R5 R4
    add R0 R4 R0
    store R0 #-2 SP
    return 

getValue:
    load SP #-2 R4; x
    load #32 R5
    mod R4 R5 R4
    load #2 R5
    load #1 R3
shift:
    mult R5 R3 R3
    add R4 MONE R4
    jumpnz R4 shift

    load SP #-1 R4; old value
    or R3 R4 R3
    store R3 #-2 SP ; store return value
    return


