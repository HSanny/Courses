# -*- coding: utf-8 -*-
"""
@author: houck
This script converts .ex files input input files for emergent
"""

import os
import sys


if len(sys.argv) != 3:
    print "Error: this script requires the names of the input and output files as arguments"
    exit
if sys.argv[1] == sys.argv[2]:
    print "Error: input and output files cannot be the same"
    exit
print "Opening input file \"{}\"...".format(sys.argv[1])
input_file = open(sys.argv[1], 'r')
print "Opening output file \"{}\"...".format(sys.argv[2])
output_file = open(sys.argv[2], 'w')
print "Converting data..."

next_line = input_file.readline()
output_line = ""
section = "Header"
while next_line != "":
    if next_line[0] == '#':
        #begin new input or output data
        if next_line[1] == 'I':
            #next line is input
            section = "Input"
        else:
            section = "Output"
    elif section == "Header":
        #do nothing
        next_line = input_file.readline()
        continue
    elif section == "Output":
        #this is the label (and the last line of the example)
        output_line += next_line.strip("\n")
        labels = next_line.split()
        for label_index in range(len(labels)):
            if labels[label_index][0] == '1':
                output_line = "{}  ".format(label_index) + output_line
                break
        output_file.writelines(output_line + "\n")
        section = "Unknown"
        output_line = ""
    elif section == "Input":
        output_line += next_line.strip("\n")
    next_line = input_file.readline()

print "Done."

output_file.close()
input_file.close()