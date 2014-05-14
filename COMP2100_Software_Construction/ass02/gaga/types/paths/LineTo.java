package types.paths;
import elements.SvgElement;
import types.Path;
import visitors.PathVisitor;
import java.util.ArrayList;
/** 
* @module LineTo.java
* @version 1.0 
* @since 2013-04-22
* @author Jimmy Lin (u5223173) - u5223173@uds.anu.edu.au
* 
* Edited by MacVim
* Class Info auto-generated by Snippet 
*/
public class LineTo extends Path {
    private Point2D point;

    public LineTo (SvgElement e, String command, double[] points) { 
        super(e);
        this.command = command;
        if (command.matches("[Ll]") ) {
            if (points.length >= 2)
                this.point = new Point2D (points[0], points[1]);
            if (points.length > 2) {
                double [] newPoints = new double [points.length - 2];
                for (int i = 0; i < newPoints.length ; i ++) {
                    newPoints[i] = points[i+2];
                }
                this.next = new LineTo (e, command, newPoints);
            }
        } else {
            if (command.matches("[Hh]")) {
                // y coordinate remain temporarily illegal
                this.point = new Point2D ( points[0] , -1); 

            } else if (command.matches("[Vv]")) {
                // x coordinate remain temporarily illegal
                this.point = new Point2D ( -1 , points[0]); 
            }
            if (points.length > 1) {
                double [] newPoints = new double [points.length - 1];
                for (int i = 0; i < newPoints.length ; i ++) {
                    newPoints[i] = points[i+1];
                }
                this.next = new LineTo (e, command, newPoints);
            }
        }
    }

    public double getAbscissa () { return this.point.getAbscissa(); }
    public double getOrdinate () { return this.point.getOrdinate(); }

    public String toString () {
        String str = "";
        str += "Line To: " + this.point.toString() ;
        return str;
    }

    // visitor pattern implementation
    public void acceptPV (PathVisitor visitor) {
        visitor.visitLineTo(this);
    }
}