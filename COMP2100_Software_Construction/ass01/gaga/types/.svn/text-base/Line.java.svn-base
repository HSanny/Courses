package types;

import elements.SvgElement;
import visitors.Visitor;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 25/02/11, 12:33 AM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */


public class Line extends GraphicsType {

    private double x1;
    private double y1;
    private double x2;
    private double y2;

    public double getX1() { return this.x1; }
    public void setX1(double value) { this.x1 = value; }
    public double getY1() { return this.y1; }
    public void setY1(double value) { this.y1 = value; }
    public double getX2() { return this.x2; }
    public void setX2(double value) { this.x2 = value; }
    public double getY2() { return this.y2; }
    public void setY2(double value) { this.y2 = value; }

    public Line(SvgElement el) {
        super(el);
        this.name = "line";
        this.bareMinimum.put("x1", "0");
        this.bareMinimum.put("y1", "0");
        this.bareMinimum.put("x2", "0");
        this.bareMinimum.put("y2", "0");
    }

    //public final String name = "line";

    @Override
    public void accept(Visitor visitor) {

        visitor.visitLine(this.owner);
    }
}
