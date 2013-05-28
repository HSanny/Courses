package comp2100.oops.tree;

import java.io.*;
import java.util.Hashtable;
import java.util.Enumeration;
import comp2100.oops.StringOps;

/**
 * Lists of name-value pairs that belong to XML tags (and later
 * to the the corresponding XML elements).
 *   
 * @author Ian Barnes (original Eiffel code)
 * @author Alexei Khorev (Java port)
 * @version $Revision: 2005.5 $, $Date: 2005/04/12 01:10:37 $
 */
public class AttributeList {
    
    /** Initialise the storage */
    public AttributeList() {
	this.storage = new Hashtable<String,String>();
    }
    
    /** Table to store name-value pair of actual attributes */
    private Hashtable<String, String>storage;
    
    /**
     * Add a name-value pair to the list of attributes 
     *  
     * @param n The key, aka name
     * @param v The value
     * @return The old value stored under the name, null if
     * there wasn't one
     */
    public String add(String n, String v) {
	String s = null;
	try {
	    s = storage.put(n, v);
	} catch (NullPointerException e) {
	    System.err.println(e.getMessage() + " is given as a parameter");
	}
	return s;
    }
    
    /** The list of attributes as space separated pairs name="value" */
    public String toString() {
	String s = new String(), n, v;
	String pad = comp2100.oops.StringOps.spaces(6);
	Enumeration <String>e = storage.keys();
	if (e != null) {
	    while (e.hasMoreElements()) {
		n = e.nextElement();
		v =  storage.get(n);
		s += "   " +  "ATTRIBUTE" + "\n" + pad + "NAME = " + "\"" + n 
			+ "\"\n" + pad +  "VALUE = " + "\"" + v + "\"\n";
	    }
	}
	return s;
    }
    
    /**
     * Like ordinary toString(), but print every pair (except
     * the first) on a new line indented by ind spaces; used to
     * simplify the code of
     * @see XmlContainerElement#prettyPrint(int, OutputStreamWriter)
     *  
     * @param indent The number of spaces of indentation
     */
    public String toString(int indent) {
	String s = new String(), n, v;
	Enumeration<String> e = storage.keys();
	if (e != null) {
	    n =  e.nextElement();
	    v =  storage.get(n);
	    s += " " + n + "=\"" + v + "\"";
	    while (e.hasMoreElements()) {
		n =  e.nextElement();
		v =  storage.get(n);
		s += "\n" + StringOps.spaces(indent) + n 
		    + "=\"" + v + "\"";
	    }
	}	
	return s;
    }

    /** Is the list empty? */
    public boolean isEmpty() {
	return storage.isEmpty();
    }
    
    /** Does the list have a pair with this name?
     *
     * @param key The name to search for
     */
    public boolean has(String key) {
	return storage.containsKey(key);
    }
    
    /** 
     * Retrieve the value associated with this name.
     *
     * @param key The name (key) to search for
     * @return The value associated with the key, or null if
     * there is no entry with that key
     */
    public String at(String key) {
	return (String) storage.get(key);
    }
    
    /**
     * The main method for testing. Initialize the storage, make
     * a few entries, queries and print.
     */
    public final static void main(String[] args) throws IOException {
	
	String name, value;
	
	AttributeList al = new AttributeList();
	
	int indent;
	if (args.length == 0) {
	    indent = 1;
	} else {
	    indent = Integer.parseInt(args[0]);
	}
	
	name = "one";
	value ="1";
	al.add(name, value);
	
	name = "two";
	value ="2";
	al.add(name, value);
	
	name = "seven";
	value = "too_many";
	al.add(name, value);
	
	System.out.println("The value at the key " + name + " is " 
			   + al.at(name));
	name = "two";
	System.out.println("The value at the key " + name + " is " 
			   + al.at(name));
	
	System.out.println(al.toString());
	System.out.println(al.toString(indent));
    }
}
