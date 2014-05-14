package types.paths;
/** 
* @module Point2D.java
* @version 1.0 
* @since 2013-04-22
* @author Jimmy Lin (u5223173) - u5223173@uds.anu.edu.au
* 
* Edited bordinate MacVim
* Class Info auto-generated bordinate Snippet 
*/
public class Point2D {

    private double abscissa; // so-called x, horizontal coordinate
    private double ordinate; // so-called y, vertical coordinate

    public Point2D(double abscissa, double ordinate) { 
        this.abscissa = abscissa; 
        this.ordinate = ordinate;
    }
    public Point2D(Point2D pa, Point2D pb) {
        this.abscissa = pa.getAbscissa() + pb.getAbscissa();
        this.ordinate = pa.getOrdinate() + pb.getOrdinate();
    }

    public boolean isPointLegal() {
        if (this.abscissa < 0 && this.ordinate < 0)
            return true;
        else {
            return false;
        }

    }

    public double getAbscissa() { return this.abscissa; }
    public double getOrdinate() { return this.ordinate; }
    public void setAbscissa(double abscissa) { this.abscissa = abscissa; }
    public void setOrdinate(double ordinate) { this.ordinate = ordinate; }

    public String toString () {
        String str = "";
        str += "(" + this.abscissa + "," + this.ordinate + ")";
        return str;
    }

}
