import java.util.Scanner;
import types.*;
import utilities.*;
import visitors.*;

/** 
* @module Breadth.java
* @version 1.0 
* @since 2013-04-30
* @author Jimmy Lin (u5223173) - u5223173@uds.anu.edu.au
* 
* Edited by MacVim
* Class Info auto-generated by Snippet 
*/
class Breadth {

    public static void main (String [] args) {
    
    // utility initialisation
    Scanner input = new Scanner(System.in);
    utilities.Compiler compiler = new utilities.Compiler();

    // variable initialisation
    // read first line from input stream
    String line = "";
    line = input.nextLine().toLowerCase().trim();

    // loop for repeated reading
    while (!line.equals("q")) {

        if (line.length() > 0) {
            switch (line.charAt(0)) {
                case 'n':
                    // separate different expression result display
                    System.out.println();
                    // output original expression in the test file.
                    System.out.println(line.replaceFirst("n", "").trim());
                    compiler.setInput(line.replaceFirst("n", "").trim());
                    compiler.buildTree();
                    break;
                case 'i':
                    // redefined method. 
                    // please see utilities/Compiler.java
                    compiler.printInOrder();
                    break;
                case 'b': 
                    // newly added command: 'b'
                    // print the result of breadth first tranversal
                    compiler.printBreathFirst();
                    break;
                    // see the Compiler.java for implementation of
                    // Breath first order
                case 'v':
                    compiler.printValue();
                    break;
                case 'h':
                    instruct();
                    break;
                default:
                    warn();
                    break;
            }
        }
        line = input.nextLine().toLowerCase().trim();
    }
    }

    /** Give instructions on how to use this thing. */
    static void instruct() {
        System.out.println("Enter one of the following commands:");
        System.out.println("  n e - Enter a New expression");
        System.out.println("  i   - Print out the Inorder form");
        System.out.println("  o   - Print out the pOstorder form.");
        System.out.println("  r   - Print out the pReorder from.");
        System.out.println("  v   - Print out the Value.");
        System.out.println("  h   - View these instructions again.");
        System.out.println("  q   - Quit");
        System.out.println("Here e stands for an arithmetic expression");
        System.out.println("involving integers, parentheses and the four");
        System.out.println("arithmetic operations: +, -, * and /.  All");
        System.out.println("operations are applied to the last");
        System.out.println("expression entered.  You must enter an");
        System.out.println("expression before you can apply any operation.\n");
    }

    /** Warning on incorrect use. */
    static void warn() {
        System.out.println("Your command was not in the correct format.");
        System.out.println("Use the h command for details.");
    }

    /** The prompt we will give the user. */
    static final String prompt = "> ";

}
