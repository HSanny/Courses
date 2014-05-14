package types;

import elements.SvgElement;
import visitors.Visitor;

import java.util.TreeMap;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 22/02/11, 4:12 PM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Rectangle extends GraphicsType {
    private double x;
    private double y;
    private double width;
    private double height;

    public Rectangle(SvgElement el) {
        super(el);
        //this.bareMinimum = new TreeMap<String,String>();
        this.name = "rect";
        this.bareMinimum.put("x", "0");
        this.bareMinimum.put("y", "0");
        this.bareMinimum.put("width", "0");
        this.bareMinimum.put("height", "0");
    }

    public double getX() { return this.x; }
    public void setX(double value) { this.x = value; }

    public double getY() { return this.y; }
    public void setY(double value) { this.y = value; }

    public double getWidth() { return this.width; }
    public void setWidth(double value) { this.width = value; }

    public double getHeight() { return this.height; }
    public void setHeight(double value) { this.height = value; }
    //public final String name = "rect";

    @Override
    public void accept(Visitor visitor) {
        visitor.visitRectangle(this.owner);
    }
}
