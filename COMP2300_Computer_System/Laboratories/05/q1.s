; COMP2300 lab question 01
; Write an rPeANUt program which prints the string 'Hello World' in the terminal without using a loop.

0x0100 : 
init:  
    load 0xfff0 R0
	load #0 R1

start:
    load #'H' R1
    store R1 0xFFF0
    load #'e' R1
    store R1 0xFFF0
    load #'l' R1
    store R1 0xFFF0
    load #'l' R1
    store R1 0xFFF0
    load #'o' R1
    store R1 0xFFF0
    load #' ' R1
    store R1 0xFFF0
    load #'W' R1
    store R1 0xFFF0
    load #'o' R1
    store R1 0xFFF0
    load #'r' R1
    store R1 0xFFF0
    load #'l' R1
    store R1 0xFFF0
    load #'d' R1
    store R1 0xFFF0
    load #'!' R1
    store R1 0xFFF0
    load #'\n' R1
    store R1 0xFFF0
