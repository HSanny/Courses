package types;
import visitors.*;
/**
 * Products of two expressions
 */
public class Multiplication extends Expression {
   
    /**
     * The expressions to be added.
     */
    private Expression left, right;
   
    /**
     * Initialise left and right sides
     */
    public Multiplication(Expression l, Expression r) {
	left = l;
	right = r;
    }
   
    /**
     * The value
     */
    public int getValue() {
	return 0; // Fix this in Exercise 1.
    }
    
    /**
     * String representation in in-order notation
     */
    public String toString() {
	return "(" + left.toString() + " * " +
	    right.toString() + ")";
    }

    /** 
     * open interface to other package to access left, right nodes
     * */
    public Expression getLeft() { return this.left; }
    public Expression getRight() { return this.right; }

    /** 
     * Newly added for visitor patterns:
     * */
    public void accept (Visitor visitor) {
        visitor.visitMultiplication(this);
    }
}
