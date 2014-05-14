; Author: Zhe Weng
;UID : U5044856
;Version: 3.01

0x0100 : 
input:  load 0xfff0 R0
	load #0 R1
	load #0 R2
	load #0 R3
	load #0 R4
	load #0 R5
	load #0 R6
	load #0 R7

halt:	jumpnz R0 enter
end:	halt
	halt

enter:	load #0x0a R1
	sub R0 R1 R0
	jumpnz R0 space
	jump input
	
space:	load #0x16 R1
	sub R0 R1 R0
	jumpnz R0 fact
	jump input
	

fact:	load #0x1 R1
	sub R0 R1 R0
	jumpnz R0 remain
	call ffact
	jump followenter
	jump input

remain: load #0x4 R1
	sub R0 R1 R0
	jumpnz R0 mul
	pop R1
	pop R0
	mod R0 R1 R0
	push R0
	jump followenter
	jump input

mul:	load #0x5 R1
	sub R0 R1 R0
	jumpnz R0 adder
	pop R1
	pop R0
	mult R0 R1 R0
	push R0
	jump followenter
	jump input

adder:	load #0x1 R1
	sub R0 R1 R0
	jumpnz R0 subtra
	pop R1
	pop R0
	add R0 R1 R0
	push R0
	jump followenter
	jump input

subtra:	load #0x2 R1
	sub R0 R1 R0
	jumpnz R0 divi

;********************************
; maybe it's a negative num   ***
;********************************

	load 0xfff0 R0

	load #0xa R1
	sub R0 R1 R0
	jumpnz R0 subnr
	load #1 R7

subnr:	load #0x26 R1
	sub R0 R1 R0
	jumpn R0 subcon
	load #-1 R1
	mult R0 R1 R0
	push R0
	jump tnum
	

subcon:	pop R1
	pop R0
	sub R0 R1 R0
	push R0
	sub R7 ONE R7
	jumpz R7 cprint
	jump input

divi:	load #0x2 R1
	sub R0 R1 R0
	jumpnz R0 num
	pop R1
	pop R0
	div R0 R1 R0
	push R0
	jump followenter
	jump input

num:	load #10 R1
	sub R0 R1 R0
	jumpn R0 newnum
	jumpnz R0 power
newnum:	load #0 R3
	push R3
	load #9 R1
	add R0 R1 R0
cont:	pop R2
	load #10 R1
	mult R1 R2 R2

;********************************
; if it's a negative num      ***
;********************************
	jumpn R2 negnum
	add R0 R2 R0
	jump pnum

negnum: sub R2 R0 R0
pnum:	push R0
	
tnum:	load 0xfff0 R0
	load #0xa R1
	sub R0 R1  R0
	jumpz R0 cprint

	load #0x26 R1
	sub R0 R1 R0
	jumpn R0 input
	jump cont


power:	load #0x25 R1
	sub R0 R1 R0
	jumpnz R0 error
	call fpower
	pop R0
	jump followenter
	jump input

error: halt

;************************************
; it's testing whether following  ***
; is enter:   			  ***
;	if so, print current num  ***
;	otherwise, jump back	  ***
;************************************

followenter:
	load 0xfff0 R0
	load #0xa R1
	sub R0 R1  R0
	jumpnz R0 followout
cprint:	call fprint
	pop R0
followout:
	jump input


;************ factorial function ****
; int fact(int a){
;   if (a == 0)
;      return 1;
;   else
;      return a*fact(a-1);
; }

; stack frame:
; return address #0
; a #-1
; result #-1
;************************************

ffact:	load SP #-1 R0
	jumpnz R0 step1
	return
step1:	load #1 R1
	sub R0 R1 R0
	jumpnz R0 step2
	return
step2:	push R0
	call ffact
	pop R0
	load SP #-1 R1
	mult R0 R1 R0
	store R0 #-1 SP
	return
;//////////////////////////////////


;************ power function ******
; int power(int a,int b){
;   int temp=1;
;   while(b){
;      temp*=a;
;      b--;
;   }
;   return temp;
; }

; stack frame:
; return address #0
; b #-1
; a #-2
; result #-1
;***********************************

fpower: load SP #-1 R1
	load SP #-2 R0
	move ONE R2

	jumpn R1 fpn
	jumpz R1 fp0

fpn0:	sub R1 ONE R1
	mult R0 R2 R2
	jumpnz R1 fpn0
; one  
fp0:	store R2 #-2 SP
; negative ignore
fpn:	return
;//////////////////////////////////



;************ print function *****************
; print function convert 2's completement
; num into ASCII num by using power function
; e.g: 2345678 in 2's completement will be 
; converted as 0x32,0x33,0x34,0x35,0x36,0x37,
; 0x38, then send to I/O port
;*********************************************
fprint:	load SP #-1 R0
	jumpn R0 nega

fpcont:	load #10 R4
	load #0 R3
prloop:	sub R4 ONE R4

	load #10 R5 
	push R5
	push R4
	call fpower
	pop R5
	pop R5
	load SP #-1 R0
	div R0 R5 R1
	mod R0 R5 R0
	store R0 #-1 SP
	jumpnz R3 prnz
	jumpz R1 prcond
	load #1 R3
prnz:	load #0x30 R6
	add R1 R6 R1
	store R1 0xfff0
prcond:	jumpnz R4 prloop
	
	jumpnz R3 prlast
	jumpz R1 prnz
	
prlast:	load #0xa R1
	store R1 0xfff0
	return

nega:	load #-1 R1
	mult R0 R1 R0
	load #0x2d R1
	store R1 0xfff0
	store R0 #-1 SP
	jump fpcont
