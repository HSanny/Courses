import java.util.ArrayList;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
/** 
* @module Paragrapher.java
* @version 1.1 
* @since 2013-04-18
* @author Jimmy Lin (u5223173) - u5223173@uds.anu.edu.au
*   
* Edited by MacVim
* Class Info auto-generated by Snippet 
* */
public class Paragrapher {
    /** Fields */
    String label;
    ArrayList<Recognizer> recognizerList = new ArrayList<Recognizer> ();
    ArrayList<Block> blockList = new ArrayList<Block> ();
    /** Constructors */
    Paragrapher() {}
    Paragrapher(String name) {
        this.label = name;
    }

    /** evaluate whether the input string is a recognizer */
    boolean isRecognizer (String candidate) {
//{{{
        if (this.recognizerList.size() == 0) {
            return false;
        } else {
            // search the arraylist of recognizers to find result
            for (int i = 0; i < this.recognizerList.size(); i++) {
                Recognizer temp;
                temp = this.recognizerList.get(i);
                if (temp.getMarkUp().equals(candidate)) {
                    // the candidate exists in arraylist
                    return true;
                }
            }
            // after search, no matach found..
            return false;
        }
//}}}
    }

    /** factory method to produce instance of paragrapher */
    public static Paragrapher factoryParagrapher (String label, String [] rlist) {
        Paragrapher paragrapher = new Paragrapher (label);
        for (int i = 0 ; i < rlist.length; i++) {
            Recognizer tempReg = new Recognizer (rlist[i]);
            paragrapher.recognizerList.add(tempReg);
        }
        return paragrapher;
    }
    /** main console */
    public static void main (String [] args) throws IOException {
        for (int a = 0; a < args.length; a ++) {
            String filename = args[a];
            
            // construct functional recognizer utilized in our program.
            String [] rlist = {".p", ".h1", ".h2", ".h3"};
            // construct instance of class Paragrapher
            Paragrapher paragrapher = factoryParagrapher("Paragrapher:"+filename, rlist);
            BufferedReader buffer;
            // open and read the input text from inputText.txt
            try {
                buffer = new BufferedReader(new FileReader(filename));
            } catch (IOException e){
                System.out.println("-----------------------");
                System.out.println("Exception of IO detected..");
                System.out.println("Failure File Reading..");
                System.out.println("-----------------------");
                continue;
            }  
                System.out.println("-----------------------");
                System.out.println("NOW, READING FROM FILE: "+ filename);
                System.out.println("-----------------------");
            // process the text in the input file
            Block currentBlock = null;
            String line = buffer.readLine();
            for ( ; line != null; line = buffer.readLine()) {
                if (line.length() == 0) {
                    continue;
                }
                //  manipulation to "..." 
                //{{{
                if (line.equals("...")) {
                    // ignore triple full stop
                    currentBlock.addToken("\n");
                    // read next line or read the rest of this line
                    if (line.length() <= 4) { 
                        // this is a line with only triple full stop 
                        continue;
                    } else {
                        // read the rest strings in the line
                        line = line.substring(3);    
                    }
                } 
                //}}}
                // manipulation to command
                //{{{
                if (line.charAt(0) == '.') {
                    // this is a command line
                    String command;
                    int firstSpace = line.indexOf(' ');
                    if (firstSpace == -1) {
                        command = new String (line);
                    } else {
                        command = line.substring(0, firstSpace);
                    }
                    if (paragrapher.isRecognizer(command)) {
                        // detect that it's a recognizer
                        Recognizer reg = new Recognizer (command);
                        Block para = new Block (reg);
                        paragrapher.blockList.add(para);
                        currentBlock = para;
                    } 
                    // read next line or read the rest of this line
                    if (firstSpace == -1 || line.length() <= firstSpace + 1) {
                        // this is a line with only command 
                        continue;
                    } else {
                        line = line.substring(firstSpace); 
                    }
                }
                //}}}
                String [] tokens = line.split(" ");
                for (int j = 0 ; j < tokens.length ; j ++ ) {
                    String temp = tokens[j];
                    // process each word here...
                    // error handling for no initial recognizer.
                    if (currentBlock == null) {
                        System.out.println("Error, no initial recognizer.");
                        System.exit(0);
                    }
                    if (temp.length() != 0 ) {
                        currentBlock.addToken(temp);
                    }
                }
            }

            // after parsing input file, print out the result
            for (int i = 0; i < paragrapher.blockList.size(); i++) {
                Block tempPara = paragrapher.blockList.get(i);
                System.out.println(tempPara.toString());
            }
            // after reading the input file, close the buffer 
            buffer.close();
        }
    }
}
/** 
 * A class constructed to describe those markup like .p .h1 .h2 .h3
 * */
class Recognizer {
    private String markUp;
    /** constructor */ 
    Recognizer(String reg) {
        this.markUp = reg;
    }

    String getMarkUp () { return this.markUp;}
}

/** 
 * A class constructed to describe "general" paragraph, including the heading.
 * but for heading, we do not consider those three parameters..
 * */
class Block {
    /** fields */
    private int maxColumns = 0;
    private int initIndent = 0;
    private int blockIndent = 0;
    Recognizer recognizer;
    ArrayList<String> text;

    /** constructors */
    Block(Recognizer reg) {
        if (reg.getMarkUp().equals(".p")) {
            // initialization only for paragraph
            this.maxColumns = 72;
            this.initIndent = 4;
            this.blockIndent = 0;
        }
        this.text = new ArrayList<String> ();
        this.recognizer = reg;
    }

    /** add content to one paragraph */
    void addToken (String input) {
        this.text.add(input);
    }

    /** this method controls the mechanism to produce displayed output text */
    public String toString() {
        String str = "";
        String regName = this.recognizer.getMarkUp();
        // add both initial and block indentation to first line of .p paragraph 
        if (regName.equals(".p")) {
            for (int i = 0; i < this.initIndent + this.blockIndent; i ++) {
                str += " ";
            }
        } 
        // manipulate the content
        int numOfCols = this.initIndent;
        for (int i = 0; i < this.text.size(); i ++ ){
            // get the word we wanna analyze before output
            String tempStr = this.text.get(i);
            // start branches
            if (regName.equals(".p")) {
                if (numOfCols + tempStr.length() > this.maxColumns ) {
                    str += "\n";
                    // add block indentation for new line
                    for (int j = 0; j < this.blockIndent; j ++) {
                        str += " ";
                    }
                    numOfCols = this.blockIndent;
                }
                if (tempStr.equals("\n")) {
                    str += tempStr;
                    for (int j = 0; j < this.blockIndent; j ++) {
                        str += " ";
                    }
                } else {
                    str += tempStr + " "; 
                }
                numOfCols += tempStr.length() + 1;
            } else if (regName.equals(".h1")) {
                // for .h1 , we convert all letters to upper case
                str += tempStr.toUpperCase() + " ";
            } else if (regName.equals(".h2")) {
                // for h2 , we only convert the first letter to upper case
                if (tempStr.length() >= 2) {
                    str += tempStr.substring(0,1).toUpperCase() 
                           + tempStr.substring(1).toLowerCase() + " ";
                } else {
                    str += tempStr.toUpperCase() + " ";
                }
            } else if (regName.equals(".h3")) {
                // for h3, we convert all letters to lower case
                str += tempStr.toLowerCase() + " ";
            }
        }
        return str;
    }
}
