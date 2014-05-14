
0x0100:
    load #0 R0
    load #0 R1
    load #0 R2
    load #0 R3
    load #0 R4
    load #0 R5
    load #0 R6
    load #0 R7

main:
    call trap
    jump main

trap: jump serviceTrap

serviceTrap:
    push R0
    push R1
    jumpz R0 read
    add MONE R0 R0
    jumpz R0 write

read:
    call readChar

readChar
    load 0xFFF0 R1
    return 

write:
    call writeChar

writeChar:
    load R0 R2
    store R1 0xFFF0
    return

end:
    halt

trapstr: block #"trap\n"
