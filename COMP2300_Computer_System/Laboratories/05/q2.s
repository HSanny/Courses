; COMP2300 lab question 02
; Write an rPeANUt program which prints the string "Hello World" to the terminal using a loop and reading from a block of data.

0x0100 : 
init:
    load #0 R0
    load #0x5000 R2
start:
    load R2 R0
    jumpnz R0 loop
    jumpz R0 end

loop:
    store R0 0xFFF0
    add R2 ONE R2
    load R2 R0
    jumpnz R0 loop
    jumpz R0 end

end:
    halt

0x5000:
    str: block #"Hello World!\n" 
