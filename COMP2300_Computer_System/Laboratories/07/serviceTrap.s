trap: jump serviceTrap
0x0003:
    jump 

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
    trap
    jump main


serviceTrap:
    push R0
    push R2
    load #trapstr R0
    jump printTrap

printTrap:
    load R0 R2
    store R2 0xFFF0
    add ONE R0 R0
    jumpnz R2 printTrap
    pop R2
    pop R0
    return

end:
    halt

trapstr: block #"trap\n"
