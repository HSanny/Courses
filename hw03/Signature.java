/** 
* @module Signature.java
* @version 1.2 
* @since 2013-03-05
* @author Jimmy Lin (u5223173) - u5223173@uds.anu.edu.au
*
* Description:
*     This code is extended based on my own homework in week 02.
* 
* Edited by MacVim
* Class Info auto-generated by Snippet 
*/


// import library
//{{{
import java.io.IOException;
import java.io.FileReader;
import java.io.FileNotFoundException;
import java.io.BufferedReader;
import java.util.ArrayList;
//}}}

/**
 *  Store information of each one java code file.
 * Every time we invoke the examineFile method, one record would be returned..
 * The purpose of writing this class is to better keep track of the examination of code files..
 * */
class Record{
//{{{
    /** Field */
    //{{{
    int braceRegister = 0;
    String currentClassName;

    int lineOfComments = 0;
    int lineOfBlanks = 0;
    int lineOfCodes = 0;
    int lineOfBraceOnly = 0;
    int lineOfTotal = 0;
    ArrayList<Class> classList = new ArrayList<Class>();
    String filename;
    //}}}
     
    /** constructor */
    public Record(String filename) { 
        this.filename = filename;
    }
    /** Add method into one class's method list */
    public void addMethod(MethodMember method){
        Class tempClass;
        for (int i = 0; i < this.classList.size(); i++){
            tempClass = this.classList.get(i);
            if (tempClass.className.compareTo( method.ownerClass) == 0){
                tempClass.methodList.add(method);
                break;
            } else {
                continue;
            }
        }
        return ;
    }
    /** Add field into one class's method list */
    public void addField(FieldMember field){
        Class tempClass;
        for (int i = 0; i < this.classList.size(); i++){
            tempClass = this.classList.get(i);
            if (tempClass.className.compareTo( field.ownerClass) == 0){
                tempClass.fieldList.add(field);
                break;
            } else {
                continue;
            }
        }
        return ;
    }

    /** Evaluate whether one class has pre-exist in the class list of this record */
    public boolean isClassInList(String name){
        boolean isInlist = false;
        for (int i = 0 ; i < this.classList.size() ; i++){
            if ( this.classList.get(i).className.compareTo(name) == 0) {
                isInlist = true;
                break;
            } else {
                continue;
            }
        }
        return isInlist;
    }

    public int getNumOfMethodAccess(Access acc) {
        Class tempc;
        int num = 0 ;
        for (int i = 0 ; i < this.classList.size() ; i ++){
            tempc = this.classList.get(i);
            num += tempc.getNumOfMethodAccess(acc);
        }
        
        return num;
    }
    public int getNumOfFieldAccess(Access acc) {
        Class tempc;
        int num = 0 ;
        for (int i = 0 ; i < this.classList.size() ; i ++){
            tempc = this.classList.get(i);
            num += tempc.getNumOfFieldAccess(acc);
        }
        
        return num;
    }

    /** Format the String according to requirment */
    public String toString_fieldSummary(){
        String str = "";
        str += "    Field: Public(+) = " + this.getNumOfFieldAccess(Access.PUBLIC);
        str += "    Private(-) = " + this.getNumOfFieldAccess(Access.PRIVATE);
        str += "    Protected(#) = " + this.getNumOfFieldAccess(Access.PROTECTED);
        str += "    PACKAGE_ACCESS(~) = " + this.getNumOfFieldAccess(Access.PACKAGE_ACCESS);
        str += "\n";
        return str;
    }
    public String toString_methodSummary(){
        String str = "";
        str += "    Method: Public(+) = " + this.getNumOfMethodAccess(Access.PUBLIC) ;
        str += "    Private(-) = " + this.getNumOfMethodAccess(Access.PRIVATE);
        str += "    Protected(#) = " + this.getNumOfMethodAccess(Access.PROTECTED);
        str += "    PACKAGE_ACCESS(~) = " + this.getNumOfMethodAccess(Access.PACKAGE_ACCESS);
        str += "\n";
        
        return str;
    }
    public String toString_hm03(){
        String str = "    "+ this.classList.size()+ " Class(es) in "+ this.filename +"\n";
        for (int i = 0; i < this.classList.size() ; i++ ){
            str += "    #"+ (i+1) + "."+ this.classList.get(i).toString();
            // this line is to test BR after parsing one class
            //str += "BR:"+this.braceRegister + "\n"; 
        }
        str += this.toString_fieldSummary()  + this.toString_methodSummary() ;
        return str;
    }
    public String toString_hm03_sum(){
       String str = String.format("%4d %s\n",this.lineOfCodes,this.filename);
       str += this.toString_fieldSummary() + this.toString_methodSummary();
       return str; 
    }

    public String toString_hm02(){
//{{{
        String str;
        str = "The file [" +this.filename+"] has "+ this.lineOfTotal + " lines, precisely:\n";
        str += "    " + this.lineOfCodes + " Code Lines...\n";
        str += "    " + this.lineOfComments + " Comment Lines... \n";
        str += "    " + this.lineOfBlanks + " Blank Lines... \n";
        str += "    " + this.lineOfBraceOnly + " Brace-Only Lines...\n";
        str += "    " + (this.lineOfComments+this.lineOfBlanks +this.lineOfBraceOnly +this.lineOfCodes) + " recognized Lines...\n"; 
//}}}
        return str;
    }//}}}
}

/**  implement the toString method
 * Use Structure to represent a class */
class Class {
//{{{
    static final String SEP = "------------------------------------------------------------------------------------------------\n"; 
    Access access;
    String className;
    ArrayList<MethodMember> methodList = new ArrayList<MethodMember> ();
    ArrayList<FieldMember> fieldList = new ArrayList<FieldMember> ();
    /** Constructor */
    public Class (String name) {
        this.className = name;
    }
    /** return how many acesses are used in the methods of this class */
    public int getNumOfMethodAccess (Access acc){
        MethodMember tempm;
        int num = 0;
        for (int i = 0 ; i < this.methodList.size(); i ++){
            tempm = this.methodList.get(i);
            if (tempm.access == acc) {
                num ++;
            }
        }
        return num;
    }
    /** return how many acesses are used in the fields of this class */
    public int getNumOfFieldAccess (Access acc){
        FieldMember tempf;
        int num = 0;
        for (int i = 0; i < this.fieldList.size(); i++){
            tempf = this.fieldList.get(i);
            if (tempf.access == acc) {
                num ++;
            }
        }
        return num;
    }

    public String toString () {
        String str = "Class:"+ this.className;
        str += "  Access:"+ this.access.getSpecifier() + "(" + this.access.getNotation() + ")" ;
        str += " #Field: " + this.fieldList.size() + "  #Method: " + this.methodList.size()+ "\n";
        for (FieldMember mem: this.fieldList){
            str += mem.toString();
        }
        if (this.fieldList.size() != 0){
            str += "    " + this.SEP;
        }
        for (MethodMember mem: this.methodList) {
            str += mem.toString();
        }
        if (this.methodList.size() != 0){
            str += "    " + this.SEP;
        }
        return str;
    }
    //}}}
}
class Member {
    String ownerClass;
    Access access;
    String memberName;
}
/**  
 * Use new structure to represent a method of class */
class MethodMember extends Member {
    String returnType;
    ArrayList<Argument> argList = new ArrayList<Argument> ();

    public String toString () {
        String str, indent;
        indent = "        - ";
        str = "Method:\t" + this.access.getSpecifier()+"(" + this.access.getNotation()+") "+ this.returnType  + " " + this.memberName + " ";
        int len = this.argList.size();
        if (len > 0) {
            str += "(";
            for (int i = 0 ; i < len ; i++ ) {
                if ( i < len - 1) {
                    str += argList.get(i).toString() + ",";
                } else {
                    str += argList.get(i).toString() + ");\n";
                }
            }
        } else {
            str += "(); \n";
        }
        return indent + str;
    }
}
/** Use new structure to represent a field of class */
class FieldMember extends Member {
    String type;
     /** We also use UML notation to denote access for abbreviation 
     *   public (+), protected(#), private(-) or package-Access(~) 
     * */
    public String toString () {
//{{{
        String str, indent;
        indent = "        - ";
        str = "Field:\t" + this.access.getSpecifier()+"(" + this.access.getNotation()+") " + this.type + " "+ this.memberName + "; \n"; 
    //}}}
    return indent + str;
    }
}

class Argument {
    String ownerMethod;
    String type;
    String argName;
    public Argument(String method, String argtype, String argname){
        this.ownerMethod = method;
        this.type = argtype;
        this.argName = argname;
    }
    public String toString(){
        String str = "";
        str += type + " " + argName;
        return str;
    }

}
/** Use enumerable access for class, method, field */
enum Access {
//{{{
    PUBLIC("public","+"), PROTECTED("protected","#"), PRIVATE("private","-"), PACKAGE_ACCESS("package","~");
    private final String specifier;
    private final String notation;
    Access(String spec, String note){
        this.specifier = spec ;
        this.notation = note;
    }
    String getSpecifier(){
        return this.specifier;
    }
    String getNotation(){
        return this.notation;
    }
//}}}
}
/** Define line type which is also enumerable */
enum LineType {
    COMMENT, BLANK, CODE, BRACE, UNKNOWN
}

public class Signature { 
    /**
     * @param a string containing information of one line .
     * @return the type of this line in the form of LineType enumeration.
     * */
    public static LineType getClassification(String CurrentLine){
        // preprocessing for this line of string//{{{
        String NoSpaceStr = CurrentLine.replace(" ","");
        String NoTabStr = NoSpaceStr.replace("\t","");
        
        /** Assign value to the string to be examined */
        String ProcessedStr = NoTabStr; 
        /** Test statement */
        // System.out.println("["+ProcessedStr.length()+"]"+ProcessedStr);
        /** Start Examination */
        if (ProcessedStr.length() == 0)
            return LineType.BLANK;
        else if (ProcessedStr.charAt(0) == '/' || ProcessedStr.charAt(0) == '*')
            return LineType.COMMENT;
        else if ((ProcessedStr.charAt(0) == '{' || ProcessedStr.charAt(0) == '}' ) &&  ProcessedStr.length() == 1)
        {
            // remove { and }, see what is the leftover 
            ProcessedStr = ProcessedStr.replace("{","");
            ProcessedStr = ProcessedStr.replace("}","");
            LineType temp;
            temp = getClassification(ProcessedStr);
            if (temp == LineType.CODE)
                // if the leftover is acutally code, we should regard this line as code line
                return LineType.CODE;
            else if (temp == LineType.COMMENT)
                // if the leftover character is comment, we just view this line as comment line instead of brace
                return LineType.COMMENT;
            else return LineType.BRACE;
        }
        else 
            return LineType.CODE;//}}}
    }

    public static boolean isValidToken(String value){
//{{{
        char temp;
        if (value.length() == 0) return false;
        /** No special notation except < and >, but < and > cannot be only char*/
        for (int i = 0 ; i < value.length(); i++) {
            temp = value.charAt(i);
            if ((temp == '<' || temp == '>')) {
                continue;
            }
            if ( (temp >= 48 && temp <= 57) || (temp >= 65 && temp <= 90) || (temp >= 97 && temp <=122) ) {
                continue;
            } 
            else return false;
        }
        /** No key word */
        String keywords [] = {"if","else","catch","return","false","true","public","private","protected","new","final","throws"};
        for (int i = 0; i< keywords.length; i++){
            if (keywords[i].compareTo( value) == 0) return false;
            else continue;
        }
//}}}
        /** Proven to be valid, true returned. */
        return true;
    }

    public static boolean isSeparator(String value){
        final String sep [] = {",","(",")"};
        for (int i = 0 ; i < sep.length; i++){
            if (sep[i].compareTo(value) == 0) {
                return true;
            } else {
                continue;
            }
        }
        return false;
    }
    /** Search Access Specifier on given string */
    public static Access searchAccess(String [] subStringList, int endIndex){
//{{{
        Access access;
        access = Access.PACKAGE_ACCESS;
        for (int j = 0; j < endIndex ; j++){
            if (subStringList[j].compareTo("public") == 0) {
                access = Access.PUBLIC;
                break;
            } else if (subStringList[j].compareTo("private") == 0){
                access = Access.PRIVATE;
                break;
            } else if (subStringList[j].compareTo("protected") == 0){
                access = Access.PROTECTED;
                break;
            }
        }
//}}}
        return access;
    }
    public static Record parseLine(String CurrentLine, Record CurrentFile){
        int codeUntilIndex = -1;
        String ProcessedStr = CurrentLine;
        Class currentClass;
        MethodMember currentMethod;
        FieldMember currentField;
        boolean parseForMethod = false;

        /** remove some elements of the string */
//{{{
        String subStringList [] = ProcessedStr.split("\"");
        ProcessedStr = "";
        int n = (subStringList.length + 1) /2;
        for (int i = 0 ; i < n ; i ++ ){
            ProcessedStr +=  subStringList[2*i];
        }
        ProcessedStr = ProcessedStr.replace("{"," { ");
        ProcessedStr = ProcessedStr.replace("}"," } ");
        ProcessedStr = ProcessedStr.replace("("," ( ");
        ProcessedStr = ProcessedStr.replace(")"," ) ");
        ProcessedStr = ProcessedStr.replace(">","> "); // for hash table
        ProcessedStr = ProcessedStr.replace("="," = ");
        ProcessedStr = ProcessedStr.replace(";"," ; ");
        ProcessedStr = ProcessedStr.replace("\t"," ");
        ProcessedStr = ProcessedStr.replace("  "," ");
//}}}

        /** Ignore the // by truncating the subString after // */
        codeUntilIndex = ProcessedStr.indexOf("//");
        if (codeUntilIndex != -1){
            // get ride of inline comment
            ProcessedStr = ProcessedStr.substring(0,codeUntilIndex);
        }

        /** Split the Pre-Processed string */
        subStringList = ProcessedStr.split(" ");
        /** Pre-estimate the necessity of precise parsing for method: */
        if (ProcessedStr.indexOf("(")!= -1 ) {
            parseForMethod = true; // ( is a must for method signature
        }
        for (int i = 0 ; i < subStringList.length ; i ++) {
            if (subStringList[i].compareTo("{") == 0){
                CurrentFile.braceRegister ++;
            } else if (subStringList[i].compareTo("}") == 0){
                CurrentFile.braceRegister --;
            }
            /** Detect class */
//{{{
            if ( (subStringList[i].compareTo("class") == 0 || subStringList[i].compareTo("enum") == 0 ) && i + 1 < subStringList.length) {
                String classIdentifier = subStringList[i+1];
                if (CurrentFile.isClassInList(classIdentifier)) {
                    System.out.println("Dupliated class, please check the validation of java file");
                    System.exit(1);
                } else  {
                    currentClass = new Class(classIdentifier);
                    CurrentFile.classList.add(currentClass);
                    CurrentFile.currentClassName = classIdentifier;
                    /** Work out the access of this class */
                    currentClass.access = searchAccess(subStringList,i);
                }
            }
//}}}
            /** Detect method */
//{{{
            int braceIndex = ProcessedStr.indexOf("(");
            boolean isMethod = true;
            boolean isConstructor = true;
            if (parseForMethod && subStringList[i].compareTo("(") == 0 ){
                // has '('  , it could be method signature, we need to further examine it...
                String methodIdentifier = subStringList[i-1];
                String returnType = subStringList[i-2];
                if ( i - 2 < 0 )  {
                    isMethod = false;
                }  else {
                    currentMethod = new MethodMember ();
                    currentMethod.memberName = methodIdentifier;
                    isConstructor = (returnType.compareTo("public") == 0 && methodIdentifier.compareTo(CurrentFile.currentClassName) == 0);

                    isMethod = Signature.isValidToken(methodIdentifier) && Signature.isValidToken(returnType);
                    if (isMethod || isConstructor){
                        /** Create new object of method */
                        currentMethod = new MethodMember ();
                        /** Assignment */
                        currentMethod.memberName = methodIdentifier;
                        currentMethod.returnType = returnType;
                        if (isConstructor){
                            currentMethod.returnType = "*Constructor";
                        }

                        /** Find access of this method */
                        currentMethod.access = searchAccess(subStringList,i);

                        /** Add arguments of this method to argList */
                        for (int k = i + 1; Signature.isValidToken(subStringList[k]) && Signature.isValidToken(subStringList[k+1]) ; k++ ) {
                            String argType = subStringList[k];
                            String argName = subStringList[k+1];
                            Argument arg = new Argument(methodIdentifier,argType, argName);
                            if (arg != null)
                                currentMethod.argList.add(arg);
                        }

                        /** Add to the method list of that class */
                        currentMethod.ownerClass = CurrentFile.currentClassName;
                        CurrentFile.addMethod(currentMethod);
                    }
                }
            }
//}}}
            /** Detect field */
//{{{
            /** Evaluation for qualification of one line being further parsed as field */
            if (CurrentFile.braceRegister == 1 && ProcessedStr.indexOf(";") != -1 && i + 1 < subStringList.length && i + 2< subStringList.length){
                /** Evaluation for validity of being a field */
                String exp0 = subStringList[i];
                String exp1 = subStringList[i+1];
                String exp2 = subStringList[i+2];

                if ( Signature.isValidToken(exp0) && Signature.isValidToken(exp1) && !Signature.isSeparator(exp2) ){
                    /** Deal with the special case of hashtable */
                    if ( exp0.indexOf(">") != -1 && exp0.indexOf("<") == -1 ) {
                        exp0 = subStringList[i-1] + exp0;
                    }
                    /** Add to the field list of that class */
                    currentField = new FieldMember();
                    currentField.type = exp0;
                    currentField.memberName = exp1;
                    currentField.access = Signature.searchAccess(subStringList,i);
                    currentField.ownerClass = CurrentFile.currentClassName;
                    CurrentFile.addField(currentField);
                }
            }
//}}}
        }// end loop
        return CurrentFile;
    }

    /**
     * @param filename - the name of java code file whose lines are to be examined (path included) 
     * @return - a Record structure that records the information of opened file.
     * */
    public static Record examineFile(String filename) throws IOException{
        Record currentFile = new Record(filename);//{{{
        BufferedReader br ;
        try{
             br = new BufferedReader(new FileReader(filename));
        } catch (IOException e){
            /** exception handling for failure to open */
            System.out.println("    ERROR: cannot open "+ filename);
            return null;
        }

        LineType temp;
        String currentLine; 
        currentLine = br.readLine() ;
        while (currentLine != null){
            temp =  getClassification(currentLine);
            currentFile.lineOfTotal += 1;
            if ( temp == LineType.COMMENT ) { 
                currentFile.lineOfComments += 1;
            } else if ( temp == LineType.CODE ) {
                currentFile.lineOfCodes += 1;
                currentFile = parseLine(currentLine,currentFile );
            } else if ( temp == LineType.BLANK ) {
                currentFile.lineOfBlanks += 1;
            } else if ( temp == LineType.BRACE ) {
                currentFile.lineOfBraceOnly += 1;
                for (int i = 0; i< currentLine.length(); i ++){
                    if (currentLine.charAt(i) == '{') {
                        currentFile.braceRegister ++;
                    } else if (currentLine.charAt(i) == '}'){
                        currentFile.braceRegister --;
                    }
                }
            }
            currentLine = br.readLine();
        }
        br.close(); // close the BufferedReader object//}}}
        return currentFile;
    }
    /** Help documentaiton */
    public void usage(){
        String usageString = "";
        usageString += "1";
        System.out.println(usageString);
        return ;
    }
    
    /**
     * */
    public static void main (String [] args) throws IOException {
//{{{
        int numOfFiles = 0 ;
        int numOfCodeLines = 0 ;

        String indentation = "==========================================================================\n";
        Record tempRecord ;
        Record oneBigRecord;
        String recordLevelSum = "";
        String overview = "";

        oneBigRecord = new Record("Just for overall statistics");
        for ( int i = 0 ; i < args.length ; i++ ) {
            // System.out.println(i +":"+args[i]);
            /** give disregard to those files that are not .java */
            if (args[i].indexOf(".java") < 0){ 
                System.out.println("    ERROR: cannot open "+args[i]);
                continue;
            }

            /** invoke the method of opening java source file */
            tempRecord = examineFile(args[i]); 
            if (tempRecord == null) continue;

            /** file open succeed... */
            oneBigRecord.classList.addAll(tempRecord.classList);
            recordLevelSum += tempRecord.toString_hm03_sum();
            numOfFiles += 1;
            numOfCodeLines += tempRecord.lineOfCodes;
            System.out.println(indentation);
            System.out.println(tempRecord.toString_hm02());
            System.out.println(tempRecord.toString_hm03()); 
            }
        System.out.println(recordLevelSum);
        overview = "Summary: \n    " + numOfFiles + " java files, " + numOfCodeLines + " lines of code\n";
        overview += oneBigRecord.toString_fieldSummary() + oneBigRecord.toString_methodSummary();
        System.out.println(overview);
        //}}}
        }

}

