;*********************************************
; CourseCode: COMP2300
; Task: Assignment 02
; Version: 1.0
;
; Name: Jimmy Lin
; UID: U5223173
; Email: 
;   - linxin@gmail.com
;   - u5223173@uds.anu.edu.au
;
; Edited by MacVim 
;*********************************************

0x0100 : 
Init: ; set to zero for initialisation
    jump checkFormat
    jump end 

;*********************************************
;checkFormat: /*{{{*/
;    check validity of first 3 chars 
;   R2 for loading data from input stream
;   R3 for storing expected character constant
;   R4 for storing result of subtraction
;*********************************************
checkFormat: 
    ; load one char from stream
    ; check whether it is char 'R'
    load 0xFFF0 R2 
    load #'R' R3
    sub R2 R3 R4
    jumpnz R4 end
    ;store R3 0xFFF0 ;  test

    ; load one char from stream
    ; check whether it is char 'P'
    load 0xFFF0 R2 
    load #'P' R3
    sub R2 R3 R4
    jumpnz R4 end
    ;store R3 0xFFF0 ; test

    ; load one char from stream
    ; check whether it is char 'P'
    load 0xFFF0 R2 
    load #'I' R3
    sub R2 R3 R4
    jumpnz R4 end 
    ;store R3 0xFFF0 ; test

    ; print \n to start a new line
    load #'\n' R3
    ;store R3 0xFFF0 

    jump readWidth
;/*}}}*/

;*********************************************
;readWidth: /*{{{*/
;    get width of this image
;   R2 for loading one char from input stream
;   R3 for accumulator
;   R4 for storing constant 10
;   R5 for storing constant 48
;     (for conversion between char and int)
;   R6 for width of the image
;*********************************************
readWidth:
    ; load constant we need here.
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

    jump readHeight 
;/*}}}*/

;*********************************************
;readHeight: /*{{{*/
;    get height of this image
;   R2 for loading data from input stream
;   R3 for accumulator
;   R4 for storing constant 10
;   R5 for storing constant 48
;     (for conversion between char and int)
;   R7 for height of the image
;*********************************************
readHeight:
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
;/*}}}*/

;*********************************************
; Create Table: global /*{{{*/
; create tables for used operation
;   to accelerate the execution.
;*********************************************
createMultTable:
    move R7 R2
    load #6 R4
loopMultTable:
    mult R2 R4 R5
    store R5 #mult6table R2
    add MONE R2 R2
    jumpnz R2 loopMultTable

createModTable:
    move R6 R1
    load #32 R4
loopModTable:
    mod R1 R4 R5
    store R5 #mod32table R1
    div R1 R4 R5
    store R5 #div32table R1
    add MONE R1 R1
    jumpnz R1 loopModTable

    jump readMode
;/*}}}*/

;*********************************************
;readMode: /*{{{*/
;   get height of this image 
;   R2 for loading data from input stream
;   R5 for storing expected char constant
;      and subtraction result
;   R6 for width of the image
;   R7 for height of the image
;*********************************************
readMode:
    ; load input character from stream
    load 0xFFF0 R2
    ;store R2 0xFFF0
    ;load #'\n' R0
    ;store R0 0xFFF0

    ; check for binary mode
    load #'B' R5
    sub R2 R5 R5
    jumpz R5 readBinary

    ; check for hexidecimal mode
    load #'X' R5
    sub R2 R5 R5
    jumpz R5 readHexdecimal

    ; check for run-length mode
    load #'R' R5
    sub R2 R5 R5
    jumpz R5 readRunLength
;/*}}}*/

;*********************************************
;readBinary: /*{{{*/
;   - read binary image to memory
;   - display the image in the screen
;*********************************************
readBinary:
    ; Initialisation for reading binary image
    load #0 R1 ; index of x
    load #0 R2 ; index of y
    ; Construct effective ascii table
    load #asciitable R5
    store ZERO #48 R5
    store ONE #49 R5

loopBinary:
    load 0xFFF0 R0 ; read binary char
    load R0 #asciitable R0 ; get color value in currently reading pixel: 0 or 1

    jumpz R0 incrementX ; no need to change data, continue
    ; if R0 is not zero, naturally go to alterMemory program

alterMemory:
    ; work out x and y coordinate for current pixel
    load R1 #div32table R3
    load R2 #mult6table R4
    ; compute corresponding memory address for current pixel
    add R3 R4 R5
    load R5 #0x7C40 R4 ; R4 is value in dest address

    ; get times of left shift
    load R1 #mod32table R3 
    ; shift the bit where the value of current pixel is given
    rotate R3 ONE R0

    ; after shifting, get data in stack back
    or R0 R4 R4 ; R4 is expected image unit
    store R4 #0x7C40 R5 ; display on the screen

incrementX:
    add R1 ONE R1 ; x++
    sub R6 R1 R4 ; compute how many pixels left in this row
    ; if there are still pixels to address in this row
    jumpnz R4 loopBinary
    ; if R4 is zero, naturally go to program that increment Y

incrementY:
    move ZERO R1 ; x index clear to 0
    add R2 ONE R2 ; y ++
    sub R7 R2 R4 ; compute how many rows left to deal with

    ; if still some rows to address, continue the loop 
    jumpnz R4 loopBinary
    ; if R4 is zero, naturally end up this program
endBinary: 
    jump end
;/*}}}*/

;*********************************************
;readHexdecimal: 
;   - read hexdecimal image to memory
;   - display it on the screen 
;   R1 register for horizontal coordinate: x 
;   R2 register for vertical coordinate: y
;*********************************************
readHexdecimal:
    ; Initialisation for reading Hexdecimal image
    load #0 R1 ; index of x
    load #0 R2 ; index of y


;*********************************************
; Create Tables: Hexdecimal /*{{{*/
;   Construct a bunch of tables to shorten
; the CPU cycles to be executed.
;  - reverse table (significance)
;  - ascii table to decode character
;  - add4table
;*********************************************
createReverseTable:
    load #15 R0 ; loop from 15 to zero

loopReverse:
    move ZERO R3
    load #1 R4
    and R0 R4 R5
    load #8 R4
    mult R4 R5 R5
    add R3 R5 R3

    load #2 R4
    and R0 R4 R5
    div R5 R4 R5
    load #4 R4
    mult R4 R5 R5
    add R3 R5 R3

    load #4 R4
    and R0 R4 R5
    div R5 R4 R5
    load #2 R4
    mult R4 R5 R5
    add R3 R5 R3

    load #8 R4
    and R0 R4 R5
    div R5 R4 R5
    load #1 R4
    mult R4 R5 R5
    add R3 R5 R3

    store R3 #reversetable R0
    add MONE R0 R0
    jumpn R0 createAsciiTableHexdecimal
    jump loopReverse

; create mapping from ascii code to hexdecimal code
createAsciiTableHexdecimal:
    load #10 R5
    load #47 R4
loopDigit: ; decode for 0-9
    add R5 R4 R3
    add MONE R5 R5
    store R5 #asciitable R3
    jumpnz R5 loopDigit
letter: ; decode for A-F
    load #6 R5
loopLetter:
    load #64 R4
    add R4 R5 R3
    load #9 R4
    add R4 R5 R0
    store R0 #asciitable R3
    add MONE R5 R5
    jumpnz R5 loopLetter

createAdd4Table:
    load #192 R3
    load #4 R4

loopAdd4Table:
    add R3 R4 R5
    store R5 #add4table R3
    add MONE R3 R3
    jumpn R3 loopHexdecimal
    jump loopAdd4Table
;/*}}}*/

;/*{{{*/ hexdecimal loop
loopHexdecimal:
    ; read data from stream
    load 0xFFF0 R0

ascii2numeric:
    ; for conversion from ascii to numeric value
    load R0 #asciitable R0

reverse:
    load R0 #reversetable R0

branching:
    load R1 #add4table R5 ; R5: x + 4
    sub R6 R5 R4 ; R4: width - (x  + 4)
    jumpn R4 push2rdPartInfoNextRow ; bit mode triggered by end row

    jumpz R0 blockIncrementX; purely-black pixel no need to change
    load R1 #mod32table R4
    load R4 #add4table R4
    load #32 R5
    sub R5 R4 R4 ; R4: 32 - (x mod 32 + 4)
    jumpn R4 push2rdPartInfoNextBlock ; bit mode triggered by crossing blocks
    jump initBlockWriteMemory ; directly store value of four pixels

push2rdPartInfoNextBlock:
    load #4 R5
    add R4 R5 R3; R3 first part length = 32 - x mod 32
    add R3 R1 R5
    push R2 ; y for second part
    push R5 ; x for second part
    mult MONE R4 R5
    push R5 ; length of second part
    jump initBitWriteMemory

push2rdPartInfoNextRow:
    add ONE R2 R5 ; y ++
    push R5 ; y for second part
    push ZERO ; x for second part
    mult MONE R4 R5
    push R5; length of second part
    jump initBitWriteMemory

initBitWriteMemory:
    pop R3 ; R3: number of bit on second part
    push R3 ; 
    load #4 R5
    sub R5 R3 R5
    push R5 ; R5: number of bit on first part
    load #15 R4 ; make a copy to be executed 
    
    mult MONE R3 R3
    rotate R3 R4 R4 ; right-ward bit shift
    and R0 R4 R4 ; R4 is value for first part

    load R1 #mod32table R5 ; leftward shift amount
    rotate R5 R4 R4

BitWriteMemoryFirstPart:
    ; work out memory address for screen display 
    ; based on x and y coordinate for current pixel
    load R1 #div32table R3
    load R2 #mult6table R5
    add R3 R5 R3 ; R3 =  x/32 + y * 6
    ; load word from specified memory unit
    load R3 #0x7C40 R5 
    ; compute resulted memory unit
    or R5 R4 R4 
    ; restore to memory unit for screen display
    store R4 #0x7C40 R3

initBitWriteMemorySecondPart:
    pop R3 ; number of bit in first part
    pop R5 ;  second part length
    pop R1 ; x 
    pop R2 ; y
    push R5; number of bit in the second part
    sub R7 R2 R4
    jumpz R4 endHexidecimal

    mult MONE R3 R3 ; shift amount is negative
    rotate R3 R0 R4
    load #15 R5
    and R4 R5 R4

BitWriteMemorySecondPart: 
    ; work out memory address for screen display 
    ; based on x and y coordinate for current pixel
    load R1 #div32table R3
    load R2 #mult6table R5
    add R3 R5 R3 ; R3 = x/32 + y * 6

    load R3 #0x7C40 R5  ; load word from specified memory unit
    or R5 R4 R4 ; compute resulted memory unit

    ; restore to memory unit for screen display
    store R4 #0x7C40 R3
    pop R5 
    add R1 R5 R1
    ; bit-by-bit display done ..
    jump loopHexdecimal

initBlockWriteMemory:
    ; compute bits to shift
    load R1 #mod32table R5
    rotate R5 R0 R0

blockWriteMemory:
    ; work out memory address for screen display 
    ; based on x and y coordinate for current pixel
    load R1 #div32table R3
    load R2 #mult6table R5
    add R3 R5 R3 ; R3 =  x/32 + y * 6
    ; load word from specified memory unit
    load R3 #0x7C40 R4 
    ; compute resulted memory unit
    or R0 R4 R4 
    ; restore to memory unit for screen display
    store R4 #0x7C40 R3

blockIncrementX:
    load R1 #add4table R1
    sub R6 R1 R5 ; compute how many pixels left in this row
    jumpz R5 blockIncrementY
    ; if there are still pixels to address in this row
    jump loopHexdecimal

blockIncrementY:
    move ZERO R1 ; x index clean to 0
    add R2 ONE R2 ; y ++
    sub R7 R2 R4 ; compute how many rows left to deal with

    ; if still some rows to deal with, continue the loop 
    jumpnz R4 loopHexdecimal
    ; if no more rows to deal with, end up

endHexidecimal:
    jump end
;/*}}}*/

;*********************************************
;readRunLength: 
;   read run-length image to memory
;   - display it on the screen 
;
;   R6 - image width 
;   R7 - image height
;   R0 - current input character
;*********************************************
readRunLength:
;/*{{{*/ create tables
    ; create mapping from ascii code to hexdecimal code
createAsciiTableRunLength:
    load #10 R5
    load #47 R4
loopDigitRunLength: ; decode for 0-9
    add R5 R4 R3
    add MONE R5 R5
    store R5 #asciitable R3
    jumpnz R5 loopDigitRunLength
letterRunLength: ; decode for A-F
    load #6 R5
loopLetterRunLength:
    load #64 R4
    add R4 R5 R3
    load #9 R4
    add R4 R5 R0
    store R0 #asciitable R3
    add MONE R5 R5
    jumpnz R5 loopLetterRunLength

createAllOnestable:
    load #33 R5 ; times
    move ZERO R4; offset
    move ZERO R3 ; R3 acc
loopAllOnestable:
    store R3 #allOnestable R4
    rotate ONE R3 R3 ; acc * 2
    add R3 ONE R3 ; acc + 1
    sub R5 ONE R5 ; times --
    add ONE R4 R4
    jumpnz R5 loopAllOnestable
;/*}}}*/

initRunLength:; /*{{{*/
    load #0 R1 ; denote x
    load #0 R2 ; denote y
    store ZERO #isWhite ZERO
loopRunLength:
    load 0xFFF0 R0
    load R0 #asciitable R0
    rotate #4 R0 R3

    load 0xFFF0 R0
    load R0 #asciitable R0
    add R3 R0 R3
    rotate #4 R3 R3

    load 0xFFF0 R0
    load R0 #asciitable R0
    add R3 R0 R3
    rotate #4 R3 R3
    
    load 0xFFF0 R0
    load R0 #asciitable R0
    add R3 R0 R0 ; R0 restores the number currently processed
    
bifurcate:
    load ZERO #isWhite R5
    jumpnz R5 processWhite
    ; otherwise, naturally go to black processing

processBlack: 
    store ONE #isWhite ZERO ; next time is for white
    ; for black, just modify current x, y coordinate
    ; no need to change memory
    sub R6 R1 R5 ; R5 = width - x
    sub R0 R5 R3 ; R3 = length - (width - x)
    jumpn R3 blackWithinOneLine
blackMultipleLine:
    add ONE R2 R2 ; y ++
    move ZERO R1 ; x = 0
    sub R7 R2 R4
    jumpz R4 endRunLength
    move R3 R0 ; length = length - (width - x)
    jump processBlack ; solve this line, continue
blackWithinOneLine:
    add R0 R1 R1 ; add remained slot to x
    sub R6 R1 R5 ; check for need to next line
    jumpnz R5 loopRunLength ; continue current line
    move ZERO R1
    add ONE R2 R2; switch to next line
    jump loopRunLength

processWhite:
    store ZERO #isWhite ZERO ; next time is for black
    load #32 R4 
    sub R6 R1 R3 ; width - x
    sub R3 R4 R5 ; width - x - 32
    jumpn R5 endRowOrNot

    load R1 #mod32table R3 ; dist to head
    sub R4 R3 R4 ; dist to tail
    sub R0 R4 R5 ; length - distToTail
    jumpn R5 whiteWithinOneBlock
    jump whiteCrossBlock

endRowOrNot:
    sub R3 R0 R4
    jumpn R4 whiteEndRow
    load R1 #mod32table R3
    load #32 R4
    sub R4 R3 R4 ; dist to tail
    sub R4 R0 R5 ; dist to tail- length
    jumpn R5 whiteCrossBlock
    jump whiteWithinOneBlock

whiteEndRow:
    sub R0 R3 R0 ; update the remained length
    load R3 #allOnestable R4
    load R1 #mod32table R3
    rotate R3 R4 R5 ; R5 data to be displayed
    load R1 #div32table R3 ; x / 32
    load R2 #mult6table R4 ; y * 6
    add R3 R4 R3
    load R3 #0x7C40 R4
    or R4 R5 R5
    store R5 #0x7C40 R3
    jump runLengthIncrementY

whiteCrossBlock:
    ; length = distToTail
    ; length > distToTail
    load R4 #allOnestable R5
    rotate R3 R5 R5
    push R4 ; dist to tail
    load R1 #div32table R3 ; x / 32
    load R2 #mult6table R4 ; y * 6
    add R3 R4 R3
    load R3 #0x7C40 R4
    or R4 R5 R5
    store R5 #0x7C40 R3
    pop R4 ; dist to tail
    sub R0 R4 R0 ; length = length - distToTail
    add R1 R4 R1 ; x = x + distToTail
    sub R6 R1 R5 ; width - x
    jumpz R5 runLengthIncrementY ; remained length displayed in next row
    jumpz R0 loopRunLength ; no more length in white, read next data

    jump processWhite ; display current data in next block

runLengthIncrementY:
    add ONE R2 R2 ; y ++
    move ZERO R1 ; x = 0
    sub R7 R2 R4 ; 
    jumpz R4 endRunLength
    jumpz R0 loopRunLength ; no more length in white, read next data
    jump processWhite ; display current data in next block

whiteWithinOneBlock:
    load R0 #allOnestable R4
    rotate R3 R4 R5 ; R5 data to be displayed
    load R1 #div32table R3 ; x / 32
    load R2 #mult6table R4 ; y * 6
    add R3 R4 R3
    load R3 #0x7C40 R4
    or R4 R5 R5
    store R5 #0x7C40 R3
    add R0 R1 R1 ; x = x + length
    sub R6 R1 R3
    move ZERO R0
    jumpz R3 runLengthIncrementY
    jump loopRunLength
    
judgeHaltRunLength:
    sub R6 R1 R5
    jumpn R5 endRunLength
    ;jumpnz R5 ret

endRunLength:
    jump end
;/*}}}*

;*********************************************
;end:
;   mark the end of this program.
;*********************************************
end: 
    halt


; variable and array declaration
asciitable: block 128
mod32table: block 193
div32table: block 193
mult6table: block 169
add4table: block 193
reversetable: block 16
isWhite: block #0 ; 0 for black turn, 1 for white
allOnestable: block 33
