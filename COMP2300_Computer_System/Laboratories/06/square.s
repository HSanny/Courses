
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
    load 0xFFF0 R1 ; input ascii code
    load #48 R7
    sub R1 R7 R2 ; numeric value
    mult R2 R2 R3 ; square

    store R1 0xFFF0
    load #'^' R4
    store R4 0xFFF0
    load #'2' R4
    store R4 0xFFF0
    load #':' R4
    store R4 0xFFF0

    push ZERO
    push R3
    call print

print:
    load SP #-1 R4
    jumpz R4 printif
    load #10 R6
    div R4 R6 R3

    mod R4 R6 R4
    push R4
    push R3
    call print
    pop R3
    pop R4

    load #48 R7
    add R4 R7 R4
    store R4 0xFFF0
    return

printif:
    store ZERO #-1 SP
    return

end:
    halt
