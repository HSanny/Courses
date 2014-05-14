package types;

import elements.SvgElement;
import visitors.Visitor;

import java.util.TreeMap;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 22/02/11, 11:54 PM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Circle extends GraphicsType {

    private double cx;
    private double cy;
    private double r;
    
    public double getCX() { return this.cx; }
    public void setCX(double value) { this.cx = value; }
    public double getCY() { return this.cy; }
    public void setCY(double value) { this.cy = value; }
    public double getRadius() { return this.r; }
    public void setRadius(double value) { this.r = value; }

    public Circle(SvgElement el) {
        super(el);
        this.name = "circle";
        //this.bareMinimum = new TreeMap<String,String>();
        this.bareMinimum.put("cx", "0");
        this.bareMinimum.put("cy", "0");
        this.bareMinimum.put("r", "0");
    }

    //public final String name = "circle";

    @Override
    public void accept(Visitor visitor) {
        visitor.visitCircle(this.owner);
    }
}
