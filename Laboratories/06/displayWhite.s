0x0100:
start:
    load #0 R1
    load #0 R2
    load #192 R6
    load #160 R7
loop:
    sub R6 R1 R3
    jumpz R3 incrementY
    sub R7 R2 R3
    jumpz R3 end
    push R2 ;push y
    push R1 ;push x
    call graphicAddress
    pop R0
    pop R5
    load #0xFFFFFFFF R5
    store R5 R0
incrementX:
    load #8 R5
    add R5 R1 R1
    jump loop
graphicAddress:
    load #0x7C40 R0
    load SP #-1 R3
    load #32 R5
    div R3 R5 R3 ; x / 32
    add R0 R3 R0 ; 7c40 + x/32
    load SP #-2 R4
    load #6 R5
    mult R4 R5 R4
    add R0 R4 R0
    store R0 #-1 SP
    return
incrementY:
    move ZERO R1
    add ONE R2 R2
    jump loop
end:
    halt
