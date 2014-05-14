package types;

import elements.SvgElement;
import visitors.Visitor;

/**
 * @author abx
 * @author $Author: u8600417 $
 *         Created: 22/02/11, 11:57 PM
 * @version $Rev: 1532 $ ($Date: 2011-05-04 11:51:55 +1000 (Wed, 04 May 2011) $)
 *          Repository: $URL$
 */
public class Polyline extends GraphicsType {
    private double x;
    private double y;
    private boolean xAssigned;
    private boolean yAssigned;
    private Polyline rest;

    public Polyline(SvgElement el) {
        super(el);
        this.name = "polyline";
    }
    public Polyline(SvgElement el, double x, double y, Polyline rest) {
        super(el);
        this.name = "polyline";
        this.x = x;
        this.y = y;
        this.xAssigned = true;
        this.yAssigned = true;
        this.rest = rest;
    }

    public void addValue(double val) {
        //As convention, coordinates are added as x of this point first, then y of this point
        if (!this.xAssigned) {
            this.x = val;
            this.xAssigned = true;
        }
        else if (this.xAssigned && !this.yAssigned) {
            this.y = val;
            this.yAssigned = true;
        } else {
            Polyline temp = new Polyline (this.owner, this.x, this.y, this.rest);
            this.clear();
            this.x = val;
            this.xAssigned = true;
            this.rest = temp;
        }
        return ;
    }

    public void clear() {
        this.x = 0;
        this.y = 0;
        this.xAssigned = false;
        this.yAssigned = false;
        this.rest = null;
    }
    public boolean isPointValid(){
        if (this.xAssigned && this.yAssigned) {
            return true;
        } else {
            return false;
        }
    }

    public boolean isDrawable() {
        if (this.rest == null) {
            return false;
        }
        if (this.isPointValid() && this.rest.isPointValid()) {
            return true;
        } else {
            return false;
        }
    }


    public String toString() {
        String str = "[";
        str += this.x + "," + this.y + "]";
        return str;
    }
    public double getX() { return this.x; }
    public double getY() { return this.y; }
    public boolean getXAssigned() {return this.xAssigned; }
    public Polyline getRest() { return this.rest; }

    //public final String name = "polyline";

    @Override
    public void accept(Visitor visitor) {
        visitor.visitPolyline(this.owner);
    }
}
