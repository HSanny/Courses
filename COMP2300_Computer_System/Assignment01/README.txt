0. Overview
This Documentation includes the components as follows:
    - Copyright Statement
    - Contact Information
    - Changelog
    - User Instruction (command line instruction)
    - Tests (see the test.sh)
    - Optional Extension
    - Limitations and possible improvement
    - Summary of experience
For better mark my assignment, please see the User Instruction where presents all my implementations and its coresponding usage explanation.

1. Copyright Statement

    © 2013, Jimmy Lin (u5223173 ANU)
    ALL RIGHTS RESERVED. 

    This program contains material submitting for educational purpose. Any unauthorized reprint or use of this material is prohibited. No part of this program may be reproduced or transmitted in any form or by any means, electronic or mechanical, including photocopying, recording, or by any information storage and retrieval system without express written permission from the author. 

2. Contact Information

    Jimmy Lin (u5223173)
    Undergraduate R&D in Advanced Computing
    College of Computer Science
    Australian National University

    Emails:
        - linxin@gmail.com
        - u5223173@uds.anu.edu.au
    Phones:
        (+61) 0451028335 - Australia
        (+86) 18605713842 - China

3. Changelog
    2013/03/01 Version 1.0 comes out. This is the first runnable version. (10 hours)
    - Functionality includes argument recognition, bmp file reading into memory, pixel processing. 
    - Besides, initial version of README file with copyright, contact information is successfully generated as well. 

    2013/03/02 Version 1.1 comes out. (5 hours)
    - Functionalities of cropping images (belonging to the extension part of the task specification) is incorporated.
    - On top of that, I also work out the User Instruction and Summary of Experience. 

    2013/03/03 - version 1.2 runnable version added with functionality of cropping image released. (7 hours)
    - Refactor the codes with a good sematics and improve the readability of the codes. 
    - Exception handling for file I/O is available as well. 
    - New version of user instruction. 
    - Generate Bash script for testing.
    - Arguments verification mechanism (for cropping argument and treshold in particularly). 

    2013/03/04 - version 1.3 more elegant version with rotation functionality released (4 hours)
    - Add prototype for each function in .c file
    - Add perpendicular rotation
    - Add more examples in test.sh
    - Ameliorate some of code blocks, make it more elegant 

    2013/04/14 - version 1.4 implements bluring and final submission (3 hours)
    - implement the bluring accepting the WINDOWSIZE parameter
    - rewrite the bash script by adding loop mechanism (more test case)
    - final check and submission 

4. User Instruction
  For more, you can see the bash script test.sh in my submission.
  Command:
    ./bmpedit [-o FILE] [-t 0.0-1.0] [-c width height X Y] [-h] FILE
  Note that FILE above must be file with .bmp suffix.

  Mode Specification:
    mode    Param                Usage Details
    -o      FILE        Sets the output file for modified images
    -the   0.0-1.0      Sets the THRESHOLD to filter the image through
    -c                  Four parameters are compulsory as shown below
            Width       Width of cropped image (in pixels)
            Height      Height of cropped image (in pixels)
             X          x-position of the leftmost remained pixel after being cropped
             Y          y-position of the remained bottom pixel after being cropped
    -r      degree      Degree of rotation (positive if clockwise and must be multiple of 90)
    -b     windowSize   Blur processing
    -h		        Displays this usage message

5. Optional Extension
As you can see in the user instruction, the optional extensions I have achieved are as follows:
    (1). Cropping the image (width, height, x and y of the left-bottom remained pixel are needed) 
    (2). Rotating the image (degree of rotation is needed)
    (3). Bluring the image (size of bluring window needed)
6. Limitations and improvement
    (1) Rotation is only available for multiple of 90 degrees, further improvement lies in the larger freedom for the available degrees.
    (2) Bluring can be functioned on a specified region of the image. (can be rectangle, circle, or some other form).
    (3) For the command line input, all parameters are compulsory. Optionality should be achieved by detecting the blank arguments and set default value to the parameters. (not implemented yet)

7. Summary
    - After a long time not using C programming language, I am tremendously oblivious to C syntax and programming conventions. And I lose the familarity to some complicated structure, such as 2-dimensional arrays for pointers when I applied indexing between pixels so as to reduce the time complexity of element access (Size of image would not change frequently). Fortunately, I work it out by manually specifying two-order pointer finally and it is rather convenient. Actually, what I do is to replace the 2-d array(should be regarded as a pointer as well) with self-specified pointer, which could be the oen of the most characteristic feature of C programming language.
    - Invoking the FILE object after closing that file by using "fclose(FILE object)", there would be no error feeding back but the binary information read then would be weird, that is, all the same values.
    - It would be prefered to pass pointer between functions rather than object entities.
    - Exception Handler should be added once file I/O tasks are involved in programming. (Things like judge for existence of input file and output directory ought to be taken sufficient attention.)
    - Writing Bash script is one considerable and effective way as a project-level test.
