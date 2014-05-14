
0x0100:

loop:
    load 0xFFF1 R0
    jumpz R0 loop

output:
    load 0xFFF0 R1
    store R1 0xFFF0
    jumpnz R1 loop
    jumpz R1 end
 
end:
    halt
