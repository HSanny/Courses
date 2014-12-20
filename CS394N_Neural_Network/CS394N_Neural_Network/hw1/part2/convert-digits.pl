#!/usr/bin/perl
#
# For SOM/LVQ NN  hwk  partII
#
#  6 Aug 98    Digit version by Tal Tversky
# 12-Oct-1998: Minor tweaks for CS342.
# 
# This script takes a file of digit training vectors, output from
# the "digit" program and converts them to a format that the sofm 
# program expects.  
# It takes three parameters, in order: the filename of your digit
# data file, the desired output vector file name, and the desired 
# output label file name.
#

if ($#ARGV != 2) {
    print "Usage: ./convert-digits.pl train.pat train.vectors train.labels\n";
    exit;
}

print "\n";
print "Generating vector file $ARGV[1]\n";
print "        and label file $ARGV[2]\n";
print "       from digit file $ARGV[0] ...\n";
print "Done.\n";
print "\n";

# Hardcoded -- number of input units
$patternwidth = 64;


open(DIGITFILE,"$ARGV[0]") or die "Can't open $ARGV[0]\n";
open(VECTORFILE,">$ARGV[1]") or die "Can't open $ARGV[1]\n";
open(LABELFILE,">$ARGV[2]") or die "Can't open $ARGV[2]\n";

# Make vector file (parameter 2)
#
#  Print header with name of label file and input dimensionality
#
print VECTORFILE "$ARGV[2]\n$patternwidth\n";

while ($line = <DIGITFILE>){
    if ($line =~ /#\s*Input/) {
        while ($line = <DIGITFILE>) {
            if ($line =~ /#\s*Output/) { # print correct digit to label file
                print VECTORFILE "\n";
                $line = <DIGITFILE>;
                ## Split the line into a fields by whitespace
                @fields = split /\s/, $line;
                $count = 0;  #count will tell us which place the 1 is in
                foreach $field (@fields) {
                    if ($field == 1) {last;}
                    $count++;
                }
                print LABELFILE "$count\n";
                last;
            }
            # print correct pattern to vector file
            $line =~ s/\n//;
            print VECTORFILE "$line";
        }
    }
}
